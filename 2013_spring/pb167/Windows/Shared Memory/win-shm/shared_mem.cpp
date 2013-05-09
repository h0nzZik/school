
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>



struct my_mem {
	int total;
	int idx;
	char arr[];
};

#define SHM_NAME	L"M_shared_memory"
#define SHM_EVENT	L"M_shm_event"
#define SHM_MUTEX	"M_shm_mutex"

int main(int argc, char *argv[])
{
	HANDLE map;

	bool srvr = true;
	if (argc > 1 && !strcmp(argv[1], "client")) {
		srvr = false;

	}




	HANDLE ev;
	/* create event */
	if (srvr) {
		ev = CreateEventW(NULL, false, false, SHM_EVENT);
	} else {
		ev = OpenEventW(EVENT_ALL_ACCESS, false, SHM_EVENT);
	}
	if (ev == NULL) {
		printf("can't create/open event");
		exit(1);
	}
	

	HANDLE mutex;
	if (srvr) {
		mutex = CreateMutex(NULL, false, SHM_MUTEX);
	} else {
		mutex = OpenMutex(SYNCHRONIZE, false, SHM_MUTEX);
	}
	if (mutex == NULL) {
		printf("mutex error\n");
		exit(1);
	} 


	/* Shared memory */
	if (srvr) {
		map = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, SHM_NAME);
	} else {
		map = OpenFileMappingW(FILE_MAP_ALL_ACCESS, false, SHM_NAME);
	}
	if (map == NULL) {
		fprintf(stderr, "Can't get file mapping: %lu\n", GetLastError());
		exit(1);
	}
	void *mem;
	mem = MapViewOfFile(map, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!mem) {
		fprintf(stderr, "MapViewOfFile() failed with %lu\n", GetLastError());
		exit(2);
	}
	CloseHandle(map);

	struct my_mem *mm;
	mm = (struct my_mem *)mem;	

	if (srvr == false) {
		/* writer */
		printf("client\n");
		mm->total = 4096;
		while(1) {
			int x = getchar();

			WaitForSingleObject(mutex, INFINITE);
			printf("idx(%d) < total(%d)\n", mm->idx, mm->total);
			if (mm->idx < mm->total) {
				mm->arr[mm->idx] = x;
				mm->idx++;
			}
		
			SetEvent(ev);
			ReleaseMutex(mutex);
			/* end of file */
			if (x < 0)
				break;			
		}
	} else {
		/* reader */
		while(1) {
			WaitForSingleObject(ev, INFINITE);
			WaitForSingleObject(mutex, INFINITE);

			int i;
			bool stop = false;
			for (i=0; i<mm->idx; i++) {
				if (mm->arr[i] < 0)
					stop = true;
				printf("%c", mm->arr[i]);
			}
			mm->idx = 0;
			ReleaseMutex(mutex);
			if (stop)
				break;
		}
	}
	
	UnmapViewOfFile(mem);
	CloseHandle(ev);
	CloseHandle(mutex);




	system("pause");
	return 0;
}

