#include "..\DLL\DLL.h"
#include <sys/types.h>
#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")
HANDLE hTabela[10], hPipe;
TCHAR buf[256];
int flagThread = 1;

DWORD WINAPI threadProdutora(LPVOID param);
void printError(unsigned short * msg);
PTGAMEDATA initMemoriaPartilhadaJogo(PTGAMEDATAMS param);
DWORD WINAPI trataCliente(LPVOID param);
DWORD WINAPI RecebeCliente(LPVOID param);

int _tmain(int argc, LPTSTR agrv[]) {
	DWORD n, *tUm = NULL;
	int i;
	HANDLE thread;
	OVERLAPPED Ov;
	HANDLE hCano;
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif // UNICODE
	GAMEDATAMS gameDataMemory;
	PCS produtorData;

	//Criar memoria partilhada do jogo
	gameDataMemory.gameData = initMemoriaPartilhadaJogo(&gameDataMemory);
	if (gameDataMemory.gameData == NULL) {
		printError(TEXT("gameDataMemory.gameData = initMemoriaPartilhadaJogo(&gameDataMemory);"));
		return 1;
	}

	produtorData.bufferMemory = initComunicacaoGatewaySide(&produtorData.handlesBuffer);
	if (produtorData.bufferMemory == NULL) {
		printError(TEXT("bufferMemory = initComunicacaoGatewaySide(&handlesBuffer);"));
		return 1;
	}

	produtorData.handleThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProdutora, (LPVOID)&produtorData, 0, NULL);
	if (produtorData.handleThread == NULL) {
		printError(TEXT("produtorData.handleThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProdutora, (LPVOID)&produtorData, 0, NULL);"));
		return 1;
	}



	/*ligação client gateway*/

	for (int i = 0; i < 10; i++)
		hTabela[i] = INVALID_HANDLE_VALUE;

	thread = CreateThread(NULL, 0, RecebeCliente, NULL, 0, tUm);
	//PeekBuffer = CreateThread(NULL, 0, OlhaBuffer, /*PARAMETROS*/NULL, 0, PeekBufferID);
	_tprintf(TEXT("[ESCRITOR] Criar uma cópia do pipe '%s' ... (CreateNamedPipe)\n"), PIPE_NAME);
	HANDLE IOReady;

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);



	do {
		_tprintf(TEXT("[ESCRITOR] Frase: "));
		_fgetts(buf, 256, stdin);
		buf[_tcslen(buf) - 1] = '\0';
		for (int i = 0; i < 10; i++) {
			if (hTabela[i] != INVALID_HANDLE_VALUE) {
				ZeroMemory(&Ov, sizeof(Ov));
				ResetEvent(IOReady);
				Ov.hEvent = IOReady;

				WriteFile(hTabela[i], buf, _tcslen(buf) * sizeof(TCHAR), &n, &Ov);

				WaitForSingleObject(IOReady, INFINITE);
				BOOL ret = GetOverlappedResult(hPipe, &Ov, &n, NULL);
				if (!ret) {
					_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
					exit(-1);
				}
				_tprintf(TEXT("[ESCRITOR] Enviei %d bytes: '%s' (WriteFile)\n"), n, buf);
			}

		}

	} while (_tcscmp(buf, TEXT("fim")));
	_tprintf(TEXT("[ESCRITOR] Desligar o pipe (DisconnectNamedPipe)\n"));
	flagThread = 0;
	hCano = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES, 10 * sizeof(TCHAR), 10 * sizeof(TCHAR), 1000, NULL);
	WaitForSingleObject(thread, INFINITE);
	for (int i = 0; i < 10; i++) {
		if (hTabela[i] != INVALID_HANDLE_VALUE) {
			if (!DisconnectNamedPipe(hTabela[i])) {
				_tprintf(TEXT("[ERRO] Desligar o pipe! (DisconnectNamedPipe)"));
				exit(-1);
			}
		}
	}


	Sleep(2000);
	CloseHandle(hPipe);



	WaitForSingleObject(produtorData.handleThread, INFINITE);

	CloseComunicacao(&produtorData);
	closeMemoriaPartilhadaJogo(&gameDataMemory);

	return 0;
}

