#include "main.h"
int random(int min, int max);
void goToXY(int x, int y);
DWORD WINAPI controlaNavesInimigasBasicas(LPVOID param);
DWORD WINAPI controlaNavesInimigasDODGE(LPVOID param);
DWORD WINAPI controlaNavesInimigasLento(LPVOID param);
DWORD WINAPI controlaTirosLancados(LPVOID param);

GAMEDATA gameData;

int _tmain(int argc, LPTSTR argv[]) {
	/*
	Handle para mutex das threads
	*/
	HANDLE threadsMutex;

	/*
	Variaveis para thread dos inimigos basicos
	*/
	DWORD threadID_naves_inimigas_basicas;
	HANDLE handle_naves_inimigas_basicas;

	/*
	Variaveis para thread dos inimigos dodge
	*/
	DWORD threadID_naves_inimigas_dodge;
	HANDLE handle_naves_inimigas_dodge;

	/*
	Variaveis para thread dos inimigos lento
	*/
	DWORD threadID_naves_inimigas_lento;
	HANDLE handle_naves_inimigas_lento;

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
	init inimigos
	*/
	gameData.spaceshipInimigos[0].id = 0;
	gameData.spaceshipInimigos[0].tipoSpaceship = SPACESHIP_INIMIGO_TIPO_BASICO;
	gameData.spaceshipInimigos[0].pontosResistencia = 1;
	gameData.spaceshipInimigos[0].pontosVida = 1;
	gameData.spaceshipInimigos[0].tamanho.X = 3;
	gameData.spaceshipInimigos[0].tamanho.X = gameData.spaceshipInimigos->tamanho.Y;
	gameData.spaceshipInimigos[0].coordenadas.X = 5;
	gameData.spaceshipInimigos[0].coordenadas.Y = 0;

	gameData.spaceshipInimigos[1].id = 1;
	gameData.spaceshipInimigos[1].tipoSpaceship = SPACESHIP_INIMIGO_TIPO_DODGE;
	gameData.spaceshipInimigos[1].pontosResistencia = 3;
	gameData.spaceshipInimigos[1].pontosVida = 1;
	gameData.spaceshipInimigos[1].tamanho.X = 2;
	gameData.spaceshipInimigos[1].tamanho.X = gameData.spaceshipInimigos->tamanho.Y;
	gameData.spaceshipInimigos[1].coordenadas.X = 10;
	gameData.spaceshipInimigos[1].coordenadas.Y = 0;

	gameData.spaceshipInimigos[2].id = 2;
	gameData.spaceshipInimigos[2].tipoSpaceship = SPACESHIP_INIMIGO_TIPO_LENTO;
	gameData.spaceshipInimigos[2].pontosResistencia = 2;
	gameData.spaceshipInimigos[2].pontosVida = 2;
	gameData.spaceshipInimigos[2].tamanho.X = 5;
	gameData.spaceshipInimigos[2].tamanho.X = gameData.spaceshipInimigos->tamanho.Y;
	gameData.spaceshipInimigos[2].coordenadas.X = 15;
	gameData.spaceshipInimigos[2].coordenadas.Y = 0;

	/*
	Init tiros
	*/
	for (int i = 0; i < NUM_MAX_TIROS; i++) {
		gameData.tiros[i].id = i;
		gameData.tiros[i].tipo = SAB_TIPO_TIRO;
		gameData.tiros[i].coordenadas.X = SAB_INVALID;
		gameData.tiros[i].coordenadas.Y = SAB_INVALID;
		gameData.tiros[i].direcao = SAB_INVALID;
	}


	/*
	Cria mutex
	*/
	threadsMutex = CreateMutex(NULL, FALSE, TEXT("mutexThreadsNavesInimigas"));
	if (threadsMutex == NULL) {
		_tprintf(TEXT("CreateMutex error: %d\n"), GetLastError());
		return 1;
	}



	/*
	Lançamento de thread para controlar naves inimigas basicas
	*/
	handle_naves_inimigas_basicas = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaNavesInimigasBasicas, (LPVOID)(&gameData.spaceshipInimigos), 0, &threadID_naves_inimigas_basicas);
	if (handle_naves_inimigas_basicas == NULL) {
		_tprintf(TEXT("Erro: Não possivel a criação Threads das naves básicas.\n"));
		return 1;
	}

	/*
	Lançamento de thread para controlar naves inimigas dodge
	*/
	handle_naves_inimigas_dodge = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaNavesInimigasDODGE, (LPVOID)(&gameData.spaceshipInimigos), 0, &threadID_naves_inimigas_dodge);
	if (handle_naves_inimigas_dodge == NULL) {
		_tprintf(TEXT("Erro: Não possivel a criação Threads das naves básicas.\n"));
		return 1;
	}

	/*
	Lançamento de thread para controlar naves inimigas lentas
	*/
	handle_naves_inimigas_lento = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaNavesInimigasLento, (LPVOID)(&gameData.spaceshipInimigos), 0, &threadID_naves_inimigas_lento);
	if (handle_naves_inimigas_lento == NULL) {
		_tprintf(TEXT("Erro: Não possivel a criação Threads das naves básicas.\n"));
		return 1;
	}

	/*
	Lançamento de thread para controlar os tiros lançados
	*/
	handle_controla_tiros = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)controlaTirosLancados, (LPVOID)(&gameData.tiros), 0, &threadID_controla_tiros);
	if (handle_controla_tiros == NULL) {
		_tprintf(TEXT("Erro: Não possivel a criação Threads das naves básicas.\n"));
		return 1;
	}

	_gettch();

	CloseHandle(handle_naves_inimigas_basicas);
	CloseHandle(handle_naves_inimigas_dodge);
	CloseHandle(handle_naves_inimigas_lento);
	CloseHandle(handle_controla_tiros);
	CloseHandle(threadsMutex);
	return 0;
}

