/*
 *                  R S 2 3 2 - thermometer - e x a m p l e
 *
 * Description : Simple demo how to control serial port from GNU/Linux.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : GNU/Linux
 *
 * Compiler : gcc 4.1.0
 *
 * $Id: temp.c,v 1.3 2006/11/25 09:04:59 celeda Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <unistd.h>

int ser_port;			/* serial port handle */
struct termios ser_port_old_cfg;/* old serial port configuration */

void signal_handler(int signal)
{
	printf("\n\n");
				/* restore serial port settings */
	tcsetattr (ser_port, TCSANOW, &ser_port_old_cfg);
	exit(0);
}

int main(int argc, char* argv[])
{
	struct termios	ser_port_cfg,	/* serial port configuration */
			ser_port_old_cfg; /* old serial port configuration */
	char	*ser_port_name = "/dev/ttyUSB0",/* serial port default name */
		readbuffer[256],	/* read buffer */
		*p;
	int	readsize;		/* number of read bytes */

	system("clear");		/* clear screen - system function call */

	if(argv[1])			/* get serial port name from command line */
		ser_port_name = argv[1];

					/* open serial port */
	ser_port = open(ser_port_name, O_RDWR | O_NOCTTY);

	if(ser_port == -1)
	{
		perror(ser_port_name);		/* print a system error message and exit */
		exit(-1);
	}

	tcgetattr(ser_port, &ser_port_old_cfg);	/* save current port settings */

	/* restore original termios settings on exit */
	signal(SIGINT,  signal_handler);
	signal(SIGTERM, signal_handler);

						/* setup serial port 9600,N,8,1 */
	bzero(&ser_port_cfg, sizeof(ser_port_cfg));
	ser_port_cfg.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	ser_port_cfg.c_iflag = IGNPAR;
	ser_port_cfg.c_oflag = 0;
	ser_port_cfg.c_lflag = 0;

	ser_port_cfg.c_cc[VTIME] = 0;	/* inter-character timer unused */
	ser_port_cfg.c_cc[VMIN] = 5;	/* blocking read until 5 chars received */

	tcflush(ser_port, TCIOFLUSH);	/* discard serial port I/O data */

	tcsetattr(ser_port, TCSANOW, &ser_port_cfg);	/* set new serial port configuration */

	printf("\n      --== Serial Port Thermometer ==-- press CTRL+C to exit\n\n");

	do
	{
		write(ser_port, "#", 1);	/* request temperature */
		sleep(1);

		readsize = read(ser_port, readbuffer, 255);	/* read response */

		readbuffer[readsize]=0;
		p = readbuffer;

		while(*p)	/* convert cp1252 to iso 8859-1 */
		{
			if((unsigned char)*p == 0xF8)
				*p = 0xB0;		/* degree sign */
			p++;
		}

		printf("%s", readbuffer);
		fflush(stdout);

	} while(1);

	tcsetattr(ser_port, TCSANOW, &ser_port_old_cfg);	/* restore port settings */

	close(ser_port);	/* close serial port */

	return (0);
}

