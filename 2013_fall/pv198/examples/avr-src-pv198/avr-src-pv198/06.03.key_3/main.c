/*
 *                  KEYBOARD - Example
 *
 * Description : Simple Calculator
 *               
 *
 * Author: Zbynek Bures
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.1 2006/12/14 21:30:54 bubulak Exp $
 */ 

#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>


#define KEY_OUT		PORTD		/* port for keyboart */
#define KEY_IN		PIND		/* input pin from 74HC165 */
#define KEY_DIR		DDRD		/* pin direction bit */

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

#define KEY_A	0xa
#define KEY_B	0xb
#define KEY_C	0xc
#define KEY_D	0xd
#define KEY_AST	0xe
#define KEY_SHA	0xf

/* Function prototypes */
void Ser_init(uint16_t baud);
int Ser_putch(char c, FILE *stream);
int Ser_getch(FILE *stream);

uint8_t		Key_scan(void);
uint8_t		Key_read(void);
uint16_t 	Key_read_val(void);
uint8_t		Key_translate(uint8_t);

/*!
 * \brief Function translates key_raw to key 0-9 for numeric and control
 *	A is 0xa, B is 0xb, C is 0xc, D is 0xd, * is 0xe, # is 0xf
 * 
 * \param key_raw	Raw key (value from 0 - 15)
 * \return key_tran	Translated value
 *
 */
 uint8_t Key_translate(uint8_t raw_key)
 {
 	uint8_t	key_tran;
	switch(raw_key) {
		case 1:
		case 2:
		case 3:		key_tran = raw_key; break;
		case 5:
		case 6:
		case 7:		key_tran = raw_key - 1; break;
		case 9:
		case 10:
		case 11:	key_tran = raw_key - 2; break;
		case 14:	key_tran = 0; break;
		case 4:		key_tran = KEY_A; break;
		case 8:		key_tran = KEY_B; break;
		case 12:	key_tran = KEY_C; break; 
		case 16:	key_tran = KEY_D; break;
		case 13:	key_tran = KEY_AST; break;
		case 15:	key_tran = KEY_SHA; break;
		default:	key_tran = 0;
	}
	return (key_tran);
 }


/*!
 * \brief Function reads byte
 *	Function reads keys from keyboard. # is last key (like enter).
 *
 * \return value
 *
 */

 uint16_t	Key_read_val(void)
{
	uint16_t	val = 0;
	uint8_t		tmp;
	do {
		tmp = Key_read();
		tmp = Key_translate(tmp);
		/* Did you press A */
		if (tmp == KEY_A) {
			val =  0x100;
			break;
		}
		else if (tmp == KEY_B) {
			val = 0x200;
			break;
		}
		else if (tmp == KEY_C) {
			val = 0x400;
			break;
		}
		else if (tmp == KEY_D) {
			val = 0x800;
			break;
		}
		else if (tmp == KEY_AST) {
			val = 0x1000;
			break;
		}
		/* Did you press ENTER <=> # */
		if (tmp == KEY_SHA) {
			val |= 0x2000;
			break;
		}
		/* Did you press a numeric key */
		else if (tmp < 0xa) {
			val *= 10;
			printf("%c",0x30 | tmp);
			val += tmp;			
			if (val > 255)	{
				val = 0x4000 | 255;			/* this is error, number is bigger than 255 */
				break;
			}
		}
	} while (1);
	return (val);
}

/*!
 * \brief M A I N   Function
 * 
 */
