#pragma once
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <io.h>
#include <windows.h>
#include <stdlib.h> 
#include <fcntl.h>
#include "../DLL/dll.h"

#define _CRT_SECURE_NO_WARNINGS
HANDLE hThreadEscritor;
DWORD WINAPI ThreadProdutor(LPVOID param); 
