#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>


int main(void)
{
	sem_t *s;

	s = sem_open("/my_semaphore", O_CREAT, 0666, 5);
	if (s == NULL) {
		perror("can't create semaphore\n");
		return 1;
	}
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
