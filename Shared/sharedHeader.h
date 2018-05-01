#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>

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
}GAMEDATA;