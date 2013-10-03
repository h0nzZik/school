/*
 *                  I2C RTC- e x a m p l e
 *
 * Description : Basic functions used for 8-bit serial communication I2C
 *               between ATmega128 and RTC PCF8583 with using INT pin
 *
 * Author: Zbynek Bures
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.1 2006/11/24 00:13:48 cvsd Exp $
 */


#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"
#include "shift.h"

/*! \brief Time structure 
 */
// --- STRUKTURA RTC --- BEGIN
typedef struct {
/*! \brief Control register */  
uint8_t cntrl;
/*! \brief ._x seconds */  
uint8_t s_1_100:4;              
/*! \brief .x seconds */               
uint8_t s_1_10:4;               
/*! \brief x. seconds */
uint8_t s_1:4;                  
/*! \brief x_.seconds */
uint8_t s_10:4;                 
/*! \brief x minutes */
uint8_t m_1:4;
/*! \brief x_ minutes */
uint8_t m_10:4;                 
/*! \brief x hours */
uint8_t h_1:4;                  
/*! \brief x_ hours */  
uint8_t h_10:4;                 
/*! \brief x days */
uint8_t d_1:4;                  
/*! \brief x_ days */
uint8_t d_10:4;                 
/*! \brief Status register */
uint8_t stat;                   
/*! \brief Old value of seconds counter (temporary) */
uint8_t s_old;                  
} _RTC;
// --- STRUKTURA RTC--- END

/* Function prototypes */
void Set_clock(void);        
void Get_clock(void);
void Hw_init(void);
void DevBoardShiftLedOut(uint8_t);

_RTC    RTC;                            /* Global declaration of RTC structure */

/*!
 *      M       A       I       N
 *
 */
int main(void) {
uint8_t *ptr = &RTC.cntrl;              /* Definition of ptr */

        Hw_init();                      /* Set HW configuration */
        *ptr++ = 0x80;                     /* Set control 32.768kHz, */
	*ptr++ = 0;                     /* Set hundredth of sec */   
	*ptr++ = 0x56;                  /* Set 56 secounds */
	*ptr++ = 0x34;                  /* Set 34 minutes */
	*ptr++ = 0x12;                  /* Set 12 hours */

	Set_clock();                                                    /* Set RTC registers */
        RTC.cntrl = 0;
        Set_clock();
        while (1) {
                if (GET_INT_STAT) {
                        Get_clock();                                            /* Read new values of RTC */
                        /* Is there some time difference ? */
        		if (RTC.s_old != ((RTC.s_10 << 4) | RTC.s_1)) { 
                                RTC.s_old = (RTC.s_10 << 4) | RTC.s_1;          /* <Y> Store new value */
		                DevBoardShiftLedOut(~RTC.s_old);		/* send value to bargraph */
                        }
                }
        }
}

/*!
 * \brief Function Hw_init sets basic HW configuration
 */
void Hw_init(void)
 {
        I2C_DDR |= _BV(I2C_SCL) | _BV(I2C_SDA); /* serial clock -> output pin, serial data -> output pin */
        P_SDA_ON;
        P_SCL_ON;
	SHIFT_DDR |= _BV(SHIFT_LED_SET_DDR);	/* parallel write -> output pin */
	SHIFT_DDR |= _BV(SHIFT_CLK_DDR);	/* serial clock -> output pin */
	SHIFT_DDR |= _BV(SHIFT_OUT_DDR);	/* serial data stream -> output pin */        
 }

/*!
 * \brief       Function Set_clock sets basic settings
 *
 */
void Set_clock(void) 
{
	uint8_t i;
	uint8_t *ptr = &RTC.cntrl;                      /* Set temporary pointer */
	I2C_Stop();                                     /* Generate stop */
	I2C_Set_Address(0,0);                           /* Set low address and func, write */
	for (i = 0; i < 6; i++) {
		I2C_Write_B(*ptr);                      /* Write 6 byte to RTC */
		ptr++;
	}
	I2C_Stop();                                     /* Generate stop */
}

/*!
 * \brief       Function Get_clock reads data from RTC
 *
 */
void Get_clock(void) 
{
	I2C_Stop();                                     /* Generate stop condition */
	I2C_Set_Address(0,0);                           /* Set RTC address to 0, func. write */
	I2C_Start();
	I2C_Set_Address(0,1);	                        /* Set RTC address to 0, func. read */ 
	I2C_Read_Block(6, &RTC.cntrl);                  /* Read first 6 byte from RTC and store data to RTC structure */
	I2C_Stop();                                     /* Generate stop condition */
}


/*!
 * \brief Send data to 8x shift LED shift interface.
 *
 * \param val   8 bits value for transmission
 *
 */
void DevBoardShiftLedOut( uint8_t val )
{
	uint8_t i=8;

	SHIFT_DDR |= _BV(SHIFT_LED_SET_DDR);	/* parallel write -> output pin */
	SHIFT_DDR |= _BV(SHIFT_CLK_DDR);	/* serial clock -> output pin */
	SHIFT_DDR |= _BV(SHIFT_OUT_DDR);	/* serial data stream -> output pin */

	SHIFT_PORT &= ~(_BV(SHIFT_LED_SET));	/* define start values */
	SHIFT_PORT &= ~(_BV(SHIFT_CLK));

	while( i-- )
	{	 /* send data to 74595, MSB (most significant bit) first */
		if( val & (1 << i) ) SHIFT_PORT |= _BV(SHIFT_OUT);
		else SHIFT_PORT &= ~(_BV(SHIFT_OUT));

		SHIFT_PORT |= _BV(SHIFT_CLK);   /* 0 -> 1 data valid */

		asm volatile("nop\n\t");	/* wait */
		SHIFT_PORT &= ~(_BV(SHIFT_CLK));
	}

	asm volatile("nop\n\t");		/* wait */
	SHIFT_PORT |= _BV(SHIFT_LED_SET);	/* 0 -> 1 parallel write */
}
