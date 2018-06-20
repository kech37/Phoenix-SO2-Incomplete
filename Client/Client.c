#include "../DLL/DLL.h"

DWORD WINAPI EnviaPedidos(LPVOID param);

void printError(unsigned short * msg);
void printLog(unsigned short * msg);

BOOLEAN continua = TRUE;
PLAYERINFO aEnviar;
GAMEDATA estadoJogo;
BOOLEAN firstTime = TRUE;

int _tmain(int argc, LPTSTR argv[]) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif	
	/*HANDLE hPipe, IOReady;
	OVERLAPPED Overlapped;
	DWORD sizeTransferred, modo;
	HANDLE thread;

	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
		printError(TEXT("18: !WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)"));
		return 1;
	}

	hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);
	if (hPipe == NULL) {
		printError(TEXT("24: (hPipe == NULL"));
		return 1;
	}

	//ALTERAR MODO LEITURA DO PIPE PARA "MESSAGE"...
	modo = PIPE_READMODE_MESSAGE;
	SetNamedPipeHandleState(hPipe, &modo, NULL, NULL);
	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EnviaPedidos, (LPVOID)hPipe, 0, NULL);

	while (continua) {
		ZeroMemory(&Overlapped, sizeof(OVERLAPPED));
		ResetEvent(IOReady);
		Overlapped.hEvent = IOReady;

		ReadFile(hPipe, &estadoJogo, sizeof(GAMEDATA), &sizeTransferred, &Overlapped);
		WaitForSingleObject(IOReady, INFINITE);

		if (!GetOverlappedResult(hPipe, &Overlapped, &sizeTransferred, TRUE)) {
			printError(TEXT("42: !GetOverlappedResult(hPipe, &overlapped, &sizeTransferred, TRUE)"));
			return 1;
			break;
		}*/
		/**********************************************/
		/* Recebeu e deverá imprimir o estado de jogo */
		/**********************************************/
		/*switch (estadoJogo.estado){
			case PREJOGO:
				if (firstTime) {
					for (int i = 0; i < MAX_PLAYERS_SIZE; i++) {
						if (estadoJogo.defensores[i].jogador.id == 0) {
							estadoJogo.defensores[i].jogador.id = (i + 1);
							aEnviar.id = (i + 1);
							break;
						}
					}
					_tprintf_s(TEXT("Nome de jogador: "));
					wscanf_s(TEXT("%s"), &aEnviar.nome, PLAYER_NAME_SIZE);
					firstTime = FALSE;
				}
			break;
			case A_JOGAR:

			break;
			case POSJOGO:

			break;
			case TERMINADO:
				continua = FALSE;
			break;
			default:
			break;
		}
	}

	WaitForSingleObject(thread, INFINITE);

	CloseHandle(thread);
	CloseHandle(hPipe);*/



	return 0;
}

DWORD WINAPI EnviaPedidos(LPVOID param) {
	DWORD sizeTransferred;
	HANDLE IOReady;
	OVERLAPPED Overlapped;
	int temp;

	// Ler do teclado
	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	do {
		switch (estadoJogo.estado) {
		case PREJOGO:
			//break;
		case A_JOGAR:
			if (!firstTime) {
				_tscanf_s(TEXT("%d"), &temp);
				switch (temp) {
				case 8:
					aEnviar.jogada = CIMA;
					break;
				case 5:
					aEnviar.jogada = BAIXO;
					break;
				case 4:
					aEnviar.jogada = ESQUERDA;
					break;
				case 6:
					aEnviar.jogada = DIREITA;
					break;
				case 7:
					aEnviar.jogada = ESPAÇO;
					break;
				default:
					aEnviar.jogada = NULA;
					break;
				}

				//Enviar PEdido ao escritor/GateWay
				ZeroMemory(&Overlapped, sizeof(OVERLAPPED));
				ResetEvent(IOReady);
				Overlapped.hEvent = IOReady;

				WriteFile((HANDLE)param, &aEnviar, sizeof(PLAYERINFO), &sizeTransferred, &Overlapped);

				WaitForSingleObject(IOReady, INFINITE);
				if (!GetOverlappedResult((HANDLE)param, &Overlapped, &sizeTransferred, TRUE)) {
					printError(TEXT("91: !GetOverlappedResult((HANDLE)param, &Overlapped, &sizeTransferred, TRUE)"));
					return 1;
				}
				printLog(TEXT("Item enviado com sucesso"));
			}
			break;
		case POSJOGO:
			break;
		default:
			break;
		}
	} while (continua);

	return 0;
}

void printError(unsigned short * msg) {
	_tprintf(TEXT("[Client]Error> '%s'\n"), msg);
}

void printLog(unsigned short * msg) {
	_tprintf(TEXT("[Client]Log> '%s'\n"), msg);
}