void imprimeNaveInimiga(SPACESHIP * naveInimiga) {
	HANDLE mutex;
	mutex = CreateMutex(NULL, FALSE, TEXT("mutexThreadsNavesInimigas"));
	WaitForSingleObject(mutex, INFINITE);
	goToXY(naveInimiga->coordenadas.X, naveInimiga->coordenadas.Y);
	_tprintf(TEXT("%d"), naveInimiga->id);
	naveInimiga->coordenadas.Y++;
	ReleaseMutex(mutex);
	CloseHandle(mutex);
}

DWORD WINAPI controlaNavesInimigasBasicas(LPVOID param) {
	SPACESHIP *naveInimiga = ((SPACESHIP *)param);
	while (1) {
		for (int i = 0; i < NUM_INIMIGOS; i++) {
			if (naveInimiga[i].tipoSpaceship == SPACESHIP_INIMIGO_TIPO_BASICO) {
				if (random(1, 100) < 85) {
					imprimeNaveInimiga(&naveInimiga[i]);
				}
				Sleep(SPACESHIP_BASE_SPEED);
			}
		}
	}
	return 0;
}

DWORD WINAPI controlaNavesInimigasDODGE(LPVOID param) {
	SPACESHIP *naveInimiga = ((SPACESHIP *)param);
	while (1) {
		for (int i = 0; i < NUM_INIMIGOS; i++) {
			if (naveInimiga[i].tipoSpaceship == SPACESHIP_INIMIGO_TIPO_DODGE) {
				if (random(1, 100) < 85) {
					imprimeNaveInimiga(&naveInimiga[i]);
				}
				Sleep((int)(SPACESHIP_BASE_SPEED * 1.2));
			}
		}
	}
	return 0;
}

DWORD WINAPI controlaNavesInimigasLento(LPVOID param) {
	SPACESHIP *naveInimiga = ((SPACESHIP *)param);
	while (1) {
		for (int i = 0; i < NUM_INIMIGOS; i++) {
			if (naveInimiga[i].tipoSpaceship == SPACESHIP_INIMIGO_TIPO_LENTO) {
				if (random(1, 100) < 85) {
					imprimeNaveInimiga(&naveInimiga[i]);
				}
				Sleep((int)(SPACESHIP_BASE_SPEED * 0.8));
			}
		}
	}
	return 0;
}

DWORD WINAPI controlaTirosLancados(LPVOID param) {
	SAB * tirosLancados = ((SAB *)param);
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

int random(int min, int max) {
	return(min + rand() % (max - min));
}
