#define _POSIX_C_SOURCE	200112L
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>

#define SHM_LEN	1024

int producent(void *mem)
{
	const char *s = "testovaci data";
	memcpy(mem, s, strlen(s)+1);

	fgets(mem, SHM_LEN, stdin);
	return 0;
}
int consument(void *mem)
{
	char buff[SHM_LEN];
	memcpy(buff, mem, SHM_LEN);

	buff[SHM_LEN-1]=0;
	printf("got: %s\n", buff);

	return 0;
}

int main(int argc, char **argv)
{

	sem_t *s;
	bool cons = false;
	int shfd;

	if (argc != 1)
		cons = true;

//	const char *shm_name = "/tmp/my_lovely_memory";
	const char *shm_name = "my_lovely_memory";

#if 0
	s = sem_open("/my_shm_sem", O_CREAT, 0666, 5);
	if (s == NULL) {
		perror("can't create semaphore\n");
		return 1;
	}
#endif

	int flags;
	flags = O_CREAT;
	if (cons == true)
		flags |= O_RDONLY;
	else
		flags |= O_RDWR;

	shfd = shm_open(shm_name, flags, 0666);
	if (shfd < 0) {
		perror("shm_open");
		return 1;
	}

	/* producent have to truncate it */
	if (cons == false) {
		if(ftruncate(shfd, SHM_LEN)) {
			perror("ftruncate");
			return 1;
		}
	}

	flags = PROT_READ;
	if (cons == false)
		flags |= PROT_WRITE;
	void *mem;
	mem = mmap(NULL, SHM_LEN, flags, MAP_SHARED, shfd, 0);
	if (!mem) {
		perror("mmap");
		return 1;
	}




	int rv;
	if (cons)
		rv = consument(mem);
	else
		rv = producent(mem);
	munmap(mem, SHM_LEN);

	close(shfd);
//	shm_unlink(shm_name);
	printf("exitting with %d\n", rv);
	return rv;



	/*********************/
	exit(107);

	
	if (sem_trywait(s)) {
		printf("musime pockat\n");
		sem_wait(s);
	} else {

		;
//		printf("jdu dovnitr..");
//		sem_wait(s);
	}
	printf("fajn\n");
	sleep(5);
	sem_post(s);

	return 0;
}
