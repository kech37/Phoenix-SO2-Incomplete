#include "..\DLL\DLL.h"

int _tmain(int argc, LPTSTR agrv[]) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif // UNICODE
	HFBUFFER handlesBuffer;
	PTBDONMEMORY bufferMemory;

	bufferMemory = initComunicacaoServerSide(&handlesBuffer);

	while (1) {
		WaitForSingleObject(handlesBuffer.semWrite, INFINITE);
		WaitForSingleObject(handlesBuffer.mHandleBuffer, INFINITE);

		_tprintf(TEXT("Recebi[%d] na pos[%d]\n"), bufferMemory->buffer[bufferMemory->nextOut], bufferMemory->nextOut);
		if (bufferMemory->nextOut == BUFFER_SIZE - 1) {
			bufferMemory->nextOut = 0;
		}
		else {
			bufferMemory->nextOut++;
		}

		ReleaseMutex(handlesBuffer.mHandleBuffer);
		ReleaseSemaphore(handlesBuffer.semRead, 1, NULL);

	}
	return 0;
}