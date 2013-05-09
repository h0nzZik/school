#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#if 0
void *thread_func(void *arg)
{
#if 0
	printf("thread #%d\n", (int)arg);
	printf("@%p\n", pthread_self());
	return NULL;
#endif
	int i;
	int c;
	c = (char)arg;
	c &=0x00FF;
	for (i=0; i<10000 || 1; i++){
		if (rand()%100 == 0)
			usleep(1000);
		putchar(c);
	}
}


int global_i = 0;
pthread_mutex_t m;

void *thread_func1(void *arg)
{

	int i;
	for (i=0; i<1000000; i++) {
		pthread_mutex_lock(&m);
		global_i++;
		pthread_mutex_unlock(&m);

	}
}



int old_main(void)
{
	const char n_threads = 100;
	pthread_t threads[n_threads];

	pthread_mutex_init(&m, NULL);
	srand(time(NULL));
	int i;
	int rv;
	for (i=0; i<n_threads; i++) {
		int c;
		c = '#'+i;
		rv = pthread_create(&threads[i], NULL, thread_func1, (void *)c);
		if (rv) {
			perror("can't create thread");
			return 1;
		}
	}

	for (i=0; i<n_threads; i++) {
		rv = pthread_join(threads[i], NULL);
		if (rv)
			perror("pthread_join");
	}

	printf("hello world: %d\n", global_i);
	return 0;
}
#endif

FILE *fw;
pthread_mutex_t lock;


void *writer(void *data)
{
	size_t x;
	x = (size_t)data;

	do {
		pthread_mutex_lock(&lock);
		fprintf(fw, "%zd", x);
		pthread_mutex_unlock(&lock);
	} while(1);
	return NULL;
}


/*
 * Prints how much there is character 'c' in file 'fname'
 * #1	possibly vulnerable
 * #2	slow
 * #3	shell inside c ;)
 */
void write_characters_in_file(const char *fname, char c)
{
	char buff[400];
	sprintf(buff, "echo There is `cat '%s' | sed -e \"s/[^%c]//g\" | wc -c` times character '%c' in file %s", fname, c, c, fname);
//	printf("tohle: %s\n", buff);
	system(buff); /* ;-) */
}

int main(void)
{
	fw = fopen("output.txt", "w");
	if (!fw) {
		perror("fopen");
		return 1;
	}

	pthread_mutex_init(&lock, NULL);

	char buff[20];
	size_t i;
	const size_t n=2;
	int rv;
	pthread_t threads[2];

	for(i=0; i<n; i++) {
		rv = pthread_create(&threads[i], NULL, writer, (void *)i);
		if (rv) {
			perror("pthread_create()");
			return 1;
		}
	}

	sleep(3);

	for(i=0; i<n; i++) {
		rv = pthread_cancel(threads[i]);
		if (rv) {
			perror("pthread_cancel()");
			return 1;
		}
	}
	fclose(fw);

//	printf("there is ");

	write_characters_in_file("output.txt", '0');
	write_characters_in_file("output.txt", '1');
	/* vulnerability */
	const char *some_filename = "/dev/null' 2>/dev/null; /bin/sh'";
//	write_characters_in_file(some_filename, '#');
	return 0;

}
