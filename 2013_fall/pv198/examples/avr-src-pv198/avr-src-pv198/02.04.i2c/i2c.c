#include <avr/io.h>
#include "i2c.h"


/*!
 * \brief Wait function for I2C
 */

void I2C_Wait() 
{
        uint8_t cnt = 10;
        while (--cnt);
}



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
		I2C_Write_B(0xa0);			/* <Y> Write chip address and operation WRITE */
                I2C_Write_B(adr);				/* Write low address */
        }
	else
		I2C_Write_B(0xa1);			/* <N> Write chip address and operation READ */

	
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
	SET_I2C_OUT;					/* Set pins direction to output */

	P_SCL_ON;					/* SDA must go down during SCL is high */
	I2C_Wait();	/* wait */ 	
	P_SDA_ON;
	I2C_Wait();	/* wait */ 	
	P_SDA_OFF;
	I2C_Wait();	/* wait */ 
	P_SCL_OFF;
}

/*!
 * \brief Function generates STOP condition on I2C
 *
 */
void I2C_Stop(void)
{
	SET_I2C_OUT;					/* Set pins direction to output */

	P_SDA_OFF;					/* SDA must go up during SCL is high */
	I2C_Wait();	/* wait */ 
	P_SCL_OFF;
	I2C_Wait();	/* wait */ 
	P_SCL_ON;
	I2C_Wait();	/* wait */ 
	P_SDA_ON;
}

/*!
 * \brief Function writes one byte to I2C and it generates ACK pulse WITHOUT ACK checking
 *
 * \param dta	Data for writing
 */
void I2C_Write_B (uint8_t dta)
{
	uint8_t	cnt = 8;
	do							/* Send one byte to I2C */
	{
		(dta & 0x80)? P_SDA_ON : P_SDA_OFF;		/* Set MSB bit to SDA */
		I2C_Wait();	/* wait */ 
		P_SCL_ON;
		I2C_Wait();	/* wait */ 
		P_SCL_OFF;
		dta <<= 1;
	} while (--cnt);
	/*  Generation of ACK pulse ---- <br> WITHOUT ACK checking </b> */
	SET_I2C_IN;						/* Set SDA to input */

	P_SCL_ON;
	I2C_Wait();	/* wait */ 
	P_SCL_OFF;
	I2C_Wait();	/* wait */ 

	SET_I2C_OUT;						/* Set SDA to output */
}

/*!
 * \brief Function reads one byte from I2C and generates an ACK condition
 *
 * \param 	ack	Type of ACK, 0 .. NoACK, 1 .. Ack
 * \return	dta	Read value
 */
uint8_t I2C_Read_B (uint8_t ack)
{
	uint8_t dta = 0;
	uint8_t cnt = 8;

	SET_I2C_IN;						/* Set SDA to input */
        I2C_Wait();
	do
	{
		P_SCL_ON;					/* Generate clock pulse */
		dta <<= 1;
                I2C_Wait();
		dta |= bit_is_set(I2C_IN_PORT,I2C_SDA) ? 1 : 0;	/* Read one bit from I2C */
		P_SCL_OFF;
                I2C_Wait();
	} while (--cnt);

	SET_I2C_OUT;	
	/* ** Do we have to generate ACK ? */
	if (ack)
		I2C_Ack_Out();					/* <Y> Generate ACK */
	else
		I2C_NoAck_Out();				/* <N> Generate NoACK */
	return (dta);						/* Return read value */
}

/*!
 * \brief Function tests ACK pulse generated on I2C
 *
 * \return 0	Err, NoACK from I2C device
 * \return 1	OK,  ACK from I2C device
 */
uint8_t I2C_Ack_In(void)
{
	uint8_t stat;
	SET_I2C_IN;                                             /* Set SDA as input */

	P_SCL_ON;                                               /* Generation CLK pulse */
	I2C_Wait();	        /* wait */ 
	stat = bit_is_set(I2C_IN_PORT,I2C_SDA) ? 0 : 1;
	P_SCL_OFF;
	SET_I2C_OUT;                                            /* Set SDA as output */
	return (stat);
}

/*!
 * \brief Function I2C_NoAck_Out generates NoACK pulse
 */
void I2C_NoAck_Out(void)
{
	P_SDA_ON;
	I2C_Wait();	        /* wait */ 
	P_SCL_ON;
	I2C_Wait();	        /* wait */ 
	P_SCL_OFF;

}

/*!
 * \brief Function I2C_Ack_Out generates ACK pulse
 */
void I2C_Ack_Out(void)
{
	P_SDA_OFF;
	I2C_Wait();	        /* wait */ 
	P_SCL_ON;
	I2C_Wait();	        /* wait */ 
	P_SCL_OFF;

}