int main(void)
{
	uint16_t val1, val2;
	uint16_t oper;

	FILE mystdout = FDEV_SETUP_STREAM(Ser_putch, Ser_getch, _FDEV_SETUP_RW);
	stdout = stdin = stderr =  &mystdout;
	Ser_init( UART_BAUD_SELECT(9600, F_CPU) );
	while (1) {
		printf ("Input first number 0 - 255 : ");
		val1 = Key_read_val();
		printf ("\nInput second number 0 - 255 : ");
		val2 = Key_read_val();
		printf ("\nInput a funcition : A = +, B = -, C = *, D = / \n");
		oper = Key_read_val();
		switch (oper) {
			case 0x100:	printf ("%d + %d = %d",(val1 & 0xff), (val2 & 0xff), ((val1 & 0xff) + (val2 & 0xff)));
						break;
			case 0x200:	printf ("%d - %d = %d",(val1 & 0xff), (val2 & 0xff), ((val1 & 0xff) - (val2 & 0xff)));
						break;
			case 0x400:	printf ("%d * %d = %d",(val1 & 0xff), (val2 & 0xff), ((val1 & 0xff) * (val2 & 0xff)));
						break;
			case 0x800:	printf ("%d / %d = %d",(val1 & 0xff), (val2 & 0xff), ((val1 & 0xff) / (val2 & 0xff)));
		}
		printf("\n\n");
	}	
	return(0);
}



/*!
 * \brief Inits ATmega128's serial port (UART0).
 *
 * \param baud  UART0 baudrate generator value.
 */
void Ser_init(uint16_t baud)
{
	/* set baud rate */
	UBRR0H = (uint8_t)(baud>>8);
	UBRR0L = (uint8_t) baud;
	/* enable receiver and transmitter */
	UCSR0B = (1<<RXEN)|(1<<TXEN);
	/* set frame format: 8 data, 2 stop bits */
	UCSR0C = (1<<USBS)|(3<<UCSZ0);
}

/*!
 * \brief Writes the character c to serial port (UART0).
 *
 * \param c  character to write.
 */
int Ser_putch(char c, FILE *stream)
{
	if (c == '\n')
		Ser_putch('\r', stream);
	/* wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE)) );
	/* put data into buffer and sends the data */
	UDR0 = c;
	return 0;
}

/*!
 * \brief Reads the character from serial port (UART0).
 *
 * \return received character.
 */
int Ser_getch(FILE *stream)
{
	/* wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* get and return received data from buffer */
	return UDR0;
}

/*!
 * \brief Function reads keyboard and return a stable key.
 *	Function waits untill button is stable. Function returns its value if button is released.
 *
 * \return key_read => Keycode of pushed button
 *
 */
uint8_t Key_read(void) {
uint8_t key_tmp = 0;
	while (!(key_tmp = Key_scan()));		/* Wait for pressing */
    do {
        _delay_ms(50);						/* Wait a while */
		if (key_tmp == Key_scan())	        /* Do we read the same value ? */
			break;
		else 
			key_tmp = Key_scan();
    } while (1);		
	
	while (Key_scan()); 					/*  Wait till button is released */
	return (key_tmp);
}


/*!
 * \brief Function for fast scanning of keyboar
 *
 * \return key_read => It returns scan code from 1 to 16 or 0xff as error (two or more keys were pushed)
 *
 */
uint8_t Key_scan(void) {
	uint8_t key_mask;
	uint8_t key_read;
	uint8_t i;

	KEY_DIR = 0xf0;									/* Set high pins as output */
	KEY_OUT = 0xff;									/* Set pins to high */

	key_mask = 0x10;								/* Prepare a mask for reading */
	key_read = 0;
	for (i = 0; i < 4; i++) {
		KEY_OUT = ~key_mask;						/* Activate one row */
		_delay_us(10);								/* Wait a while */
		if (~KEY_IN & 0xf) {						/* Read a pins */
			switch (~KEY_IN & 0xf) {				/* Translate a value from bin to inc */
				case 1:	key_read += 1; break;
				case 2: key_read += 2; break;
				case 4: key_read += 3; break;
				case 8: key_read += 4; break;
				default:	
						key_read = 0xff; break;		/* For error set value to 0xFF */

			}
			if (key_read != 0xff)
				key_read += i * 4;					/* If not error make a correction from rows */
		}
		key_mask <<= 1;
		if (key_read)								/* If we don't have a value, try to read next row */
			break;
	}
	return (key_read);
}
