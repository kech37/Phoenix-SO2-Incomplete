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
	HANDLE hBuffer;

	param->mHandleBuffer = CreateMutex(NULL, FALSE, MUTEX_HANDLE_BUFFER);

	param->semWrite = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEM_WRITE_BUFFER);
	param->semRead = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEM_READ_BUFFER);

	hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(BDONMEMORY), TEXT("buffer"));
	temp = (PTBDONMEMORY)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(BDONMEMORY));

	temp->nextIn = 0;
	temp->nextOut = BUFFER_SIZE - 1;

	CloseHandle(hBuffer);

	return temp;
}