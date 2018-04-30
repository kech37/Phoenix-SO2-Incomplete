#pragma once

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>  
#include <stdlib.h> 


//tamanho do buffer 
#define MAX 80 
//dimensão da mensagem
#define SIZE 100  

#ifdef dll_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif

extern "C"
{
	//Variável global da DLL
	extern DLL_IMP_API int nDLL;



}




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
