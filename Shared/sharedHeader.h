#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>

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