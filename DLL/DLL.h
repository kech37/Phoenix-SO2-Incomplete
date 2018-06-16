#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <stdlib.h> 
#include <fcntl.h>

#define BUFFER_SIZE 32

#define MUTEX_HANDLE_BUFFER TEXT("mutexHandleBuffer")
#define SEM_WRITE_BUFFER TEXT("semaphoreWrite")
#define SEM_READ_BUFFER TEXT("semaphoreRead")

#define BUFFER_SHAREDMEMORY_NAME TEXT("memoriaPartilhadaNomeParaBuffer")

#ifdef DLL_EXPORTS 
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif

typedef struct _handlesForBuffer {
	HANDLE	mHandleBuffer;
	HANDLE	semWrite;
	HANDLE	semRead;
	HANDLE	memoryBufferHandle;
}HFBUFFER, *PTHFBUFFER;

typedef struct _bufferDataOnMemory {
	int buffer[BUFFER_SIZE];
	int nextIn;
	int nextOut;
}BDONMEMORY, *PTBDONMEMORY;

typedef struct _produtorConsumidorStruct {
	HFBUFFER handlesBuffer;
	PTBDONMEMORY bufferMemory;
	HANDLE handleThread;
} PCS, *PTPCS;

#ifdef __cplusplus
extern "C" {
#endif
	DLL PTBDONMEMORY initComunicacaoServerSide(PTHFBUFFER param);
	DLL PTBDONMEMORY initComunicacaoGatewaySide(PTHFBUFFER param);
	DLL void CloseComunicacao(PTPCS data);
#ifdef __cplusplus
}
#endif