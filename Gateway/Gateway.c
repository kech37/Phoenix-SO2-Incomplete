#include "..\DLL\DLL.h"

HANDLE hBuffer, mHandleBuffer, semRead, semWrite;
PTBDONMEMORY bufferMemory;

int o = 1;

void initComunicacao() {
	mHandleBuffer = CreateMutex(NULL, FALSE, MUTEX_HANDLE_BUFFER);

	semWrite = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEM_WRITE_BUFFER);
	semRead = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEM_READ_BUFFER);

	hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(BDONMEMORY), TEXT("buffer"));
	bufferMemory = (PTBDONMEMORY)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(BDONMEMORY));

	bufferMemory->nextIn = 0;
	bufferMemory->nextOut = BUFFER_SIZE - 1;
}


int _tmain(int argc, LPTSTR agrv[]) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif // UNICODE
	initComunicacao();
	while (1) {
		WaitForSingleObject(semRead, INFINITE);
		WaitForSingleObject(mHandleBuffer, INFINITE);

		_tprintf(TEXT("nextInt[%d]\n"), bufferMemory->nextIn);

		bufferMemory->buffer[bufferMemory->nextIn] = ++o;
		if (bufferMemory->nextIn == BUFFER_SIZE - 1) {
			bufferMemory->nextIn = 0;
		}
		else {
			bufferMemory->nextIn++;
		}

		ReleaseMutex(mHandleBuffer);
		ReleaseSemaphore(semWrite, 1, NULL);

		Sleep(500);

	}
	return 0;
}