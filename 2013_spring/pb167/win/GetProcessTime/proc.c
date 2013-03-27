/*  */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdbool.h>

void print_time(FILETIME *t)
{
	char buff[80];
	SYSTEMTIME st;

	FileTimeToSystemTime(t, &st);

	GetTimeFormat(LOCALE_USER_DEFAULT,0,&st,NULL,buff,80);
	printf("%s\n", buff);
}


int _tmain(int argc, _TCHAR* argv[])
{

	HANDLE self;
	bool rv;
	FILETIME ct, et, kt, ut;
	memset(&ct, 0, sizeof(ct));
	memset(&et, 0, sizeof(ct));
	memset(&kt, 0, sizeof(ct));
	memset(&ut, 0, sizeof(ct));

	Sleep( 2500);
	/* gets pseudo-handle (in fact -1) */
	self = GetCurrentProcess();
	rv = GetProcessTimes(self, &ct, &et, &kt, &ut);
	if (!rv) {
		// missing some simple perror()-like function
		printf("an error\n");
		return -1;
	}
	print_time(&ct);	// creation time
	print_time(&et);	// exit time - undefined (we are still running)
	print_time(&kt);	//
	print_time(&ut);

	return 0;
}

