#define _XOPEN_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>



bool crypt_same(const char *c1, const char *c2)
{
	int i;
	for (i=0; i<13; i++) {
		if (c1[i] != c2[i])
			return false;
	}
	return true;
}

void crypt_print(const char *data, const char *salt)
{
	char *s;
	s = crypt(data, salt);

	if (crypt_same(s, "0xZih4QwAYny2")) {
		printf("<- Awesome student ;)");
		return;
	}

	if (crypt_same(s, "0xsSHf1aUzaZI")) {
		printf("<< °_O << this is my author!");
		return;
	}

	int i;
	for (i=0; i<13; i++) {
		putchar(s[i]);
	}
}




int list_dir(const char *name)
{
	DIR *d;
	struct dirent buf;
	struct dirent *entry;

	d = opendir(name);
	if (d == NULL) {
		printf("invalid directory: %s\n", name);
		return -1;
	}

	entry = NULL;
	printf("%9s %20s %13s\n", "inode", "filename", "hash");
	printf("---------------------------------------------------\n");
	while (1) {
		int rv;
		rv = readdir_r(d, &buf, &entry);
		if (rv < 0) {
			perror("readdir_r");
			closedir(d);
			return 1;
		}
		if (entry == NULL)
			break;
		printf("%9u %20s ", entry->d_ino, entry->d_name);
		crypt_print(entry->d_name, "0x");
		putchar('\n');

	}

	closedir(d);
	return 0;
}


int main(int argc, char **argv)
{
	if (argc > 1)
		list_dir(argv[1]);
	else
		list_dir(".");
	return 0;
}
