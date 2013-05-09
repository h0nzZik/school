#include "common.h"

struct message *get_message_mem(int bytes, const char *mname, const char *sname, sem_t **sem)
{
	int fd;
	struct message *ms;
	bool init = false;

	/* try to open it */
	fd = shm_open(mname, O_RDWR, 0666);
	if (fd == -1)
		init = true;
	if (init) {
		/* ok, try to create new */
		fd = shm_open(mname, O_CREAT | O_RDWR, 0666);
		if (fd == -1) {
			perror("shm_open");
			goto error;
		}

		if(ftruncate(fd, bytes + 2*sizeof(int)))
			goto err_trunc;
	}

	ms = (struct message *) mmap(NULL, bytes + sizeof(*ms),
			PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (!ms)
		goto err_mmap;

	*sem = sem_open(sname, O_CREAT, 0666, 0);
	if (!*sem)
		goto err_sem;

	if (init) {
		/* clear it */
		ms->size = bytes;
		ms->len = 0;
		ms->pid = 0;
		sem_post(*sem);
	}
	
//	sem_post(*sem);


	close(fd);
	return ms;

err_sem:
	munmap(ms, bytes + 2*sizeof(int));
err_mmap:
err_trunc:
	close(fd);
error:
	perror("error while initializing memory");
	return NULL;
}

