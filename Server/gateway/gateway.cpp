#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <io.h>
#include <windows.h>
#include <stdlib.h> 
#include <fcntl.h>
#include "../Phoenix_DLL/dll.h"

#define _CRT_SECURE_NO_WARNINGS

HANDLE hThreadEscritor;

DWORD WINAPI ThreadProdutor(LPVOID param); //LADO DO GATEWAY
int _tmain(int argc, LPTSTR argv[]) {

	//	TCHAR resp;
	DWORD threadId;

	//	Jogo Jogo;


	Sincronizacao();

#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);

#endif

	hMemoriaBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, TAMANHOBUFFER, mPartilhadaMensagens); //onde está o invalid tbm posso guardar num dados.txt



	if (!hMemoriaBuffer) {
		_tprintf(TEXT("[Erro]Bufer"));
		return -1;
	}

	mensager = (PBuffer)MapViewOfFile(hMemoriaBuffer, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	
	if (mensager == NULL) {
		_tprintf(TEXT("[Erro]na memoria partilhada"));
		return -1;
	}

	

	hThreadEscritor = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProdutor, NULL, 0, &threadId);
	if (hThreadEscritor != NULL)
		_tprintf(TEXT("Lancei uma thread com id %d\n"), threadId);
	else {
		_tprintf(TEXT("Erro ao criar Thread Escritor\n"));
		return -1;
	}

	WaitForSingleObject(hThreadEscritor, INFINITE);

	UnmapViewOfFile(mensager);
	CloseHandle(SemaforoWriteSin);
	CloseHandle(SemaforoReadSin);
	CloseHandle(hMemoriaBuffer);
	CloseHandle(hMemoriaJogo);
	return 0;
}

DWORD WINAPI ThreadProdutor(LPVOID param) {

	while (1) { //Isto é para passar para a DLL
		EnviaMensagens();
	}
	return 0;
}