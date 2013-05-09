#ifndef _COMMON_H
#define _COMMON_H

#define _BSD_SOURCE
#include <stdio.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define SHM_NAME "/super_chat_memory"
#define SEM_NAME "/super_chat_semaph"


struct message {
	int size;
	int len;
	int pid;
};

struct message *get_message_mem(int bytes, const char *mname, const char *sname, sem_t **sem);

#endif
