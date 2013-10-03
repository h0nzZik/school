/*
 *                  UDP - thermometer - e x a m p l e
 *
 * Description : Simple demo how to send temperature strings over net via UDP.
 *
 * Note  : Copy source code to <ETHERNUT>/nutapp/udptherm directory. Run 'make'
 *         to build program and 'make burn' to program ATmega128 chip.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Ethernut - 4.2.1
 *                 Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: udptherm.c,v 1.1 2007/01/04 22:25:40 qceleda Exp $
 */
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include <dev/board.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <pro/dhcp.h>

#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/confnet.h>
#include <sys/socket.h>

/*
 * Client IP configuration.
 */
#define MY_MAC          {0x00,0x06,0x98,0x20,0x00,0x00}
#define MY_IP           "192.168.50.200"	/* 192.168.50.194 - 192.168.50.220*/
#define MY_GATE         "192.168.50.1"		/* 192.168.50.1 */
#define MY_MASK         "255.255.255.0"

/*
 * Server IP configuration.
 */
#define SERVER_IP	"192.168.50.12"		/* 192.168.50.12 - 192.168.50.21 */
#define SERVER_PORT	2000

/*
 * Dec 	Hex  Unicode  Char  Description - ASCII IBM PC
 * 248 	F8   U+00B0   °     degree sign
 */
#define DEGREE_SIGN 	0xB0	/* Linux - 0xB0, Windows - 0xF8 */

/*
 * Main application routine.
 *
 * Nut/OS automatically calls this entry after initialization.
 */
int main(void)
{
	u_char	my_mac[] = MY_MAC;	/* device MAC address */
	u_char	t = 0,			/* dummy temperature value */
		buf[64];		/* dummy text buffer */
	u_long	baud = 9600;		/* default baud rate 9600,N,8,1 */
	FILE	*uart;			/* UART 0 file */
	UDPSOCKET *sock;		/* UDP socket */

				/* register UART 0 */
	NutRegisterDevice(&DEV_UART, 0, 0);

				/* open UART 0 */
	uart = fopen(DEV_UART_NAME, "r+b");
				/* assign stdout to the UART 0 device */
	freopen(DEV_UART_NAME, "w", stdout);
				/* set UART 0 speed */
	_ioctl(_fileno(uart), UART_SETSPEED, &baud);

	fprintf(uart,"UDP Thermometer demo example\n");
	fflush(uart);

				/* register 8019as Realtek controller */
	NutRegisterDevice(&DEV_ETHER, 0x8300, 5);

				/* get IP configuration */
	if (NutDhcpIfConfig("eth0", my_mac, 2000)) {
				/* no DHCP server available, use hard coded values */
		NutNetIfConfig("eth0", my_mac, inet_addr(MY_IP), inet_addr(MY_MASK));
		NutIpRouteAdd(0, 0, inet_addr(MY_GATE), &DEV_ETHER);
		confnet.cdn_gateway = inet_addr(MY_GATE);
	}

				/* display IP configuration */
	printf("\nMAC  : %02X:%02X:%02X:%02X:%02X:%02X\n",
		my_mac[0], my_mac[1], my_mac[2], my_mac[3], my_mac[4], my_mac[5]);
	printf("IP   : %s\n", inet_ntoa(confnet.cdn_ip_addr));
	printf("MASK : %s\n", inet_ntoa(confnet.cdn_ip_mask));
	printf("GW   : %s\n\n", inet_ntoa(confnet.cdn_gateway));

				/* create client socket for the UDP traffic */
	if ((sock = NutUdpCreateSocket(0)) == 0) {
		printf("Can't create UDP socket.");
		return (-1);
	}

	for (;;) {
		sprintf(buf,"Temperature = %3d.%1d %cC\n", t/10, t%10, DEGREE_SIGN);
		t++;			/* increment temperature */

		printf("%s",buf);	/* send temperature to UART 0 */
		fflush(uart);
					/* send temperature to server */
		if (NutUdpSendTo(sock, inet_addr(SERVER_IP), SERVER_PORT, buf, strlen(buf)) < 0)
			break;

		NutSleep(1000);		/* sleep 1000 ms */
	}

	NutUdpDestroySocket(sock);	/* destroy UDP socket*/
	printf("Program terminated.");
	return (-1);
}

