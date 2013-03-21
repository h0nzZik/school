// 02-handle.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	int i;
	for (i=0; i<argc; i++) {
		printf("[%2d] '%s'\n", i, argv[i]);
	}

	HANDLE h;
	if (argc < 2 || _stscanf(argv[1], L"%p", &h) != 1) {
		printf("usage: %s handle\n", argv[0]);
		system("pause");
		return 1;
	}

	printf("handle: %p\n", h);	
	system("pause");
	return 0;
}

