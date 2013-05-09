
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <aio.h>
#include <errno.h>
#include <string.h>

typedef struct {
	
} async_cb;

int main(int argc, char **argv)
{

	char buffer[1024];
	struct aiocb a;
	int fd;
	fd = open("file.txt", O_RDONLY);
	if (fd < 0 ) {
		perror("open");
		return 1;
	}

	memset(&a, 0, sizeof(a));
	a.aio_fildes = fd;
	a.aio_buf = &buffer;
	a.aio_nbytes = 1024;
	
	int rv;
	rv = aio_read(&a);
	if (rv < 0) {
		perror("aio_read");
		close(fd);
		return 1;
	}
	
	int i = 0;
	do {
		printf("[%4d] doing some mess\n", i);
		rv = aio_error(&a);
		i++;
	} while(rv == EINPROGRESS);
	if (rv) {
		perror("aio_error");
		close(fd);
		return 1;
	}
	int bytes;
	bytes = aio_return(&a);
	/* now we have some date in the buffer */
	buffer[bytes] = 0;
	printf("got %d bytes\n", bytes);
	printf("data: '%s'", buffer);
	
	close(fd);
	return 0;
}
