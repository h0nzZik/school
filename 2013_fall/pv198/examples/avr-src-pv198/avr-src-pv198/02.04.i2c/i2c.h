#ifndef _I2C_H
#define _I2C_H

#include <avr/io.h> 

/* Definitions for I2C */
#define I2C_OUT_PORT            PORTD		/* port for I2C */
#define I2C_IN_PORT             PIND		/* input pin from I2C (SDA) */
#define I2C_DDR                 DDRD		/* pin direction bit */

#define I2C_SCL                 PORTD0		/* I2C SCL (serial clock) pin */
#define I2C_SDA                 PORTD1		/* I2C SDA (serial data) pin */
#define I2C_INT                 PORTD2		/* I2C INT (interrupt) pin */

/*! \brief MACRO => set SDA to high */
#define P_SDA_ON		(I2C_OUT_PORT |= _BV(I2C_SDA))
/*! \brief MACRO => set SDA to low */
#define P_SDA_OFF		(I2C_OUT_PORT &= ~_BV(I2C_SDA))
/*! \brief MACRO => set SCL to high */
#define P_SCL_ON		(I2C_OUT_PORT |= _BV(I2C_SCL))
/*! \brief MACRO => set SDA to low */
#define P_SCL_OFF		(I2C_OUT_PORT &= ~_BV(I2C_SCL))

/*! \brief MACRO => set SDA and SCL as outputs */
#define SET_I2C_OUT		(I2C_DDR |= (_BV(I2C_SDA) | _BV(I2C_SCL)))
/*! \brief MACRO => set SDA as input and SCL as output */
#define SET_I2C_IN		(I2C_DDR |= _BV(I2C_SCL), I2C_DDR &= ~_BV(I2C_SDA))

/*! \brief MACRO => read PCF8583 INT pin */
#define GET_INT_STAT            (I2C_IN_PORT & _BV(I2C_INT))

void I2C_Set_Address(uint8_t, uint8_t);		/* Set I2C low address and type of operation */
void I2C_Start(void);				/* Generate start condition */
void I2C_Stop(void);				/* Generate stop condition */
void I2C_Write_B (uint8_t);			/* Write one byte to temporary register */
uint8_t I2C_Read_B (uint8_t);			/* Read one byte from I2C */
uint8_t I2C_Ack_In(void);			/* Generate ACK pulse for slave present testing */
void I2C_NoAck_Out(void);			/* Generate NON ACK for slave to stop next reading */
void I2C_Ack_Out(void);				/* Generate ACK for slave for next reading */ 
void I2C_Write_Block_W(uint16_t *);		/* Write block of word to I2C */
void I2C_Read_Block(uint8_t , uint8_t *);	/* Read block from I2C */
void I2C_Wait(void);                            /* Wait for I2C */
#endif
