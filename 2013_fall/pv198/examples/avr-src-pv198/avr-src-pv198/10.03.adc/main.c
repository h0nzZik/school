/*
 *                  A / D    C o n v e r t e r
 *
 * Description : Simple demo how to use a/d converter single ended and 
 * 					differential connection on ATMEGA 128
 *               
 *
 * Author: Zbynek Bures 
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.1 2007/01/05 00:45:20 bubulak Exp $
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* function prototypes */
void ser_init(uint16_t baud);
int ser_putch(char c, FILE *stream);
int ser_getch(FILE *stream);
void adc_init(void);
int main( void )
{
uint16_t adc_val0, adc_val1;
uint8_t i;

	FILE mystdout = FDEV_SETUP_STREAM(ser_putch, ser_getch, _FDEV_SETUP_RW);

	stdout = stdin = stderr =  &mystdout;
	/* open UART0 - 9600,N,8,1 */
	ser_init( UART_BAUD_SELECT(9600, F_CPU) );

	/* display program version */
	printf_P(PSTR("\rATmega128 A/D single ended and differential example\n"
				"Build on %s, %s with AVR-GCC %d.%d\n\n"),
				__DATE__, __TIME__, __GNUC__, __GNUC_MINOR__);		

	adc_init();
	/* Configure a2d port (PORTF) as input so we can receive analog signals */
	DDRF = 0x00; 
	/* make sure pull-up resistors are turned off (else we’ll just read 0xCFF) */
	PORTF = 0x00; 

	while(1)
	{	
		ADMUX &= 0b11100000; 			/* selects conversion on PF0 */		
		ADCSRA |= _BV(ADSC); 			/* start a conversion by writing a one to the ADSC bit (bit 6) */
		while(ADCSRA & 0b01000000); 	/* wait for conversion to complete (bit 6 will change to 0) */
		adc_val0 = ((ADCL) | ((ADCH)<<8)); /* 10-bit conversion for channel 0 (PF0) */
		ADMUX |= 0x10;			 			/* selects conversion on PF1 */		
		_delay_ms(1);
		ADCSRA |= _BV(ADSC); 			/* start a conversion by writing a one to the ADSC bit (bit 6) */
		while(ADCSRA & 0b01000000); 	/* wait for conversion to complete (bit 6 will change to 0) */
		adc_val1 = ((ADCL) | ((ADCH)<<8)); /* 10-bit conversion for channel 1 (PF1) */
		/* Ch0 - single ended ADC0, Ch1 - differential ADC0 possitive, ADC1 negative input */
		printf("Ch0 = %d, | Ch1 = %d \n",adc_val0, adc_val1);
		for (i = 0; i < 10; i++)
			_delay_ms(10);
	}
	return (0);
}
/*!
 * \brief Inits ATmega128's A/D converter
 *
 *	Sets basic configuration, channel 0, ADC clock to 115.200 kHz, single sample, right adjusted data,
 *	maximum voltage AVCC.
 *
 */
void adc_init(void)
{
	ADCSRA |= _BV(ADEN); 		/* enables ADC by setting bit 7 (ADEN) in the ADCSRA */
	ADCSRA &= ~_BV(ADFR); 		/* single sample conversion by clearing bit 5 (ADFR) in the ADCSRA */
	ADCSRA = ((ADCSRA & 0b11111000) | 0b00000111);	/* selects div by 128 clock prescaler */
	ADMUX = ((ADMUX & 0b00111111) | 0b01000000); 	/* selects AVCC as Vref */
	ADMUX &= ~_BV(ADLAR); 		/* selects right adjust of ADC result */
	ADMUX &= 0b11100000; 		/* selects single-ended conversion on PF0 */
	ADMUX |= 0x10;
}






/*!
 * \brief Inits ATmega128's serial port (UART0).
 *
 * \param baud  UART0 baudrate generator value.
 */
void ser_init(uint16_t baud)
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
int ser_putch(char c, FILE *stream)
{
	if (c == '\n')
		ser_putch('\r', stream);

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
int ser_getch(FILE *stream)
{
	/* wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );

	/* get and return received data from buffer */
	return UDR0;
}

