#include "..\DLL\DLL.h"

HANDLE hClientes[MAX_PLAYERS_SIZE], hPipe;
BOOLEAN flagThread = TRUE;

void printError(unsigned short * msg);
void printLog(unsigned short * msg);
PTGAMEDATA initMemoriaPartilhadaJogo(PTGAMEDATAMS param);
DWORD WINAPI controlaPipes(LPVOID param);

DWORD WINAPI trataCliente(LPVOID param);
DWORD WINAPI RecebeCliente(LPVOID param);

GAMEDATAMS gameDataMemory;
PCS produtorData;
HANDLE hPipesMain;

int _tmain(int argc, LPTSTR agrv[]) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif // UNICODE
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

	hPipesMain = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaPipes, NULL, 0, NULL);
	if (hPipesMain == NULL) {
		printError(TEXT("hPipesMain = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProdutora, NULL, 0, NULL);"));
		return 1;
	}

	WaitForSingleObject(produtorData.handleThread, INFINITE);
	WaitForSingleObject(hPipesMain, INFINITE);

	CloseComunicacao(&produtorData);
	closeMemoriaPartilhadaJogo(&gameDataMemory);

	return 0;
}

void printError(unsigned short * msg) {
	_tprintf(TEXT("[Gateway]Error> '%s'\n"), msg);
}

void printLog(unsigned short * msg) {
	_tprintf(TEXT("[Gateway]Log> '%s'\n"), msg);
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

DWORD WINAPI controlaPipes(LPVOID param) {
	DWORD sizeTransferred;
	HANDLE tRecebeCliente, hCano, IOReady;
	OVERLAPPED overlapped;
	BOOLEAN segue = TRUE;
	GAMEDATA gamedataTemp;

	for (int i = 0; i < MAX_PLAYERS_SIZE; i++) {
		hClientes[i] = INVALID_HANDLE_VALUE;
	}

	tRecebeCliente = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebeCliente, NULL, 0, NULL);
	//_tprintf(TEXT("[ESCRITOR] Criar uma cópia do pipe '%s' ... (CreateNamedPipe)\n"), PIPE_NAME);

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	do {
		switch (gameDataMemory.gameData->estado) {
		case PREJOGO:
			/*_tprintf(TEXT("PREJOGO\n"));
			break;*/
		case A_JOGAR:
			/********************************************/
			/* Envia estado de jogo a todos os clientes */
			/********************************************/
			WaitForSingleObject(gameDataMemory.mutex, INFINITE);
			for (int i = 0; i < MAX_PLAYERS_SIZE; i++) {
				if (hClientes[i] != INVALID_HANDLE_VALUE) {
					ZeroMemory(&overlapped, sizeof(overlapped));
					ResetEvent(IOReady);
					overlapped.hEvent = IOReady;

					memcpy(&gamedataTemp, gameDataMemory.gameData, sizeof(GAMEDATA));

					WriteFile(hClientes[i], &gamedataTemp, sizeof(GAMEDATA), &sizeTransferred, &overlapped);

					WaitForSingleObject(IOReady, INFINITE);
					if (!GetOverlappedResult(hPipe, &overlapped, &sizeTransferred, TRUE)) {
						printError(TEXT("110: !GetOverlappedResult(hPipe, &overlapped, &sizeTransferred, TRUE)"));
						return 1;
					}
					printLog(TEXT("Enviei estado de jogo!"));
				}
			}
			ReleaseMutex(gameDataMemory.mutex);
			break;
		case POSJOGO:
			printLog(TEXT("POSJOGO"));
			break;
		case TERMINADO:
			segue = FALSE;
			break;
		default:
			printError(TEXT("switch (gameDataMemory.gameData->estado)"));
			break;
		}
		Sleep(1000);
	} while (segue);

	printLog(TEXT("[ESCRITOR] Desligar o pipe (DisconnectNamedPipe)"));
	flagThread = FALSE;

	hCano = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES, 10 * sizeof(TCHAR), 10 * sizeof(TCHAR), 1000, NULL);
	WaitForSingleObject(tRecebeCliente, INFINITE);

	for (int i = 0; i < MAX_PLAYERS_SIZE; i++) {
		if (hClientes[i] != INVALID_HANDLE_VALUE) {
			if (!DisconnectNamedPipe(hClientes[i])) {
				printError(TEXT("Desligar o pipe! (DisconnectNamedPipe)"));
				return 1;
			}
		}
	}

	CloseHandle(hPipe);
	return 0;
}

DWORD WINAPI RecebeCliente(LPVOID param) {
	int i = 0;
	while (flagThread) {
		i = 0;
		hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES, 10 * sizeof(TCHAR), 10 * sizeof(TCHAR), 1000, NULL);

		if (hPipe == INVALID_HANDLE_VALUE) {
			printError(TEXT("Criar Named Pipe! (CreateNamedPipe)"));
			return 1;
		}
		if (!ConnectNamedPipe(hPipe, NULL)) {
			printError(TEXT("Ligação ao leitor! (ConnectNamedPipe"));
			return 1;
		}

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)trataCliente, (LPVOID)hPipe, 0, NULL);
		while (hClientes[i] != INVALID_HANDLE_VALUE) {
			i++;
		}
		hClientes[i] = hPipe;
	}
	return 0;
}

DWORD WINAPI trataCliente(LPVOID param) {
	DWORD sizeTransferred;
	OVERLAPPED Overlapped;
	HANDLE IOReady;
	PLAYERINFO itemLido;

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	while (flagThread) {
		ZeroMemory(&Overlapped, sizeof(Overlapped));
		ResetEvent(IOReady);
		Overlapped.hEvent = IOReady;

		/****************************************************/
		/* Recebe jogada do cliente e envia para o servidor */
		/****************************************************/
		ReadFile((HANDLE)param, &itemLido, sizeof(PLAYERINFO), &sizeTransferred, &Overlapped);
		produzItem(&produtorData, itemLido);
		_tprintf_s(TEXT("[Gateway]Log> Produzi item do jogado '%s'.\n"), itemLido.nome);

		WaitForSingleObject(IOReady, INFINITE);

		if (!GetOverlappedResult((HANDLE)param, &Overlapped, &sizeTransferred, TRUE)) {
			printError(TEXT("211: !GetOverlappedResult((HANDLE)param, &Overlapped, &sizeTransferred, TRUE)"));
			break;
		}
		//_tprintf(TEXT("[ESCRITOR] Recebi %d bytes: '%d' (ReadFile)\n"), sizeTransferred, leitura);
	}
	return 0;
}