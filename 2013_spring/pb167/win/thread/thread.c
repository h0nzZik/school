#include <stdio.h>
#include <windows.h>


int global_variable;

int thread_func()
{

	printf("hello from thread\n");
	return 0;
}

void __stdcall thread_func_stdcall(void *data)
{

	int i;
	for (i=0; i<1000000; i++) {
//		printf("global: %d\n", global_variable);
//		putchar((char)data);
		global_variable++;
	}
//	printf("hello from thread %d\n", (int)data);
}




int main(int argc, char **argv)
{

	printf("hello world\n");

	global_variable=0;
	const int num = 10;

	HANDLE h[num];

	int i;
	for (i=0; i<num; i++)
		h[i] = CreateThread(NULL, 0, thread_func_stdcall, (void *)('0'+i), 0, NULL);

	for (i=0; i<num; i++)
		WaitForSingleObject(h[i],INFINITE);

	printf("exitting\n");
	printf("global var: %d\n", global_variable);

	return 0;
}


