#include "..\DLL\DLL.h"

DWORD WINAPI threadProdutora(LPVOID param);
void printError(unsigned short * msg);
PTGAMEDATA initMemoriaPartilhadaJogo(PTGAMEDATAMS param);

int _tmain(int argc, LPTSTR agrv[]) {
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