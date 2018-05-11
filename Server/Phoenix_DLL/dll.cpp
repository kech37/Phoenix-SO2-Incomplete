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

/*estas funções foram tiradas dos exemplos dos professores em pec */
//memoria partilhada para troca de mensagens
void EnviaMensagens() {
	WaitForSingleObject(MutexRead, INFINITE);
	WaitForSingleObject(SemaforoWriteSin, INFINITE);
	mensager->buffer[mensager->input].id = 1;
	mensager->input = (mensager->input + 1) % MAXBufer;
	mensager->numeroMensagens++;
	ReleaseMutex(MutexRead);
	ReleaseSemaphore(SemaforoReadSin, 1, NULL);
}

/*para depois no servidor iniciar a sincronização*/
//memoria partilhada para troca de mensagens
void Sincronizacao() {
	MutexWrite = CreateMutex(NULL, FALSE, NULL);
	MutexRead = CreateMutex(NULL, FALSE, NULL);
	SemaforoReadSin = CreateSemaphore(NULL, 0, MAXBufer, SemaforoWrite);
	SemaforoWriteSin = CreateSemaphore(NULL, MAXBufer, MAXBufer, SemaforoRead);
	if (!SemaforoWriteSin || !SemaforoReadSin || !MutexRead || !MutexWrite) {
		_tprintf(TEXT("Deu asneira na sincro!!!!!"));
		return;
	}

}

/*para conseguir depois enviar mensagem */
//memoria partilhada troca de mensagens
void TrataMensagens() {
	WaitForSingleObject(MutexWrite, INFINITE);
	WaitForSingleObject(SemaforoReadSin, INFINITE);
	int n = mensager->buffer[mensager->output].id;
	_tprintf(_TEXT("::->%d \n"), n);
	mensager->buffer[mensager->output].id = 0;
	mensager->output = (mensager->output + 1) % MAXBufer;
	mensager->numeroMensagens--;
	ReleaseMutex(MutexWrite);
	ReleaseSemaphore(SemaforoWriteSin, 1, NULL);
}

//ficar no infinito e mais a alem a trocar mensagens xD
DWORD WINAPI ThreadProdutor(LPVOID param) {
	while (1) { EnviaMensagens();}
	return -1;
}

//para mandar mensagem para o servidor 
DWORD WINAPI ThreadConsumidor(LPVOID param) { 
	while (1) { TrataMensagens();} // ver esta merda porque depois temos que terminar as mensagens quando o jogo terminar 
	return -1;
}