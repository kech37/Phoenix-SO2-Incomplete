#include <conio.h>
#include "../Phoenix_DLL/dll.h"


#define NUM_THREADS 3

#define TAMECRA 50

int random(int min, int max) {
	return(min + rand() % (max - min));
}

HANDLE hMutex;

DWORD WINAPI checkDefenderSpaceshop(LPVOID param);
void goToXY(int x, int y);

int _tmain(int argc, LPTSTR argv[]) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif // UNICODE


	DWORD * threadDefenderSpaceshipID;
	HANDLE * hThreadDefenderSpaceship;
	SPACESHIP * defenderSpaceship = NULL;
	
	

	srand((unsigned int) time(NULL));

	threadDefenderSpaceshipID = (DWORD *)malloc(sizeof(DWORD) * NUM_THREADS);
	hThreadDefenderSpaceship = (HANDLE *)malloc(sizeof(HANDLE) * NUM_THREADS);
	defenderSpaceship = (SPACESHIP *)malloc(sizeof(SPACESHIP) * NUM_THREADS);

	hMutex = CreateMutex(NULL, FALSE, NULL);

	for (int i = 0; i < NUM_THREADS; i++) {
		defenderSpaceship[i].id = i;
		defenderSpaceship[i].spaceshipType = SPACESHIP_DEFENDER_TYPE;
		defenderSpaceship[i].sizeX = 3;
		defenderSpaceship[i].sizeY = defenderSpaceship[i].sizeX;
		defenderSpaceship[i].lifePoints = 1;
		defenderSpaceship[i].resistancePoints = 1;
		defenderSpaceship[i].cordX = random(0, 20);
		defenderSpaceship[i].cordY = random(0, 20);
		
		hThreadDefenderSpaceship[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)checkDefenderSpaceshop, (LPVOID)&defenderSpaceship[i], 0, &threadDefenderSpaceshipID);
		if (hThreadDefenderSpaceship[i] != NULL) {
			//_tprintf(TEXT("Lançei uma thread com id %d.\n"), (int) threadDefenderSpaceshipID);
		}else {
			_tprintf(TEXT("Erro ao criar Thread.\n"));
		}
	}

	WaitForMultipleObjects(NUM_THREADS, hThreadDefenderSpaceship, TRUE, INFINITE);

	CloseHandle(hMutex);

	free(defenderSpaceship);
	free(hThreadDefenderSpaceship);
	free(threadDefenderSpaceshipID);
	//_gettch();
	_tprintf(TEXT("Vou terminar...\n"));

	return 0;
}

DWORD WINAPI checkDefenderSpaceshop(LPVOID param) {
	HANDLE hSem;
	SPACESHIP *tempSpaceship = (SPACESHIP *)param;

	//_tprintf(TEXT("[Thread %d] >> Vou Começar a trabalhar!\n"), GetCurrentThreadId());

	for (int i = 0; i < 30; i++) {
		
		hSem = CreateSemaphore(NULL, 1, 1, TEXT("teste"));
		WaitForSingleObject(hSem, INFINITE);
		goToXY(tempSpaceship->cordX, tempSpaceship->cordY);
		Sleep(1000);
		tempSpaceship->cordX++;
		_tprintf(TEXT("%d"), tempSpaceship->id);
		ReleaseSemaphore(hSem, 1, NULL);
		CloseHandle(hSem);
	
	}

	return 0;
}

void goToXY(int x, int y) {
	static HANDLE hStdout = NULL;
	COORD coord;
	coord.X = x;
	coord.Y = y;
	//verificaPosicao(coord);					//testar depois porque nem e aqui que se vai chamar esta funçao
	if (!hStdout) {
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	SetConsoleCursorPosition(hStdout, coord);
	
}

BOOL verificaPosicao(COORD cordenada)
{
	COORD coord;
	if ((cordenada.Y < 0) || (cordenada.Y >= TAMECRA) || (cordenada.X < 0) || (cordenada.X >= TAMECRA))	//verifica limites do mapa
		return FALSE;
	//if ()	//verificar se a posição para onde vai o macaco esta livre mas neste momento não tenho nada onde me diga a posição das outras naves 
		return TRUE;
	return FALSE;
}
	

