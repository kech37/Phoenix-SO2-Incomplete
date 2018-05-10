#pragma once
#include <windows.h>
#include <tchar.h>

#define TAM 256
//tamanho do buffer 
#define MAX 80 
//dimensão da mensagem
#define SIZE 100  

HANDLE SemaforoWriteSin;
HANDLE SemaforoReadSin;
HANDLE MutexRead;
HANDLE MutexWrite;
HANDLE hMemoriaBuffer;
HANDLE hMemoriaJogo;

TCHAR SemaforoRead[] = TEXT("Ler");
TCHAR SemaforoWrite[] = TEXT("Escrever");


#ifdef Phoenix_DLL_EXPORTS
#define Phoenix_DLL __declspec(dllexport)
#else
#define Phoenix_DLL __declspec(dllimport)
#endif

#ifdef __cpluplus
extern "C" {
#endif

	//Variável global da DLL
	extern Phoenix_DLL int nDLL;
	extern  Phoenix_DLL PBuffer mensager;

	Phoenix_DLL void Sincronizacao();
	Phoenix_DLL void TrataMensagem();
	Phoenix_DLL void EnviaMensagem();

	//exemplo da aula 
//	Phoenix_DLL int UmaString(void);
	
#ifdef __cplusplus
}
#endif


/*-------------------------Todas as estruturas que vamos partilhar tem que ser aqui para depois mapear percebes????*/
/*
Mecanismo de sicronizão para o raio do bufer

Gataway
semaphore
mutex

Servidor 

semaphore
mutex

*/


typedef enum Teclas {
	CIMA,
	BAIXO,
	ESQUERDA,
	DIREITA,
	ESPACO         /*disparar*/
}Teclas;

/*
Mensagens do servidor
Ao incio para começar jogo id e nome do cliente
notificar jogadores de actualização
enviar no fim pontuação e já é bem bom
*/
typedef enum MensagemCliente {
	Start,
	EmJogo
} MensagemCliente;

typedef enum MensagemServidor {
	AIniciar,
	AJogar
} MensagemServidor;

//Mensagem que vai do Gateway para o cliente
typedef struct MsgServidor {
	MensagemServidor mensagem;

} MsgServidor;

typedef struct Pontuacao {
	int pontos;
	TCHAR jogador[SIZE];
} Pontuacao;

//Mensagem que vai do Cliente para o Gateway
typedef struct MsgCliente {
	MensagemCliente mensagem;
	int id;
	TCHAR nome[SIZE];
	TCHAR tecla[SIZE];
} MsgCliente;


typedef struct MsgPontuacao {
	Pontuacao pontuacoes[10];
} MsgPontuacao;

typedef struct _Buffer{
	int numeroMensagens;
	MsgCliente buffer[MAX];
	int in;
	int out;

} Buffer, *PBuffer;
