#include "main.h"

int random(int min, int max);
void goToXY(int x, int y);
/*carlos isto podia e devia ir para  a dll*/
DWORD WINAPI controlaNavesInimigas(LPVOID param);
DWORD WINAPI controlaTirosLancados(LPVOID param);
DWORD WINAPI controlaNaveDefensora(LPVOID param);

int _tmain(int argc, LPTSTR argv[]) {

	// esta merda aqui em baixo  nao esta ca a fazer nada tens e que usar o jogo que esta a ser partilhado
	// sen nao estas a jogar tu sozinho no servidor
		//PGAMEDATA gameView;
	/*
	Handle para mutex das threads
	*/
	HANDLE threadsMutex;

	/*
	Variaveis para thread dos inimigos basicos
	*/
	DWORD threadID_naves_inimigas;
	HANDLE handle_naves_inimigas;

	/*
	Variaveis para thread dos tiros
	*/
	DWORD threadID_controla_tiros;
	HANDLE handle_controla_tiros;

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif // UNICODE

	/*
	Memoria partilhada
	*/
	hMemoriaJogo = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(GAMEDATA), MAPAJOGO);
	if (!hMemoriaJogo) {
		_tprintf(TEXT("[Erro] GameData"));
		return -1;
	}

	/*
	init inimigos
	*/


	/*esta merda tem que ser no gateway como te disse men */
	gameView = (PGAMEDATA)MapViewOfFile(hMemoriaJogo, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (hMemoriaJogo == NULL) {
		_tprintf(TEXT("[Erro] na memoria partilhada\n"));
		return -1;
	}

	gameView->spaceshipInimigos[0].id = 0;
	gameView->spaceshipInimigos[0].tipoSpaceship = SPACESHIP_INIMIGO_TIPO_BASICO;
	gameView->spaceshipInimigos[0].pontosResistencia = 1;
	gameView->spaceshipInimigos[0].pontosVida = 1;
	gameView->spaceshipInimigos[0].tamanho.X = 3;
	gameView->spaceshipInimigos[0].tamanho.X = gameView->spaceshipInimigos[0].tamanho.Y;
	gameView->spaceshipInimigos[0].coordenadas.X = 5;
	gameView->spaceshipInimigos[0].coordenadas.Y = 0;

	gameView->spaceshipInimigos[1].id = 1;
	gameView->spaceshipInimigos[1].tipoSpaceship = SPACESHIP_INIMIGO_TIPO_DODGE;
	gameView->spaceshipInimigos[1].pontosResistencia = 3;
	gameView->spaceshipInimigos[1].pontosVida = 1;
	gameView->spaceshipInimigos[1].tamanho.X = 2;
	gameView->spaceshipInimigos[1].tamanho.X = gameView->spaceshipInimigos[1].tamanho.Y;
	gameView->spaceshipInimigos[1].coordenadas.X = 10;
	gameView->spaceshipInimigos[1].coordenadas.Y = 0;

	gameView->spaceshipInimigos[2].id = 2;
	gameView->spaceshipInimigos[2].tipoSpaceship = SPACESHIP_INIMIGO_TIPO_LENTO;
	gameView->spaceshipInimigos[2].pontosResistencia = 2;
	gameView->spaceshipInimigos[2].pontosVida = 2;
	gameView->spaceshipInimigos[2].tamanho.X = 5;
	gameView->spaceshipInimigos[2].tamanho.X = gameView->spaceshipInimigos[2].tamanho.Y;
	gameView->spaceshipInimigos[2].coordenadas.X = 15;
	gameView->spaceshipInimigos[2].coordenadas.Y = 0;

	/*
	Init tiros
	*/
	for (int i = 0; i < NUM_MAX_TIROS; i++) {
		gameView->tiros[i].id = i;
		gameView->tiros[i].tipo = SAB_TIPO_TIRO;
		gameView->tiros[i].coordenadas.X = SAB_INVALID;
		gameView->tiros[i].coordenadas.Y = SAB_INVALID;
		gameView->tiros[i].direcao = SAB_INVALID;
	}

	UnmapViewOfFile(gameView);

	/*
	Cria mutex
	*/
	threadsMutex = CreateMutex(NULL, FALSE, TEXT("mutexThreadsNavesInimigas"));
	if (threadsMutex == NULL) {
		_tprintf(TEXT("CreateMutex error: %d\n"), GetLastError());
		return 1;
	}

	/*
	Lançamento de thread para controlar naves inimigas
	*/
	handle_naves_inimigas = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaNavesInimigas, NULL, 0, &threadID_naves_inimigas);
	if (handle_naves_inimigas == NULL) {
		_tprintf(TEXT("Erro: Não possivel a criação Threads das naves básicas.\n"));
		return 1;
	}


	/*
	Lançamento de thread para controlar os tiros lançados
	*/
	handle_controla_tiros = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaTirosLancados, NULL, 0, &threadID_controla_tiros);
	if (handle_controla_tiros == NULL) {
		_tprintf(TEXT("Erro: Não possivel a criação Threads das naves básicas.\n"));
		return 1;
	}

	HANDLE mutex;
	while (1) {
		system("cls");
		gameView = (PGAMEDATA)MapViewOfFile(hMemoriaJogo, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		if (hMemoriaJogo == NULL) {
			_tprintf(TEXT("[Erro] na memoria partilhada 'imprime nave inimiga'\n"));
			return -1;
		}
		for (int i = 0; i < NUM_INIMIGOS; i++) {
			mutex = CreateMutex(NULL, FALSE, TEXT("mutexThreadsNavesInimigas"));
			WaitForSingleObject(mutex, INFINITE);
			goToXY(gameView->spaceshipInimigos[i].coordenadas.X, gameView->spaceshipInimigos[i].coordenadas.Y);
			_tprintf(TEXT("%d"), gameView->spaceshipInimigos[i].id);
			ReleaseMutex(mutex);
		}
		CloseHandle(mutex);
		UnmapViewOfFile(gameView);
		Sleep(100);
	}

	_gettch();

	CloseHandle(handle_naves_inimigas);
	CloseHandle(handle_controla_tiros);
	CloseHandle(threadsMutex);
	CloseHandle(hMemoriaJogo);
	return 0;
}

