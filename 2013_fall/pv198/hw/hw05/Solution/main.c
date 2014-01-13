/**
 * @file main.c
 * @brief Simple memory tester
 * @author Jan Tusil <410062@mail.muni.cz>
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: sram.c,v 1.1 2006/12/04 21:26:28 qceleda Exp $
 */

// C headers
#define __ASSERT_USE_STDERR 
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


// AVR headers
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>



/* calculate baudrate value */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/* global and extern variables */
const char ver_str_pgm[]  PROGMEM = "\r\nATmega128 onchip SRAM ver. 0.1.0 - "__DATE__", "__TIME__;



#define BUFF_SIZE 16

struct RingBuffer
{
	uint8_t buff[BUFF_SIZE];
	uint8_t start;
	uint8_t end;

	// When start == end,
	// determines whether buffer is empty or full
	bool empty;
};

/**
 * @brief Initializes RingBuffer as empty.
 */
void RingBuffer_init(struct RingBuffer *rb)
{
	rb->start = 0;
	rb->end = 0;
	rb->empty = true;
}

/**
 * @brief Determines whether ring buffer is empty
 * @return true if empty, false if not
 */
bool RingBuffer_empty(struct RingBuffer *self)
{
	return ((self->start == self->end) && self->empty);
}

/**
 * @brief Determines whether ring buffer is full
 * @return true if full, false otherwise
 */
bool RingBuffer_full(struct RingBuffer *self)
{
	return ((self->start == self->end) && !self->empty);
}

/**
 * @brief Adds byte to ring buffer.
 * @param data  ^^ byte
 * Does not block. Ring buffer shold not be full.
 */
void RingBuffer_add(struct RingBuffer *self, uint8_t data)
{
	assert(!RingBuffer_full(self));

	self->buff[self->start++] = data;
	self->start %= BUFF_SIZE;
	self->empty = false;
}

/**
 * @brief Gets a byte from ring buffer
 * @return the oldest byte
 * Does not block. Ring buffer shold not be empty.
 */
uint8_t RingBuffer_get(struct RingBuffer *self)
{
	assert(!RingBuffer_empty(self));

	uint8_t tmp;
	tmp = self->buff[self->end++];
	self->end %= BUFF_SIZE;

	if (self->start == self->end)
		self->empty = true;


	return tmp;
}



/*!
 * \brief Inits ATmega128's serial port (UART0).
 *
 * \param baud  UART0 baudrate generator value.
 */
void ser_init(uint16_t baud, bool int_rx, bool int_tx)
{
	/* set baud rate */
	UBRR0H = (uint8_t)(baud>>8);
	UBRR0L = (uint8_t) baud;

	/* enable receiver and transmitter */
	uint8_t tmp = _BV(RXEN0)|_BV(TXEN0);
	if (int_rx)
	{
		tmp |= _BV(RXCIE0);
	}

	if (int_tx)
	{
		tmp |= _BV(TXCIE0);
	}

	UCSR0B = tmp;

	/* set frame format: 8 data, 2 stop bits */
	UCSR0C = _BV(USBS0)|_BV(UCSZ01)|_BV(UCSZ00);
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

	struct RingBuffer *rb;
	rb = (struct RingBuffer *) fdev_get_udata(stream);


	while (!(UCSR0A & _BV(UDRE0)))
	{

		;
	}
	UDR0 = c;
	return 0;

	// Wait until there is space
	do {
		;
	} while(RingBuffer_full(rb));

	cli();

	// Ring buffer is empty and serial is free
	if (RingBuffer_empty(rb) && (UCSR0A & _BV(UDRE0)))
	{
		UDR0 = c;
	} else {
		RingBuffer_add(rb, c);
	}
	sei();

	return 0;
}

/**
 * @brief Reads the character from serial port buffer (UART0).
 * Blocks until there is something in buffer
 * @return received character.
 */
int ser_getch(FILE *stream)
{
	struct RingBuffer *rb;
	rb = (struct RingBuffer *) fdev_get_udata(stream);

	// Wait for character
	do
	{
		;
	} while (RingBuffer_empty(rb));

	
	cli();
	int c = RingBuffer_get(rb);
	sei();

	return c;
}




/**
 * @brief Application structure
 */
struct RingBufferExample
{
	// stdout points here
	FILE out;
	struct RingBuffer buf_out;

	// stdin points here
	FILE in;
	struct RingBuffer buf_in;
};



/**
 * @brief Initializes this example application.
 * After this function is called,
 * stdin, stdout and stderr are redirected to serial 0
 * and they are buffered.
 */
void RingBufferExample_init(struct RingBufferExample *self)
{
	// init ring buffers
	RingBuffer_init(&self->buf_in);
	RingBuffer_init(&self->buf_out);

	// RS232
	ser_init( UART_BAUD_SELECT(9600, F_CPU), true, true);


	// Standard input
	stdin = &self->in;
	fdev_setup_stream(stdin, NULL, ser_getch, _FDEV_SETUP_READ);
	fdev_set_udata(stdin, (void *)&self->buf_in);

	// Standard output
	stdout = stderr = &self->out;
	fdev_setup_stream(stdout, ser_putch, NULL, _FDEV_SETUP_WRITE);
	fdev_set_udata(stdout, (void *)&self->buf_out);
	

	// pins
	DDRD = 0xff;		/* use all pins on Port D for output */
	PORTD = 0x03;
	DDRB = 0x00;		/* use all pins on Port B for input */

	// Enable interrupts
	sei();

}

/**
 * @brief Tests output buffering.
 * This is only testing function and it breaks API,
 * so please, do not remove final getchar().
 */
void RingBufferExample_outputTest(struct RingBufferExample *self)
{
	puts_P(PSTR("\nPress any key. When you see some characters, press it again"));
	getchar();

	// quickly fill the buffer
	int i;
	for (i=0; i<100; i++)
	{
		if (!RingBuffer_full(&self->buf_out))
			RingBuffer_add(&self->buf_out, 'a' + (i%('z'-'a')));

	}
	// And then start transfer
	UDR0 = '-';
	getchar();
	
}
/**
 * @brief Tests input buffering
 */
void RingBufferExample_inputTest(struct RingBufferExample *self)
{
	puts_P(PSTR("\nType something (about 10 character) and then create event on portB"));
	puts_P(PSTR("But I thing we will catch something without help ;-)"));
	uint8_t old = PINB;

	do {
		;
	} while(old == PINB);

	puts_P(PSTR("\nYou typed:"));

}

struct RingBufferExample app;

// Interrupt handler - Recieved a byte over RS232
ISR(USART0_RX_vect)
{
	if (!RingBuffer_full(&app.buf_in))
	{
		RingBuffer_add(&app.buf_in, UDR0);
	} else {
		// TODO: Some logging that data are lost
		PORTD |= 0x80;
	}
}

// Interrupt handler - Transmit complete
ISR(USART0_TX_vect)
{
	// So we can send next character
	if (!RingBuffer_empty(&app.buf_out))
	{
		UDR0 = RingBuffer_get(&app.buf_out);
	}
}

int main( void )
{
	RingBufferExample_init(&app);

	// Show welcome string
	puts_P(ver_str_pgm);

	RingBufferExample_outputTest(&app);
	RingBufferExample_inputTest(&app);



	while(1)
	{
		putchar(getchar());
	}
}



