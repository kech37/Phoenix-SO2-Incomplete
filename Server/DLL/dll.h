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
#define MAXBufer 80 
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

#ifdef	DLL_EXPORTS
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif




#define PARTILHAMENSAGENS TEXT("PARTILHAMENSAGENS")
#define MAPAJOGO TEXT("MAPAJOGO")
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

/*ciclos de vida do nosso jogo mas por enquanto nao vai ser preciso*/
typedef enum MensagemCliente {
	Start,
	EmJogo
} MensagemCliente;

/*ciclos de vida do nosso jogo mas por enquanto nao vai ser preciso*/
typedef enum MensagemServidor {
	AIniciar,
	AJogar
} MensagemServidor;

//Mensagem que vai do Gateway para o cliente
typedef struct MsgServidor {
	MensagemServidor mensagem;
} MsgServidor;


//Mensagem que vai do Cliente para o Gateway
typedef struct MsgCliente {
	MensagemCliente mensagem;
	int id;
	TCHAR nome[SIZE];		//para depois comparar
	TCHAR tecla[SIZE];		//para depois comparar com o nome da estrutura que criamos me cima o ze
} MsgCliente;


typedef struct Buffer {
	int input;
	int output;
	int numeroMensagens;
	MsgCliente buffer[MAXBufer];

} Buffer, *PBuffer;

PBuffer mensager=NULL;




/*------------------------------------------------todas as estruturas para o jogo vao estar aqui -------------------------------*/
#define SPACESHIP_BASE_SPEED 800

#define NUM_MAX_DEFENSORES	3
#define NUM_MAX_POWERUPS	20
#define NUM_MAX_TIROS		50
#define NUM_MAX_BOMBAS		50
#define NUM_INIMIGOS		3

typedef struct _COORDENADAS {
	int X;
	int Y;
}COORDENADAS;

typedef struct _TAMANHO {
	int X;
	int Y;
}TAMANHO;

#define	SPACESHIP_TIPO_DEFENSOR			0
#define SPACESHIP_INIMIGO_TIPO_BASICO	1
#define SPACESHIP_INIMIGO_TIPO_DODGE	2
#define SPACESHIP_INIMIGO_TIPO_LENTO	3

typedef struct _SPACESHIP {
	int			id;
	int			tipoSpaceship;
	int			pontosVida;
	int			pontosResistencia;
	COORDENADAS coordenadas;
	TAMANHO		tamanho;
}SPACESHIP;

#define POWERUP_TIPO_ESCUDO		0
#define POWERUP_TIPO_GELO		1
#define POWERUP_TIPO_BATERIA	2
#define POWERUP_TIPO_MAIS		3
#define POWERUP_TIPO_VIDA		4
#define POWERUP_TIPO_ALCOOL		5
#define POWERUP_TIPO_EXTRA		6

typedef struct _POWERUP {
	int			id;
	int			tipoPowerup;
	int			spawnOccurrence;
	int			duracao;
	COORDENADAS coordenadas;
	TAMANHO		size;
}POWERUP;

#define SAB_INVALID		   -1
#define SAB_TIPO_BOMBA		0
#define SAB_TIPO_TIRO		1
#define SAB_DIRECAO_CIMA	2
#define SAB_DIRECAO_BAIXO	3

typedef struct _SAB {
	int		id;
	int		tipo;
	int		direcao;
	COORDENADAS coordenadas;
}SAB;

typedef struct _GAMEDATA {
	SPACESHIP	spaceshipInimigos[NUM_INIMIGOS];
	SPACESHIP	spaceshipDefensores[NUM_MAX_DEFENSORES];
	POWERUP		powerups[NUM_MAX_POWERUPS];
	SAB			tiros[NUM_MAX_TIROS];
	SAB			bombas[NUM_MAX_BOMBAS];
}GAMEDATA, *PGAMEDATA;

PGAMEDATA gameView;


#ifdef __cplusplus 
extern "C" {
	extern DLL int nDLL;
	extern  DLL PBuffer mensagemBuffer;
	extern DLL PGAMEDATA gameView;
	DLL void EnviaMensagens();
	DLL void Sincronizacao();
	DLL void TrataMensagens();
	DLL DWORD WINAPI ThreadProdutor(LPVOID param);
	DLL DWORD WINAPI ThreadConsumidor(LPVOID param);

}
#endif
