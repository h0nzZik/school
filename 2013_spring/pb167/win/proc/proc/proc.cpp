/*  */

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>



int _tmain(int argc, _TCHAR* argv[])
{
	int b;
	printf("hello world\n");

	STARTUPINFO si;
	PROCESS_INFORMATION pi;


	OFSTRUCT ofs;
	SECURITY_ATTRIBUTES sa;

	ZeroMemory((void *)&si, sizeof(si));
	ZeroMemory((void *)&pi, sizeof(pi));
	ZeroMemory((void *)&sa, sizeof(sa));


	/*  */
	HANDLE fw;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	fw = CreateFile(L"stdout", GENERIC_WRITE, 0, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (fw == NULL) {
		fprintf(stderr, "can't open file: 0x%x\n", GetLastError());
		system("pause");
		return 1;
	}

	ZeroMemory((void *)&si, sizeof(si));
	ZeroMemory((void *)&si, sizeof(pi));


	si.cb = sizeof(si);
	si.hStdOutput = fw;

	si.dwFlags = STARTF_USESTDHANDLES;

	//const WCHAR *exename = L"C:\\Windows\\system32\\tree.com";
	const WCHAR *exename = L"02-handle.exe";
	b = CreateProcess (exename, NULL, NULL, NULL,
			/* inherit handles */ true, NORMAL_PRIORITY_CLASS, NULL, 
			/* current directory */	NULL, &si, &pi);
	if (!b) {
		fprintf(stderr, "some error occured: %d\n", GetLastError());
		system("pause");
		return 1;
	}


	DWORD rv;
	rv = WaitForSingleObject(pi.hProcess, 12000);
	if (rv == WAIT_TIMEOUT) {
		printf("no free timeleft\n");
	} else {
		printf("done");
	}
	printf("closig: 0x%x\n", rv);



	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	system("pause");
	return 0;
}

