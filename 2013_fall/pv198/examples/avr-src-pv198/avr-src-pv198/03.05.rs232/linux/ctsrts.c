/*
 *                  R S 2 3 2 - modem signals - e x a m p l e
 *
 * Description : Sets and shows serial port modem signals.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : GNU/Linux
 *
 * Compiler : gcc 4.1.0
 *
 * $Id: ctsrts.c,v 1.4 2006/11/26 10:16:56 celeda Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

void show_modem_signals(int flags)
{
	printf("RTS %c ", flags & TIOCM_RTS ? '1' : '0');
	printf("CTS %c ", flags & TIOCM_CTS ? '1' : '0');
	printf("DTR %c ", flags & TIOCM_DTR ? '1' : '0');
	printf("DSR %c ", flags & TIOCM_DSR ? '1' : '0');
	printf("CD %c ", flags & TIOCM_CD ? '1' : '0');
	printf("RI %c \r", flags & TIOCM_RI ? '1' : '0');
	fflush(stdout);
}

int main(int argc, char* argv[])
{

	int	ser_port;		/* serial port handle */
	char	*ser_port_name = "/dev/ttyUSB0";/* serial port default name */
	int	flags;			/* modem signals */

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

	ioctl(ser_port, TIOCMGET, &flags);	/* get modem signals */

	printf("\n      --== Serial Port Modem Lines Status ==-- press CTRL+C to exit\n\n");

	while(1)
	{
		/* toggles RTS/DTR signals every 2s */

		flags &= ~TIOCM_RTS;			/* clear RTS */
		flags &= ~TIOCM_DTR;			/* clear DTR */
		ioctl(ser_port, TIOCMSET, &flags);	/* apply changes to serial port */
		ioctl(ser_port, TCSBRK, 1);		/* clear TXD - this last only for 0.1 s */

		usleep(100);				/* wait before reading new status */

		ioctl(ser_port, TIOCMGET, &flags);	/* get modem signals */
		show_modem_signals(flags);

		sleep(2);

		flags |= TIOCM_RTS;			/* set RTS */
		flags |= TIOCM_DTR;			/* set DTR */
		ioctl(ser_port, TIOCMSET, &flags);	/* apply changes to serial port */
		ioctl(ser_port, TCSBRK, 0);		/* set TXD */

		usleep(100);				/* wait before reading new status */

		ioctl(ser_port, TIOCMGET, &flags);	/* get modem signals */
		show_modem_signals(flags);

		sleep(2);
	}

	close(ser_port);	/* close serial port */

	return (0);
}

