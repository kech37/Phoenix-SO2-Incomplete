//usaDLL.cpp
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include "dll.h"

//Para verificar ao carregar a dll que a aplicação irá ocupar mais memória
char ponteiro[40960];
//Definição da variável global
int nDLL = 1234;
PBuffer mensager = NULL;


void EnviaMensagem() {
	WaitForSingleObject(MutexRead, INFINITE);
	WaitForSingleObject(SemaforoWriteSin, INFINITE);
	mensager->buffer[mensager->in].id = 1;
	mensager->in = (mensager->in + 1) % MAX;
	mensager->numeroMensagens++;
	ReleaseMutex(MutexRead);
	ReleaseSemaphore(SemaforoReadSin, 1, NULL);
}

void Sincroni() {
	MutexWrite = CreateMutex(NULL, FALSE, NULL);
	MutexRead = CreateMutex(NULL, FALSE, NULL);
	SemaforoReadSin = CreateSemaphore(NULL, 0, MAX, SemaforoWrite);
	SemaforoWriteSin = CreateSemaphore(NULL, MAX, MAX, SemaforoRead);
	
	if (!SemaforoWriteSin|| !SemaforoReadSin || !MutexRead  || !MutexWrite) {
		_tprintf(TEXT("Deu asneira na sincro!!!!!"));
		return;
	}

}


void TrataMensagem() {
	
	WaitForSingleObject(MutexWrite, INFINITE);  
	WaitForSingleObject(SemaforoReadSin, INFINITE);
	int n = mensager->buffer[mensager->out].id;
	_tprintf(_TEXT("::->%d \n"), n);
	mensager->buffer[mensager->out].id = 0;
	mensager->out = (mensager->out + 1) % MAX;
	mensager->numeroMensagens--;
	ReleaseMutex(MutexWrite);
	ReleaseSemaphore(SemaforoWriteSin, 1, NULL);
}