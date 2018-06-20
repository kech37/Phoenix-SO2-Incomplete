#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <stdlib.h> 
#include <fcntl.h>
#include <sys/types.h>

#define BUFFER_SIZE 32

#define PLAYER_NAME_SIZE 64

#define MAX_SHOOTS_SIZE 32
#define MAX_BOMBS_SIZE 32
#define MAX_POWERUPS_SIZE 16
#define MAX_ENEMYS_SPACESHIPS_SIZE 64
#define MAX_PLAYERS_SIZE 2

#define MUTEX_HANDLE_BUFFER TEXT("mutexHandleBuffer")
#define SEM_WRITE_BUFFER TEXT("semaphoreWrite")
#define SEM_READ_BUFFER TEXT("semaphoreRead")

#define MUTEX_ACCESS_GAME_MEMORY TEXT("mutexDeAcessoAmemoriaDeJogo")
#define ACCESS_GAME_MEMORY_NAME TEXT("nomeMemoriaPartilhadaDeJogo")

#define BUFFER_SHAREDMEMORY_NAME TEXT("memoriaPartilhadaNomeParaBuffer")


#define PIPE_NAME TEXT("\\\\.\\pipe\\comunicaçãoCLientesGateway")

#ifdef DLL_EXPORTS 
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif

/*########################################
###          Structs de jogo         ####
########################################*/

typedef struct _area {
	short cordX;
	short cordY;
	unsigned short altura;
	unsigned short comprimento;
} AREA, * PTAREA;

typedef enum _powerupType {
	GELO,
	VIDA,
	MAIS,
	EXTRA,
	ALCOOL,
	ESCUDO,
	BATERIA
} POWERUPTYPE;

typedef struct _powerup {
	AREA area;
	POWERUPTYPE tipo;
	unsigned short id;
	unsigned short duracao;
	unsigned short velocidade;
} POWERUP, * PTPOWERUP;

typedef enum _enemySpaceshipType {
	OUTRO,
	BASICO,
	ESQUIVO
} ENEMYTYPE;

typedef struct _enemySpaceship {
	AREA area;
	ENEMYTYPE tipo;
	unsigned short id;
	unsigned short vidas;
	unsigned short velocidade;
} ENEMYSPACESHIP, * PTENEMYSPACESHIP;

typedef enum _tecla {
	CIMA,
	NULA,
	BAIXO,
	ESPAÇO,
	DIREITA,
	ESQUERDA,
	OUTRA_TECLA
} TECLA, *PTTECLA;

typedef struct _playInfo {
	TECLA jogada;
	unsigned short id;
	TCHAR nome[PLAYER_NAME_SIZE];
} PLAYERINFO, PTPLAYERINFO;

typedef struct _defendorSpaceship {
	AREA area;
	unsigned short id;
	PTPOWERUP powerup;
	unsigned short vidas;
	PTPLAYERINFO jogador;
	unsigned short pontos;
	unsigned short velocidade;
}DEFENDORSPACESHIP, * PTDEFENDORSPACESHIP;

typedef struct _shoots {
	AREA area;
	unsigned short id;
	PTPLAYERINFO jogador;
	unsigned short velocidade;
} SHOOTS, * PTSHOOTS;

typedef struct _bombs {
	AREA area;
	unsigned short id;
	PTENEMYSPACESHIP dono;
	unsigned short velocidade;
} BOMBS, *PTBOMBS;

typedef enum _state {
	A_JOGAR,
	PREJOGO,
	POSJOGO,
	TERMINADO
} STATE;

typedef struct _gameData {
	STATE estado;
	unsigned short tirosCount;
	unsigned short bombasCount;
	unsigned short powerupsCount;
	unsigned short inimigosCount;
	BOMBS bombas[MAX_BOMBS_SIZE];
	SHOOTS tiros[MAX_SHOOTS_SIZE];
	unsigned short defensoresCount;
	POWERUP powerups[MAX_POWERUPS_SIZE];
	DEFENDORSPACESHIP defensores[MAX_PLAYERS_SIZE];
	ENEMYSPACESHIP inimigos[MAX_ENEMYS_SPACESHIPS_SIZE];
} GAMEDATA, *PTGAMEDATA;

/* ######################################## */

/*########################################
###    Structs Produtor Consumidor  ####
########################################*/

typedef struct _gameDataMemoryStruct {
	HANDLE mutex;
	HANDLE handle;
	PTGAMEDATA gameData;
}GAMEDATAMS, * PTGAMEDATAMS;

/* ######################################## */

/*########################################
 ###    Structs Produtor Consumidor  ####
########################################*/

typedef struct _handlesForBuffer {
	HANDLE	semRead;
	HANDLE	semWrite;
	HANDLE	mHandleBuffer;
	HANDLE	memoryBufferHandle;
} HFBUFFER, *PTHFBUFFER;

typedef struct _bufferDataOnMemory {
	short nextIn;
	short nextOut;
	PLAYERINFO buffer[BUFFER_SIZE];
} BDONMEMORY, *PTBDONMEMORY;

typedef struct _produtorConsumidorStruct {
	HANDLE handleThread;
	HFBUFFER handlesBuffer;
	PTBDONMEMORY bufferMemory;
} PCS, *PTPCS;

/* ######################################## */

#ifdef __cplusplus
extern "C" {
#endif
	DLL PTBDONMEMORY initComunicacaoServerSide(PTHFBUFFER param);
	DLL PTBDONMEMORY initComunicacaoGatewaySide(PTHFBUFFER param);
	DLL void CloseComunicacao(PTPCS data);
	DLL void closeMemoriaPartilhadaJogo(PTGAMEDATAMS param);
	DLL void produzItem(PTPCS data, PLAYERINFO jogada);
	DLL PLAYERINFO consumeItem(PTPCS data);
#ifdef __cplusplus
}
#endif