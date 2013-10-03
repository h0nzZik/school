/*
 *                  R S 2 3 2 - modem signals - e x a m p l e
 *
 * Description : Sets and shows serial port modem signals. For more details
 *               see http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnfiles/html/msdn_serial.asp
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : MS Windows 2000, XP, ...
 *
 * Compiler : gcc (GCC) 3.4.2 (mingw-special)
 *
 * $Id: ctsrts.c,v 1.1 2006/11/25 12:16:41 celeda Exp $
 */
#include <stdio.h>
#include <conio.h>
#include <windows.h>

/*! \brief Display programm error and exit.
 *
 * \param s user defined error string
 */
void err(char *s)
{
	LPTSTR lpBuffer;

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, GetLastError(), LANG_USER_DEFAULT, (LPTSTR)&lpBuffer, 0, NULL );

	printf("%s %s\n", s, lpBuffer);
	exit(-1);
}

/*! \brief Display modem lines status.
 *
 * \param ser_port serial port to observe
 * \param rts current value of rts/dtr signals
 */
void show_modem_signals(HANDLE	ser_port, int rts)
{
	DWORD	ModemStat;	/* modem status */

	/* Note - there is no way to read RTS/DTR signals. */

	GetCommModemStatus(ser_port, &ModemStat);

	printf("RTS %c ", rts ? '1' : '0');
	printf("CTS %c ", ModemStat & MS_CTS_ON ? '1' : '0');
	printf("DTR %c ", rts ? '1' : '0');
	printf("DSR %c ", ModemStat & MS_DSR_ON ? '1' : '0');
	printf("CD %c ", ModemStat & MS_RLSD_ON ? '1' : '0');
	printf("RI %c \r", ModemStat & MS_RING_ON ? '1' : '0');
}

int main(int argc, char* argv[])
{
	HANDLE	ser_port;		/* serial port handle */
	char	*ser_port_name = "COM1";/* serial port default name */

	system("cls");			/* clear screen - system function call */

	if(argv[1])			/* get serial port name from command line */
		ser_port_name = argv[1];

					/* open serial port */
	ser_port = CreateFile(ser_port_name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);

	if (ser_port == INVALID_HANDLE_VALUE)
		err("CreateFile failed with error -");

	printf("\n      --== Serial Port Modem Lines Status ==-- press any key to exit\n\n");

	do
	{
		/* toggles RTS/DTR signals every 2s */

		EscapeCommFunction(ser_port, SETRTS);	/* set RTS */
		EscapeCommFunction(ser_port, SETDTR);	/* set DTR */
		EscapeCommFunction(ser_port, SETBREAK);	/* set TXD */

		show_modem_signals(ser_port, 1);
		Sleep(2000);

		EscapeCommFunction(ser_port, CLRRTS);	/* clear RTS */
		EscapeCommFunction(ser_port, CLRDTR);	/* clear DTR */
		EscapeCommFunction(ser_port, CLRBREAK);	/* clear TXD */

		show_modem_signals(ser_port, 0);
		Sleep(2000);

	} while(!kbhit());		/* press any key to terminated */

	if(!CloseHandle(ser_port))	/* close serial port */
		err("CloseHandle failed with error -");

	while(kbhit()) getch();		/* clear keyboard buffer */

	return (0);
}

