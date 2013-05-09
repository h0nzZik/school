#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>


#define N_PIPES	5
int main(void)
{
	int rv;
	struct pollfd fd[N_PIPES];
	int i;
	for (i=0; i<N_PIPES; i++) {
		char buff[80];
		sprintf(buff, "my_fifo_%d", i);
		printf("opening %s\n", buff);
		rv = open(buff, O_RDONLY | O_NONBLOCK);
		if (rv < 0) {
			perror("open");
			return 1;
		}
		fd[i].fd = rv;
		fd[i].events = POLLIN | POLLNVAL;
		fd[i].revents = 0;
		printf("</opening>\n");
	}

	char buffer[1024];


	while(1) {
		poll(fd,N_PIPES, -1);
		/* find something */
		for (i=0; i<N_PIPES; i++) {
			if (fd[i].revents & POLLNVAL) {
				printf("#%d is invalid\n", i);
				goto error;

			}
			if (fd[i].revents & POLLIN) {
				printf("reading from #%d\n", i);
				rv = read(fd[i].fd, buffer, 1024);
				if (rv < 0) {
					perror("read");
					goto error;
				}
				buffer[rv] = 0;
				printf("=> '%s'\n", buffer);
//				fd[i].revents = 0;
			}

		}
	}
	return 0;
error:
	printf("some error\n");
	return 1;

	return 0;
}
