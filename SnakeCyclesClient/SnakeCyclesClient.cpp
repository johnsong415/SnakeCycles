#include "framework.h"
#include "SnakeCyclesClient.h"
#include "Screen.h"
#include "Communication.h"
#include "Types.h"

#include <string>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
bool Screen::m_programValid;
SOCKET m_socket;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
InitInstanceRet                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SNAKECYCLESCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    InitInstanceRet returned = InitInstance(hInstance, nCmdShow);
    if (!(returned.initiated))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SNAKECYCLESCLIENT));
    /* Default window stuff done */
    /* Connect to server */
    WSADATA winSockInfo;

    HWND hWnd = returned.hWnd;
    Screen* screen = new Screen(GetDC(hWnd));
    Screen::m_programValid = true;

    WORD version = MAKEWORD(2, 2);
    int initialized = WSAStartup(version, &winSockInfo);
    if (initialized != 0) {
        int errCode = WSAGetLastError();
        printf("WSA start up failed - Error code: %d\n", errCode);
        return 0;
    }

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        int errCode = WSAGetLastError();
        printf("Socket creation failed - Error code: %d\n", errCode);
        WSACleanup();
        return 0;
    }

    ULONG buffer;
    IN_ADDR transportAddress;
    inet_pton(AF_INET, "127.0.0.1", &buffer);
    transportAddress.S_un.S_addr = buffer;


    sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(8989);
    socketAddress.sin_addr = transportAddress;
    int success = connect(m_socket, reinterpret_cast<sockaddr*>(&socketAddress), sizeof(sockaddr));
    if (success != 0) {
        int errCode = WSAGetLastError();
        printf("Connect failed - Error code: %d\n", errCode);
        WSACleanup();
        closesocket(m_socket);
        return 0;
    }
    /* server connect done */
    
    MSG msg;
    //bool isOpen = true;

    std::chrono::steady_clock::time_point timeStart = std::chrono::steady_clock::now();

    // Main message loop:
    while (Screen::m_programValid) {
        RecvCommand received;
        int bytesRead = recv(m_socket, reinterpret_cast<char*>(&received), sizeof(received), 0);
        if (bytesRead == SOCKET_ERROR) {
            int errCode = WSAGetLastError();
            if (errCode == 10054) {
                break;
            }
            else {
                printf("Recv failed - Error code: %d\n", errCode);
                WSACleanup();
                closesocket(m_socket);
                return 0;
            }
        }
        if (bytesRead == 0) {
            break;
        }
        while (bytesRead < sizeof(RecvCommand)) {
            bytesRead += recv(m_socket, &(reinterpret_cast<char*>(&received)[bytesRead]), sizeof(RecvCommand) - bytesRead, 0);
        }
        if (received.recvedCommand == NEWLOC) {
            screen->UpdatePoint(received.coordinates);
            screen->PrintBorders();
        }
        else if (received.recvedCommand == STARTGAME) {
            screen->DefineTeam(received.coordinates.team);
        }
        else if (received.recvedCommand == ENDGAME) {
            break;
        }
        if (PeekMessage(&msg, nullptr, 0, 0, 1)) {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
    std::chrono::steady_clock::duration timeElapsed = timeNow - timeStart;
    int seconds = (int)(std::chrono::duration_cast<std::chrono::milliseconds>(timeElapsed)).count() / 1000;
    // Quit message
    LPCWSTR message = L"The game ended.";
    LPCWSTR quit = L" seconds. Press Q to quit.";
    LPCWSTR lasted = L"Program ran for ";
    std::wstring secondsString = std::to_wstring(seconds);
    TextOut(GetDC(hWnd), TA_CENTER, TA_TOP, message, 15);
    TextOut(GetDC(hWnd), TA_CENTER, 15, lasted, 16);
    TextOut(GetDC(hWnd), TA_CENTER, 30, &(secondsString[0]), secondsString.length());
    TextOut(GetDC(hWnd), TA_CENTER, 45, quit, 26);

    Screen::m_programValid = true;
    while (Screen::m_programValid) {
        if (PeekMessage(&msg, nullptr, 0, 0, 1)) {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    

    WSACleanup();
    closesocket(m_socket);
    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SNAKECYCLESCLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SNAKECYCLESCLIENT);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//              
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
InitInstanceRet InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 600, 700, nullptr, nullptr, hInstance, nullptr);
    // width (CW_USEDEFAULT) to 500
    // height (0) to 500
    InitInstanceRet ret;
    ret.hWnd = hWnd;

    if (!hWnd) { ret.initiated = false; }
    else { ret.initiated = true; }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return ret;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        SendCommand toSend;
        toSend.sendCommand = DIRECTIONCHANGE;
        if (wParam == VK_LEFT) {
            toSend.newDirection = LEFT;
            SendToServer(m_socket, toSend);
        }
        else if (wParam == VK_RIGHT) {
            toSend.newDirection = RIGHT;
            SendToServer(m_socket, toSend);
        }
        else if (wParam == VK_UP) {
            toSend.newDirection = UP;
            SendToServer(m_socket, toSend);
        }
        else if (wParam == VK_DOWN) {
            toSend.newDirection = DOWN;
            SendToServer(m_socket, toSend);
        }
        else if (wParam == 0x51) {
            Screen::m_programValid = false;
        }
        else if (wParam == 0x58) {
            Screen::m_programValid = false;
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
