#include "controloNaves.h"

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