DWORD WINAPI threadProdutora(LPVOID param) {
	PTPCS data = (PTPCS)param;
	while (1) {
		WaitForSingleObject(data->handlesBuffer.semRead, INFINITE);
		WaitForSingleObject(data->handlesBuffer.mHandleBuffer, INFINITE);

		_tprintf(TEXT("[Gateway]> "));
		wscanf_s(TEXT("%d"), &data->bufferMemory->buffer[data->bufferMemory->nextIn]);

		if (data->bufferMemory->nextIn == BUFFER_SIZE - 1) {
			data->bufferMemory->nextIn = 0;
		}else {
			data->bufferMemory->nextIn++;
		}

		ReleaseMutex(data->handlesBuffer.mHandleBuffer);
		ReleaseSemaphore(data->handlesBuffer.semWrite, 1, NULL);

		Sleep(500);

	}
	return 0;
}

void printError(unsigned short * msg) {
	_tprintf(TEXT("[Gateway]Error> '%s'\n"), msg);
}

PTGAMEDATA initMemoriaPartilhadaJogo(PTGAMEDATAMS param) {
	param->mutex = OpenMutex(MUTEX_MODIFY_STATE, FALSE, MUTEX_ACCESS_GAME_MEMORY);
	if (param->mutex == NULL) {
		printError(TEXT("param->mutex = OpenMutex(MUTEX_MODIFY_STATE, FALSE, MUTEX_ACCESS_GAME_MEMORY);"));
		return NULL;
	}

	param->handle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, ACCESS_GAME_MEMORY_NAME);
	if (param->handle == NULL) {
		printError(TEXT("param->handle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, ACCESS_GAME_MEMORY_NAME);"));
		return NULL;
	}

	return (PTGAMEDATA)MapViewOfFile(param->handle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
}


/*----------------------------comunicação client gateway----------------------------*/


DWORD WINAPI RecebeCliente(LPVOID param) {
	int i = 0;

	while (flagThread) {
		i = 0;
		hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES, 10 * sizeof(TCHAR), 10 * sizeof(TCHAR), 1000, NULL);
		if (hPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
			exit(-1);
		}
		_tprintf(TEXT("[ESCRITOR] Esperar ligação de um leitor... (ConnectNamedPipe)\n"));
		if (!ConnectNamedPipe(hPipe, NULL)) {
			_tprintf(TEXT("[ERRO] Ligação ao leitor! (ConnectNamedPipe\n"));
			exit(-1);
		}
		CreateThread(NULL, 0, trataCliente, (LPVOID)hPipe, 0, NULL);
		while (hTabela[i] != INVALID_HANDLE_VALUE)
			i++;
		hTabela[i] = hPipe;


	}
	return 0;
}




DWORD WINAPI trataCliente(LPVOID param) {
	HANDLE pipezao = (HANDLE)param;
	DWORD n = 0;
	BOOL ret;
	TCHAR bufzao[256];
	OVERLAPPED Ov;
	HANDLE IOReady;
	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	_tprintf(TEXT("\n \nThread Lançada!"));
	while (1) {
		ZeroMemory(&Ov, sizeof(Ov));
		ResetEvent(IOReady);
		Ov.hEvent = IOReady;


		ReadFile(pipezao, bufzao, sizeof(bufzao), &n, &Ov);

		//bufzao[n / sizeof(TCHAR)] = '\0';
		WaitForSingleObject(IOReady, INFINITE);
		ret = GetOverlappedResult(pipezao, &Ov, &n, NULL);
		if (!ret || !n) {
			_tprintf(TEXT("ESCRITOR] %d %d... (ReadFile)\n"), ret, n);
			break;
		}
		bufzao[n / sizeof(TCHAR)] = '\0';
		_tprintf(TEXT("[ESCRITOR] Recebi %d bytes: '%s' (ReadFile)\n"), n, bufzao);
	}
	return 0;
}

