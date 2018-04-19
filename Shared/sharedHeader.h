#include <tchar.h>
#include <Windows.h>

#define SPACESHIP_DEFENDER_TYPE 0;
#define SPACESHIP_ENEMY_TYPE_BASIC 1;
#define SPACESHIP_ENEMY_TYPE_DODGE 2;
#define SPACESHIP_ENEMY_TYPE_SLOW 3;

typedef struct Spaceship {
	int id;
	int spaceshipType;
	int lifePoints;
	int resistancePoints;
	int cordX, cordY;
}Spaceship;

#define POWERUP_TYPE_SHILD 0;
#define POWERUP_TYPE_ICE 1;
#define POWERUP_TYPE_BATTERY 2;
#define POWERUP_TYPE_PLUS 3;
#define POWERUP_TYPE_LIFE 4;
#define POWERUP_TYPE_ALCOOL 5;
#define POWERUP_TYPE_EXTRA 6;

typedef struct Powerup {
	int id;
	int powerupType;
	int spawnOccurrence;
	int durantion;
	int cordX, cordY;
}Powerup;