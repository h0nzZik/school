

#include "common.h"

/* Some ANSI terminal stuff */
#define screen_clear(x)	do {\
	printf("\033[2J");\
	printf("\033[1;1H");\
	} while(0)
#define cursor_up(x)	printf("\033[%dA", x)
#define line_erase(x)	printf("\033[%dK", x)
#define color_set(x)	printf("\033[1;%dm", x)
#define fg_color_clr() printf("\033[0m");

void wtfprint(const char *str, int percent)
{
	int i = rand()%10;
	while(*str) {
		color_set(30+i);
		fputc(*str, stdout);
		fg_color_clr();
		if (rand()%100 <= percent)
			i = (i+1)%10;
		*str++;
	}

}
const char *logo = ""
"    _            ____ _           _   _            \n"
"   / \\__      __/ ___| |__   __ _| |_| |_ ___ _ __ \n"
"  / _ \\ \\ /\\ / / |   | '_ \\ / _` | __| __/ _ \\ '__|\n"
" / ___ \\ V  V /| |___| | | | (_| | |_| ||  __/ |   \n"
"/_/   \\_\\_/\\_/  \\____|_| |_|\\__,_|\\__|\\__\\___|_|\n\n";
int main(void)
{
	struct message *ms;
	char *s;
	sem_t *sem;
	srand(time(NULL));
	ms = get_message_mem(1024, SHM_NAME, SEM_NAME, &sem);
	if (ms == NULL) {
		printf("can't get it\n");
		return 1;
	}
	s = (char *) (ms + 1);

//	printf("%p, %p\n", ms, s);

	screen_clear();
	wtfprint("Welcome to my awesome chatter\n", 100);
	wtfprint(logo, 50);
	char buff[80];
	while(1) {
		wtfprint("Write a message here -> ", 10);
		if (NULL == fgets(buff, 80, stdin))
			break;

		buff[79] = 0;
		int len = strlen(buff);

		/* newline */
		if (len && buff[len-1] == '\n')
			buff[len-1]=0;


		/* Clear the line */
		cursor_up(1);	// one character up
		line_erase(2);	// all line
	
		/* wait for remote.. */
		sem_wait(sem);

		
		/* is there any unread message? */
		if (ms->len != 0 && ms->pid != getpid() && ms->pid != 0) {
			color_set(31);
			printf("remote:");
			fg_color_clr();
			printf("\t%s\n", s);
			ms->len = 0;
		}


		/* is the buffer non-empty? */
		if (buff[0] && !isspace(buff[0])) {
			/* print it (again) */

			color_set(32);
			printf("local:");
			fg_color_clr();
			printf("\t%s\n", buff);

			/* copy message to the buffer */
			strcpy(s, buff);
			ms->pid = getpid();
			ms->len = strlen(s);
		}		

		sem_post(sem);
	}
	return 0;
}
