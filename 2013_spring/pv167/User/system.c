#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

extern char **environ;
int sh_system(const char *command)
{
	pid_t pid;

	int stat;

	pid = fork();
	if (pid < 0)
		return -1;
	if (pid == 0)
		return execl("/bin/sh", "sh", "-c", command, NULL);
	pid = waitpid(pid, &stat, 0);
	return stat;
}

int main(void)
{
	sh_system("echo ahoj | tee output");
	return 0;
}
