#include <stdio.h>
#include <windows.h>
#include <stdbool.h>


CRITICAL_SECTION cs;
FILE *fw;

void __stdcall writer(void *data)
{
	while(1){
		/* doing some 'usefull' work */
		EnterCriticalSection(&cs);
		fputc((char)data, fw);
		LeaveCriticalSection(&cs);


		/* message handling */
		MSG msg;
		if (PeekMessage(&msg, NULL,WM_USER, WM_USER+1, PM_REMOVE)) {
			printf("got message: %c\n", (char)data);
			break;
		}
	}
}



void print_stat(const char *fname, int n)
{
	FILE *fr;
	fr = fopen(fname, "rt");
	if (!fr)
		return;

	int x[n];
	memset(x, 0, sizeof(x));

	int c;
	while((c=fgetc(fr)) != EOF){
		if (c < '0')
			continue;
		if (c > '0' + n - 1)
			continue;
		x[c-'0']++;
	}

	int i;
	for (i=0; i<n; i++) {
		printf("%d x %d\n", i, x[i]);
	}


	fclose(fr);
}


int main(int argc, char **argv)
{

	const int num = 10;
	int i;
	HANDLE h[num];



	/* open the file */
	fw = fopen("output.txt", "wt");
	if (!fw) {
		perror("fopen");
		return 1;
	}

	InitializeCriticalSection(&cs);


	/* spawn threads */
	for (i=0; i<num; i++)
		h[i] = CreateThread(NULL, 0, writer, (void *)('0'+i), 0, NULL);


	/* wait for 5s */
	Sleep(5000);

	/* destroy the threads */
	for (i=0; i<num; i++) {
		PostThreadMessage(GetThreadId(h[i]), WM_USER, 0,0);

	}

	/* wait for the threads */
	WaitForMultipleObjects(num, h, true, INFINITE);

	fclose(fw);

	print_stat("output.txt", num);
	return 0;
}