//Servirá como consumidor pois tudo o resto é trabalhado por 'IA'
DWORD WINAPI controlaNaveDefensora(LPVOID param) {
	WaitForSingleObject(MutexWrite, INFINITE);
	WaitForSingleObject(SemaforoReadSin, INFINITE);

	int n = mensager->buffer[mensager->output].id;
	_tprintf(_TEXT("::->%d \n"), n);
	mensager->buffer[mensager->output].id = 0;
	mensager->output = (mensager->output + 1) % MAXBufer;
	mensager->numeroMensagens--;

	ReleaseMutex(MutexWrite);
	ReleaseSemaphore(SemaforoWriteSin, 1, NULL);
	return 0;
}

DWORD WINAPI controlaNavesInimigas(LPVOID param) {
	PGAMEDATA gameView;
	gameView = (PGAMEDATA)MapViewOfFile(hMemoriaJogo, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (hMemoriaJogo == NULL) {
		_tprintf(TEXT("[Erro] na memoria partilhada 'controlaNavesInimigas'\n"));
		return -1;
	}
	while (1) {
		for (int i = 0; i < NUM_INIMIGOS; i++) {
			switch (gameView->spaceshipInimigos[i].tipoSpaceship) {
			case SPACESHIP_INIMIGO_TIPO_BASICO:
				if (random(1, 100) < 65) {
					gameView->spaceshipInimigos[i].coordenadas.Y++;
				}
				else {
					if (random(1, 100) < 45) {
						gameView->spaceshipInimigos[i].coordenadas.X++;
					}
					else {
						if (random(1, 100) < 45) {
							gameView->spaceshipInimigos[i].coordenadas.X--;
						}
					}
				}
				Sleep(SPACESHIP_BASE_SPEED);
				break;
			case SPACESHIP_INIMIGO_TIPO_DODGE:
				if (random(1, 100) < 35) {
					gameView->spaceshipInimigos[i].coordenadas.Y++;
				}
				else {
					if (random(1, 100) < 50) {
						gameView->spaceshipInimigos[i].coordenadas.X++;
					}
					if (random(1, 100) < 45) {
						gameView->spaceshipInimigos[i].coordenadas.X--;
					}
				}
				Sleep((int)(SPACESHIP_BASE_SPEED * 1.2));
				break;
			case SPACESHIP_INIMIGO_TIPO_LENTO:
				if (random(1, 100) < 65) {
					gameView->spaceshipInimigos[i].coordenadas.Y++;
				}
				else {
					if (random(1, 100) < 45) {
						gameView->spaceshipInimigos[i].coordenadas.X++;
					}
					else {
						if (random(1, 100) < 45) {
							gameView->spaceshipInimigos[i].coordenadas.X--;
						}
					}
				}
				Sleep((int)(SPACESHIP_BASE_SPEED * 0.8));
				break;
			}
		}
	}
	UnmapViewOfFile(gameView);
	return 0;
}

DWORD WINAPI controlaTirosLancados(LPVOID param) {
	/*SAB * tirosLancados = ((SAB *)param);
	while (1) {
	for (int i = 0; i < NUM_MAX_TIROS; i++) {
	if (tirosLancados[i].direcao != SAB_INVALID) {
	//TODO PRECISA DE MUTEX?
	if (tirosLancados[i].direcao == SAB_DIRECAO_CIMA) {
	tirosLancados[i].coordenadas.Y--;
	}
	else if (tirosLancados[i].direcao == SAB_DIRECAO_BAIXO) {
	tirosLancados[i].coordenadas.Y++;
	}
	//TODO checkar se bateu em algo
	}
	}
	}*/
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

int random(int min, int max) {
	return(min + rand() % (max - min));
}
