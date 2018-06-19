#include <windows.h> 
#include <tchar.h>
#include <fcntl.h> 
#include <io.h>
#include <stdio.h> 
#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")

DWORD WINAPI EnviaPedidos(LPVOID param) {
	int i = 0;
	DWORD n = 0;
	HANDLE hPipe = (HANDLE)param;
	TCHAR buf[256];
	HANDLE IOReady;
	OVERLAPPED Ov;

	// Ler do teclado
	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	do {
		_tprintf(TEXT("[LEITOR] Pedido: "));
		_fgetts(buf, 256, stdin);
		buf[_tcslen(buf) - 1] = '\0';



		//Enviar PEdido ao escritor/GateWay
		ZeroMemory(&Ov, sizeof(Ov));
		ResetEvent(IOReady);
		Ov.hEvent = IOReady;

		WriteFile(hPipe, buf, (DWORD) _tcslen(buf) * sizeof(TCHAR), &n, &Ov);

		WaitForSingleObject(IOReady, INFINITE);
		if (!GetOverlappedResult(hPipe, &Ov, &n, TRUE)) {
			_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
			exit(-1);
		}

		_tprintf(TEXT("[LEITOR] Enviei %d bytes: '%s' (WriteFile)\n"), n, buf);
	} while (_tcscmp(buf, TEXT("fim")));

	return 0;
}
int _tmain(int argc, LPTSTR argv[]) {
//	TCHAR buf[256];
	int o = 0;
	HANDLE hPipe, IOReady;
	OVERLAPPED Ov;
	int i = 0;
	BOOL ret;
	DWORD n;
	HANDLE thread;
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	_tprintf(TEXT("[LEITOR] Esperar pelo pipe '%s' (WaitNamedPipe)\n"), PIPE_NAME);
	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (WaitNamedPipe)\n"), PIPE_NAME);
		return 1;
	}
	_tprintf(TEXT("[LEITOR] Ligação ao pipe do escritor... (CreateFile)\n"));
	hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);
	if (hPipe == NULL) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (CreateFile)\n"), PIPE_NAME);
		return 1;
	}
	_tprintf(TEXT("[LEITOR] Liguei-me...\n"));

	//ALTERAR MODO LEITURA DO PIPE PARA "MESSAGE"...
	DWORD modo = PIPE_READMODE_MESSAGE;
	SetNamedPipeHandleState(hPipe, &modo, NULL, NULL);
	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	thread = CreateThread(NULL, 0, EnviaPedidos, (LPVOID)hPipe, 0, NULL);
	while (1) {
		ZeroMemory(&Ov, sizeof(Ov));
		ResetEvent(IOReady);
		Ov.hEvent = IOReady;

		ReadFile(hPipe, (LPCVOID)&o, (DWORD) sizeof(int), (LPDWORD)&n, (LPOVERLAPPED)&Ov);
		WaitForSingleObject(IOReady, INFINITE);

		ret = GetOverlappedResult(hPipe, &Ov, &n, TRUE);
		if (!ret || !n) {
			_tprintf(TEXT("[LEITOR] %d %d... (ReadFile)\n"), ret, n);
			break;
		}
		_tprintf(TEXT("[LEITOR] Recebi %d bytes: '%d'... (ReadFile)\n"), n, o);
	}
	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);
	CloseHandle(hPipe);
	Sleep(200);
	return 0;
}