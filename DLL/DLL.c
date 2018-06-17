#include "DLL.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

PTBDONMEMORY initComunicacaoServerSide(PTHFBUFFER param) {
	PTBDONMEMORY temp;

	param->mHandleBuffer = CreateMutex(NULL, FALSE, MUTEX_HANDLE_BUFFER);

	param->semWrite = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEM_WRITE_BUFFER);
	param->semRead = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEM_READ_BUFFER);

	param->memoryBufferHandle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(BDONMEMORY), BUFFER_SHAREDMEMORY_NAME);
	
	if (param->mHandleBuffer == NULL || param->semWrite == NULL || param->semRead == NULL || param->memoryBufferHandle == NULL) {
		return NULL;
	}
	
	temp = (PTBDONMEMORY)MapViewOfFile(param->memoryBufferHandle, FILE_MAP_WRITE, 0, 0, sizeof(BDONMEMORY));
	if (temp != NULL) {
		temp->nextIn = 0;
		temp->nextOut = 0; //BUFFER_SIZE - 1
	}

	return temp;
}

PTBDONMEMORY initComunicacaoGatewaySide(PTHFBUFFER param) {
	PTBDONMEMORY temp;

	param->mHandleBuffer = OpenMutex(MUTEX_MODIFY_STATE, FALSE, MUTEX_HANDLE_BUFFER);

	param->semWrite = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, SEM_WRITE_BUFFER);
	param->semRead = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, SEM_READ_BUFFER);

	param->memoryBufferHandle = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, BUFFER_SHAREDMEMORY_NAME);

	if (param->mHandleBuffer == NULL || param->semWrite == NULL || param->semRead == NULL || param->memoryBufferHandle == NULL) {
		return NULL;
	}

	temp = (PTBDONMEMORY)MapViewOfFile(param->memoryBufferHandle, FILE_MAP_WRITE, 0, 0, sizeof(BDONMEMORY));

	return temp;
}

void closeMemoriaPartilhadaJogo(PTGAMEDATAMS param) {
	UnmapViewOfFile(param->gameData);
	CloseHandle(param->handle);
	CloseHandle(param->mutex);
}

void CloseComunicacao(PTPCS data) {
	UnmapViewOfFile(data->bufferMemory);
	CloseHandle(data->handleThread);
	CloseHandle(data->handlesBuffer.mHandleBuffer);
	CloseHandle(data->handlesBuffer.memoryBufferHandle);
	CloseHandle(data->handlesBuffer.semRead);
	CloseHandle(data->handlesBuffer.semWrite);
}