#include "..\DLL\DLL.h"

DWORD WINAPI threadConsumidora(LPVOID param);
void printError(unsigned short * msg);
PTGAMEDATA initMemoriaPartilhadaJogo(PTGAMEDATAMS param);

int _tmain(int argc, LPTSTR agrv[]) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif // UNICODE
	GAMEDATAMS gameDataMemory;
	PCS consumidorData;

	//Criar memoria partilhada do jogo
	gameDataMemory.gameData = initMemoriaPartilhadaJogo(&gameDataMemory);
	if (gameDataMemory.gameData == NULL) {
		printError(TEXT("gameDataMemory.gameData = initMemoriaPartilhadaJogo(&gameDataMemory);"));
		return 1;
	}

	//Iniciar memoria partilhada do jogo
	gameDataMemory.gameData->estado = PREJOGO;
	gameDataMemory.gameData->powerupsCount = 0;
	gameDataMemory.gameData->inimigosCount = 0;
	gameDataMemory.gameData->defensoresCount = 0;
	gameDataMemory.gameData->bombasCount = 0;

	//Iniciar tudo para o produtor consumidor
	consumidorData.bufferMemory = initComunicacaoServerSide(&consumidorData.handlesBuffer);
	if (consumidorData.bufferMemory == NULL) {
		printError(TEXT("bufferMemory = initComunicacaoServerSide(&handlesBuffer);"));
		return 1;
	}

	//Cria thread para o consumidor
	consumidorData.handleThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsumidora, (LPVOID)&consumidorData, 0, NULL);
	if (consumidorData.handleThread == NULL) {
		printError(TEXT("hTreadConsumidora = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsumidora, (LPVOID)&consumidorData, 0, NULL);"));
		return 1;
	}



	WaitForSingleObject(consumidorData.handleThread, INFINITE);

	CloseComunicacao(&consumidorData);
	closeMemoriaPartilhadaJogo(&gameDataMemory);

	return 0;
}

DWORD WINAPI threadConsumidora(LPVOID param) {
	PTPCS data = (PTPCS)param;

	while (1) {
		WaitForSingleObject(data->handlesBuffer.semWrite, INFINITE);
		WaitForSingleObject(data->handlesBuffer.mHandleBuffer, INFINITE);

		_tprintf(TEXT("Recebi [%d]\n"), data->bufferMemory->buffer[data->bufferMemory->nextOut]);
		if (data->bufferMemory->nextOut == BUFFER_SIZE - 1) {
			data->bufferMemory->nextOut = 0;
		}else {
			data->bufferMemory->nextOut++;
		}

		ReleaseMutex(data->handlesBuffer.mHandleBuffer);
		ReleaseSemaphore(data->handlesBuffer.semRead, 1, NULL);
	}
	return 0;
}

void printError(unsigned short * msg) {
	_tprintf(TEXT("[Server]Error> '%s'\n"), msg);
}

PTGAMEDATA initMemoriaPartilhadaJogo(PTGAMEDATAMS param) {
	param->mutex = CreateMutex(NULL, FALSE, MUTEX_ACCESS_GAME_MEMORY);
	if (param->mutex == NULL) {
		printError(TEXT("param->mutex = CreateMutex(NULL, FALSE, MUTEX_ACCESS_GAME_MEMORY);"));
		return NULL;
	}

	param->handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(GAMEDATA), ACCESS_GAME_MEMORY_NAME);
	if (param->handle == NULL) {
		printError(TEXT("param->handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(GAMEDATA), ACCESS_GAME_MEMORY_NAME);"));
		return NULL;
	}

	return (PTGAMEDATA)MapViewOfFile(param->handle, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
}
