/*
 *                  R S 2 3 2 - thermometer - e x a m p l e
 *
 * Description : Simple demo how to control serial port from MS Windows.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : MS Windows 2000, XP, ...
 *
 * Compiler : gcc (GCC) 3.4.2 (mingw-special)
 *
 * $Id: temp.c,v 1.3 2006/11/24 09:06:07 celeda Exp $
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

int main(int argc, char* argv[])
{
	HANDLE	ser_port;		/* serial port handle */
	DCB	dcb;			/* control settings for a serial port */
	char	*ser_port_name = "COM1",/* serial port default name */
		readbuffer[256]	;	/* read buffer */
	DWORD	readsize;		/* number of read bytes */

	system("cls");			/* clear screen - system function call */

	if(argv[1])			/* get serial port name from command line */
		ser_port_name = argv[1];

					/* open serial port */
	ser_port = CreateFile(ser_port_name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);

	if (ser_port == INVALID_HANDLE_VALUE)
		err("CreateFile failed with error -");

	if (!GetCommState(ser_port, &dcb))	/* get current serial port control settings */
		err("GetCommState failed with error -");

						/* serial port settings - 9600, N,8,1 */
	dcb.BaudRate = CBR_9600;		/* baud rate */
	dcb.ByteSize = 8;			/* data size */
	dcb.Parity = NOPARITY;			/* no parity bit */
	dcb.StopBits = ONESTOPBIT;		/* one stop bit */
	dcb.fDtrControl=DTR_CONTROL_DISABLE;	/* DTR handshake disabled */
	dcb.fRtsControl=RTS_CONTROL_DISABLE;	/* RTS handshake disabled */

	if(!SetCommState(ser_port, &dcb))		/* configure serial port */
		err("SetCommState failed with error -");

	printf("\n      --== Serial Port Thermometer ==--\n\n");

	do
	{
		PurgeComm(ser_port, PURGE_RXCLEAR);	/* discard all characters from the input buffer */

		if(!TransmitCommChar(ser_port,'#'))	/* write single character to serial port */
			err("TransmitCommChar failed with error -");

		FlushFileBuffers(ser_port);	/* write data to serial port, flush output buffer */

		Sleep(1000); 			/* wait for device respons */

		strcpy (readbuffer, "");

		if(!ReadFile(ser_port, &readbuffer, 100, &readsize,NULL)) /* read data */
			err("ReadFile failed with error -");

		readbuffer[readsize]=0;
		printf("%s", readbuffer);

	} while(!kbhit());		/* press any key to terminated */

	if(!CloseHandle(ser_port))	/* close serial port */
		err("CloseHandle failed with error -");

	while(kbhit()) getch();		/* clear keyboard buffer */

	return (0);
}

