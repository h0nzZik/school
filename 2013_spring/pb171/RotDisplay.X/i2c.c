#include <pic.h>

#include "i2c.h"




typedef unsigned char uns8;

void i2c_write_bit(uns8 x)
{
	x &= 0x01;
	if (x)
		i2c_sda_set();
	else
		i2c_sda_clr();
	i2c_scl_set();
//	asm("nop");
	i2c_scl_clr();
}

uns8 i2c_read_bit()
{
	uns8 b;

	i2c_sda_set();
	i2c_scl_set();
	b = i2c_sda_get();
	i2c_scl_clr();

	if (b)
		return 1;
	else
		return 0;
}

uns8 i2c_check_ack()
{
	uns8 tmp;
	i2c_sda_set();
	i2c_scl_set();
	tmp = i2c_sda_get();
	i2c_scl_clr();

	if (CHECK_ACKNOWLEDGE && tmp != 0) {
		i2c_stop();
		return 0xFF;
	}
	return 0;
}
/**
 *
 * @param dev	device address (0x63 for write)
 * @param addr	address in device's memory (0x00 to 0x0F for RTC)
 * @param data	data to write
 * @return	0x00 on success, 0xFF on error
 */

uns8 i2c_write_byte(uns8 byte)
{
//	i2c_start();

	uns8 i;
	for (i=0; i<8; i++) {
		/* send one bit*/
		i2c_write_bit((byte >> (7-i)) & 0x01);
	}
	if (i2c_check_ack())
		return 0xFF;

	return 0;
}

uns8 i2c_read_byte(uns8 *byte)
{


}

uns8 i2c_write(uns8 dev, uns8 addr, uns8 data)
{
/*
	if (I2C_SDA_PORT == 0)
		return 0xFF;
*/
	/* set LSB to zero */
	dev &= 0xFE;
	i2c_start();

	if (i2c_write_byte(dev))
		return 0xFF;
	if (i2c_write_byte(addr))
		return 0xFF;
	if (i2c_write_byte(data))
		return 0xFF;
	i2c_stop();
	return 0;
}

/* TODO:
 */
uns8 i2c_read(uns8 dev, uns8 addr, uns8 *data)
{
//	dev |= 0x01;	/* read addr */
	i2c_start();
	if (i2c_write_byte(dev & 0xFE))	/* device in write mode*/
		return 0xFF;
	if (i2c_write_byte(addr))
		return 0xFF;
	i2c_start();
	if (i2c_write_byte(dev | 0x01))	/* device in read mode */
		return 0xFF;
}
