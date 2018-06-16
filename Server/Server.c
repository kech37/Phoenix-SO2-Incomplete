#include "..\DLL\DLL.h"

DWORD WINAPI threadConsumidora(LPVOID param);
void printError(short * msg);

int _tmain(int argc, LPTSTR agrv[]) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif // UNICODE
	PCS consumidorData;

	consumidorData.bufferMemory = initComunicacaoServerSide(&consumidorData.handlesBuffer);
	if (consumidorData.bufferMemory == NULL) {
		printError(TEXT("bufferMemory = initComunicacaoServerSide(&handlesBuffer);"));
		return 1;
	}

	consumidorData.handleThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsumidora, (LPVOID)&consumidorData, 0, NULL);
	if (consumidorData.handleThread == NULL) {
		printError(TEXT("hTreadConsumidora = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsumidora, (LPVOID)&consumidorData, 0, NULL);"));
		return 1;
	}

	WaitForSingleObject(consumidorData.handleThread, INFINITE);
	
	CloseComunicacao(&consumidorData);

	return 0;
}

DWORD WINAPI threadConsumidora(LPVOID param) {
	PTPCS data = (PTPCS)param;
	while (1) {
		WaitForSingleObject(data->handlesBuffer.semWrite, INFINITE);
		WaitForSingleObject(data->handlesBuffer.mHandleBuffer, INFINITE);

		_tprintf(TEXT("Recebi[%d] na pos[%d]\n"), data->bufferMemory->buffer[data->bufferMemory->nextOut], data->bufferMemory->nextOut);
		if (data->bufferMemory->nextOut == BUFFER_SIZE - 1) {
			data->bufferMemory->nextOut = 0;
		}
		else {
			data->bufferMemory->nextOut++;
		}

		ReleaseMutex(data->handlesBuffer.mHandleBuffer);
		ReleaseSemaphore(data->handlesBuffer.semRead, 1, NULL);
	}
	return 0;
}

void printError(short * msg) {
	_tprintf(TEXT("[Server]Error> '%s'\n"), msg);
}