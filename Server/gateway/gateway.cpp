#include "gateway.h"


int _tmain(int argc, LPTSTR argv[]) {
	DWORD threadId;
	HANDLE hThreadEscritor;
	// vamos sicronizar tudo para o troca de mensagens do jogo
		
#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	//tem que se lançar uma thread para que através 
	//de named pipes mandei para os clientes 
	//o estado do jogo
	hMemoriaJogo= CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(GAMEDATA),TEXT("sei la se e o chines se vou partilhar se e o ..."));
	hMemoriaBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUFFERTAM, PARTILHAMENSAGENS);
	if (!hMemoriaBuffer || !hMemoriaJogo) {
		_tprintf(TEXT("[Erro]Bufer ou no jogo agora escolhe"));
		return -1;
	}

	mensager = (PBuffer)MapViewOfFile(hMemoriaBuffer, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (mensager == NULL) {
		_tprintf(TEXT("[Erro]na memoria partilhada"));
		return -1;
	}
	
	gameView = (PGAMEDATA) MapViewOfFile(hMemoriaJogo, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (gameView == NULL)
		return-1;

	// e so ver o exemplo que o prof deu sobre o escritor que vais perceber isto meu 
	hThreadEscritor = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProdutor, NULL, 0, &threadId);
	if (hThreadEscritor != NULL)
		_tprintf(TEXT("Lancei thread com id %d\n"), threadId);
	else {
		_tprintf(TEXT("Erro ao criar Thread Escritor\n"));
		return -1;
	}

	/*actualizar threds
	mas vais ter que ficar depois de certeza*/
	WaitForSingleObject(hThreadEscritor, INFINITE);
	
	UnmapViewOfFile(mensager);
	CloseHandle(SemaforoWriteSin);
	CloseHandle(SemaforoReadSin);
	CloseHandle(hMemoriaBuffer);
	CloseHandle(hMemoriaJogo);
	return 0;
}