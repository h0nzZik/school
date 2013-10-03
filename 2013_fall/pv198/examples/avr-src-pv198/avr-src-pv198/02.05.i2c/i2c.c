#include <avr/io.h>
#include "i2c.h"

#define C_MT_SLA_ACK    0x18
#define C_MT_DATA_ACK   0x28
#define C_START         0x8 
#define C_START_R       0x10 

/*!
 * \brief Wait function for I2C
 */



/*!
 * \brief Function sets high part of address
 *
 * \param adr	Low part of address
 * \param func	Type of operation (0 - write, 1 - read)
 *
 */
 void I2C_Set_Address(uint8_t adr, uint8_t func)

{
	I2C_Start();					/* Generate start condition */
	/* ** Do you want to write ? */
	if (!func) {
                TWDR = 0xa0;                            /* Address RTC + write signature */ 
                TWCR = (1<<TWINT) | (1<<TWEN);          /* <Y> Write chip address and operation WRITE */
                while (!(TWCR & (1<<TWINT)));
                if ((TWSR & 0xF8) != C_MT_SLA_ACK)
                        asm volatile("nop\n\tnop\n\tnop\n\tnop\n\t");	/* wait */         

                TWDR = adr; 
                TWCR = (1<<TWINT) | (1<<TWEN);          /* Write low address */
                while (!(TWCR & (1<<TWINT)));
                if ((TWSR & 0xF8) != C_MT_DATA_ACK)
                        asm volatile("nop\n\tnop\n\tnop\n\tnop\n\t");	/* wait */         

        }
	else {
		TWDR = 0xa1;
                TWCR = (1<<TWINT) | (1<<TWEN);          /* <N> Write chip address and operation READ */
                while (!(TWCR & (1<<TWINT)));
                if ((TWSR & 0xF8) != C_MT_SLA_ACK)
                        asm volatile("nop\n\tnop\n\tnop\n\tnop\n\t");	/* wait */
	}
}

/*!
 * \brief Function reads block of <I>i</I> data from I2C and store them to <B>*p_dta</B>
 *
 * \param	i		Number of byte for reading 
 * \param	*p_dta		Pointer to the memory for writting 
 */
void I2C_Read_Block(uint8_t i, uint8_t *p_dta)
{
	uint8_t j;
	for (j = 0; j < i; j++) {			/* Read (i-1) byte with ACK and read last with NoACK condition */
		if ((j + 1) != i)
			*p_dta = I2C_Read_B(1);		/* Read one byte from I2C and generate ACK condition */
		else
			*p_dta = I2C_Read_B(0);		/* Read last byte from I2C and generate NoAck condition */
		p_dta++;
	}
	I2C_Stop();					/* Generate STOP condition */
}

/*!
 * \brief Function generates START condition on I2C
 *
 */
void I2C_Start(void)
{
        TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);         /* Send START condition */
        while (!(TWCR & (1<<TWINT)));                   /* Wait for TWINT */
        if (((TWSR & 0xF8) != C_START) && ((TWSR & 0xF8) != C_START_R))
                asm volatile("nop\n\tnop\n\tnop\n\tnop\n\t");	/* error */         
}

/*!
 * \brief Function generates STOP condition on I2C
 *
 */
void I2C_Stop(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);         /* Transmit STOP condition */   
}

/*!
 * \brief Function writes one byte to I2C and it generates ACK pulse WITHOUT ACK checking
 *
 * \param dta	Data for writing
 */
void I2C_Write_B (uint8_t dta)
{
        TWDR = dta; 
        TWCR = (1<<TWINT) | (1<<TWEN);          /* Write low address */
        while (!(TWCR & (1<<TWINT)));
        if ((TWSR & 0xF8) != C_MT_DATA_ACK)
                asm volatile("nop\n\tnop\n\tnop\n\tnop\n\t");	/* error */   
}

/*!
 * \brief Function reads one byte from I2C and generates an ACK condition
 *
 * \param 	ack	Type of ACK, 0 .. NoACK, 1 .. Ack
 * \return	dta	Read value
 */
uint8_t I2C_Read_B (uint8_t ack)
{
        uint8_t dta;
	if (ack)
                TWCR |= (1<<TWEA);              /* Read next byte */

	else
                TWCR &= ~(1<<TWEA);             /* Stop with reading */

//        TWCR = (1<<TWINT) | (1<<TWEN);        /* Write low address */
        while (!(TWCR & (1<<TWINT)));
        dta = TWDR;
	return (dta);                           /* Return read value */
}





