#include "../../Shared/sharedHeader.h"



#define NUM_THREADS 3


int random(int min, int max) {
	return(min + rand() % (max - min));
}

HANDLE hMutex;

DWORD WINAPI controlEnemySpaceship(LPVOID param);
void goToXY(int x, int y);
void cls(HANDLE hConsole);

int _tmain(int argc, LPTSTR argv[]) {
	DWORD * threadDefenderSpaceshipID;
	HANDLE * hThreadDefenderSpaceship;
	SPACESHIP * enemySpaceship = NULL;
	HANDLE hStdout;

	srand((unsigned int)time(NULL));
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif // UNICODE

	threadDefenderSpaceshipID	= (DWORD *)		malloc(sizeof(DWORD) * NUM_THREADS);
	hThreadDefenderSpaceship	= (HANDLE *)	malloc(sizeof(HANDLE) * NUM_THREADS);
	enemySpaceship = (SPACESHIP *)	malloc(sizeof(SPACESHIP) * NUM_THREADS);

	hMutex = CreateMutex(NULL, FALSE, NULL);

	for (int i = 0; i < NUM_THREADS; i++) {
		enemySpaceship[i].id = i;
		enemySpaceship[i].spaceshipType = SPACESHIP_ENEMY_TYPE_BASIC;
		enemySpaceship[i].size.X = 3;
		enemySpaceship[i].size.Y = enemySpaceship[i].size.X;
		enemySpaceship[i].lifePoints = 1;
		enemySpaceship[i].resistancePoints = 1;
		enemySpaceship[i].coord.X = random(0, 50);
		enemySpaceship[i].coord.Y = random(0, 3);

		hThreadDefenderSpaceship[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlEnemySpaceship, (LPVOID)&enemySpaceship[i], 0, &threadDefenderSpaceshipID);

		if (hThreadDefenderSpaceship[i] == NULL) {
			_tprintf(TEXT("Erro ao criar Thread.\n"));
		}
	}
	
	WaitForMultipleObjects(NUM_THREADS, hThreadDefenderSpaceship, TRUE, INFINITE);

	CloseHandle(hMutex);

	free(enemySpaceship);
	free(hThreadDefenderSpaceship);
	free(threadDefenderSpaceshipID);

	_tprintf(TEXT("Vou terminar...\n"));

	return 0;
}

DWORD WINAPI controlEnemySpaceship(LPVOID param) {
	HANDLE hSem;
	SPACESHIP *tempSpaceship = (SPACESHIP *)param;

	for (int i = 0; i < 5; i++) {
		hSem = CreateSemaphore(NULL, 1, 1, TEXT("teste"));
		WaitForSingleObject(hSem, INFINITE);
		goToXY(tempSpaceship->coord.X, tempSpaceship->coord.Y);
		Sleep(1000);
		_tprintf(TEXT("%d"), tempSpaceship->id);
		tempSpaceship->coord.Y++;
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
	if (!hStdout) {
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	SetConsoleCursorPosition(hStdout, coord);

}

void cls(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };    // home for the cursor 
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer. 

	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks.

	if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer 
		(TCHAR) ' ',     // Character to write to the buffer
		dwConSize,       // Number of cells to write 
		coordScreen,     // Coordinates of first cell 
		&cCharsWritten))// Receive number of characters written
	{
		return;
	}

	// Get the current text attribute.

	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	// Set the buffer's attributes accordingly.

	if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer 
		csbi.wAttributes, // Character attributes to use
		dwConSize,        // Number of cells to set attribute 
		coordScreen,      // Coordinates of first cell 
		&cCharsWritten)) // Receive number of characters written
	{
		return;
	}

	// Put the cursor at its home coordinates.

	SetConsoleCursorPosition(hConsole, coordScreen);
}






