#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <stdlib.h> 
#include <fcntl.h>

#define BUFFER_SIZE 32

#ifdef DLL_EXPORTS 
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif

#define MUTEX_HANDLE_BUFFER TEXT("mutexHandleBuffer")
#define SEM_WRITE_BUFFER TEXT("semaphoreWrite")
#define SEM_READ_BUFFER TEXT("semaphoreRead")

typedef struct _handlesForBuffer {
	HANDLE	mHandleBuffer;
	HANDLE	semWrite;
	HANDLE	semRead;
}HFBUFFER, *PTHFBUFFER;

typedef struct _bufferDataOnMemory {
	int buffer[BUFFER_SIZE];
	int nextIn;
	int nextOut;
}BDONMEMORY, *PTBDONMEMORY;

#ifdef __cplusplus
extern "C" {
#endif
	DLL HFBUFFER initComunicacaoServerSide(PTBDONMEMORY param);
#ifdef __cplusplus
}
#endif