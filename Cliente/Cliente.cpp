// Cliente.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Cliente.h"
#include "../DLL/DLL.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

BOOLEAN continua = TRUE;
PLAYERINFO aEnviar;
GAMEDATA estadoJogo;

HANDLE hPipe;
DWORD modo;

BOOLEAN firstTime = TRUE;

DWORD WINAPI recebeEstadoJogo(LPVOID param) {
	OVERLAPPED Overlapped;
	DWORD sizeTransferred;
	HANDLE IOReady;

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	while (continua) {
		ZeroMemory(&Overlapped, sizeof(OVERLAPPED));
		ResetEvent(IOReady);
		Overlapped.hEvent = IOReady;

		ReadFile(hPipe, &estadoJogo, sizeof(GAMEDATA), &sizeTransferred, &Overlapped);
		WaitForSingleObject(IOReady, INFINITE);

		if (!GetOverlappedResult(hPipe, &Overlapped, &sizeTransferred, TRUE)) {
			return 1;
		}
		/**********************************************/
		/* Recebeu e deverá imprimir o estado de jogo */
		/**********************************************/
		switch (estadoJogo.estado) {
		case PREJOGO:
			if (firstTime) {
				for (int i = 0; i < MAX_PLAYERS_SIZE; i++) {
					if (estadoJogo.defensores[i].jogador.id == 0) {
						estadoJogo.defensores[i].jogador.id = (i + 1);
						aEnviar.id = (i + 1);
						break;
					}
				}
				wcscpy_s(aEnviar.nome, TEXT("Carlos"));
				firstTime = FALSE;
			}
			break;
		case A_JOGAR:
			break;
		case POSJOGO:
			break;
		case TERMINADO:
			continua = FALSE;
			break;
		default:
			break;
		}
	}
	return 0;
}

BOOLEAN mandaJogada(TECLA tecla) {
	DWORD sizeTransferred;
	OVERLAPPED Overlapped;
	HANDLE IOReady;

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	aEnviar.jogada = tecla;

	ZeroMemory(&Overlapped, sizeof(OVERLAPPED));
	ResetEvent(IOReady);
	Overlapped.hEvent = IOReady;

	WriteFile(&hPipe, &aEnviar, sizeof(PLAYERINFO), &sizeTransferred, &Overlapped);

	WaitForSingleObject(IOReady, INFINITE);
	if (!GetOverlappedResult(&hPipe, &Overlapped, &sizeTransferred, TRUE)) {
		return FALSE;
	}

	return TRUE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENTE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENTE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENTE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLIENTE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BITMAP bmNave;
	static HDC hdcNave;
	static HBITMAP hNave = NULL;
	static HDC hdc = NULL;
	static HDC auxDC = NULL;
	static HBITMAP auxBM = NULL;
	static HBRUSH bg = NULL;
	static int nX = 0, nY = 0;
	static int x = 50, y = 50;

	HANDLE thread;

	PAINTSTRUCT ps;

    switch (message)
    {

	case WM_CREATE:
		HANDLE IOReady;

		bg = CreateSolidBrush(RGB(255, 128, 128));
		nX = GetSystemMetrics(SM_CXSCREEN);
		nY = GetSystemMetrics(SM_CYSCREEN);


		hdc = GetDC(hWnd);
		auxDC = CreateCompatibleDC(hdc);
		auxBM = CreateCompatibleBitmap(hdc, nX, nY);
		SelectObject(auxDC, auxBM);
		SelectObject(auxDC, bg);
		PatBlt(auxDC, 0, 0, nX, nY, PATCOPY);


		hNave = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		GetObject(hNave, sizeof(bmNave), &bmNave);
		hdcNave = CreateCompatibleDC(hdc);
		SelectObject(hdcNave, hNave);
		ReleaseDC(hWnd, hdc);

		if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
			return 1;
		}

		hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);
		if (hPipe == NULL) {
			return 1;
		}


		modo = PIPE_READMODE_MESSAGE;
		SetNamedPipeHandleState(hPipe, &modo, NULL, NULL);
		IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

		thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recebeEstadoJogo, NULL, 0, NULL);

		break;


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



	case WM_KEYDOWN:
		if (wParam == VK_RIGHT)
		{
			//mandaJogada(DIREITA);
			aEnviar.jogada = DIREITA;
			WriteFile(&hPipe, &aEnviar, sizeof(PLAYERINFO), NULL, NULL);
		}
		else if (wParam == VK_UP)
		{
			mandaJogada(CIMA);
		}
		else if (wParam == VK_LEFT)
		{
			mandaJogada(ESQUERDA);
		}
		else if (wParam == VK_DOWN)
		{
			mandaJogada(BAIXO);
		}
		

		break;


    case WM_PAINT:
        {

		PatBlt(auxDC, 0, 0, nX, nY, PATCOPY);

		SetStretchBltMode(auxDC, BLACKONWHITE);
		StretchBlt(auxDC, x, y, 50, 25, hdcNave, 0, 0, bmNave.bmWidth, bmNave.bmHeight, SRCCOPY);

		// COPIA INFORMACAO DO 'DC' EM MEMORIA PARA O DISPLAY... 
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, nX, nY, auxDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);


        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
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
