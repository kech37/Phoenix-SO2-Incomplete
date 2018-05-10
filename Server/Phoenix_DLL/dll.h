#pragma once
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>

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

#define PartilhaMensagem TEXT("Partilha cenas e isso tudo")
#define BUFFERTAM sizeof(Buffer)

/*-------------------------Todas as estruturas que vamos partilhar tem que ser aqui para depois mapear percebes????*/


/*-------------------------todas as estruturas de comunicação vao estar neste sitio ----------------------------------*/
typedef enum Teclas {
	CIMA,
	BAIXO,
	ESQUERDA,
	DIREITA,
	ESPACO         /*para um gajo poder disparar*/
}Teclas;


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

typedef struct _Buffer {
	int numeroMensagens;
	MsgCliente buffer[MAX];
	int in;
	int out;

} Buffer, *PBuffer;

PBuffer mensager;


extern "C" {

	//Variável global da DLL
	extern Phoenix_DLL int nDLL;
	extern  Phoenix_DLL PBuffer mensagemBuffer;
	Phoenix_DLL void EnviaMensagens();
	Phoenix_DLL void Sincronizacao();
	Phoenix_DLL void TrataMensagens();


	//exemplo da aula 
	//	Phoenix_DLL int UmaString(void);

}

/*------------------------------------------------todas as estruturas para o jogo vao estar aqui -------------------------------*/

#define	SPACESHIP_DEFENDER_TYPE		0x00;
#define SPACESHIP_ENEMY_TYPE_BASIC	0x01;
#define SPACESHIP_ENEMY_TYPE_DODGE	0x02;
#define SPACESHIP_ENEMY_TYPE_SLOW	0x03;

typedef struct SPACESHIP {
	int id;
	int spaceshipType;
	int lifePoints;
	int resistancePoints;
	int cordX, cordY;
	int sizeX, sizeY;
}SPACESHIP;

#define POWERUP_TYPE_SHILD		0x00;
#define POWERUP_TYPE_ICE		0x01;
#define POWERUP_TYPE_BATTERY	0x02;
#define POWERUP_TYPE_PLUS		0x03;
#define POWERUP_TYPE_LIFE		0x04;
#define POWERUP_TYPE_ALCOOL		0x05;
#define POWERUP_TYPE_EXTRA		0x06;

typedef struct POWERUP {
	int id;
	int powerupType;
	int spawnOccurrence;
	int durantion;
	int cordX, cordY;
	int sizeX, sizeY;
}POWERUP;


