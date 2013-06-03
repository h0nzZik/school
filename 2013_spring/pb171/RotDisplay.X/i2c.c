#include <pic.h>

#define I2C_RTC_DEV	0x63


#define I2C_SDA_PORT	RB1
#define I2C_SDA_TRIS	TRISB1
#define I2C_SCL_PORT	RB4
#define I2C_SCL_TRIS	TRISB4

#define RTC_CLKOUT	RB3

#define i2c_sda_clr()	do {I2C_SDA_PORT = 0; I2C_SDA_TRIS = 0; } while(0)
#define i2c_sda_set()	do {/*I2C_SDA_PORT = 1;*/ I2C_SDA_TRIS = 1; } while(0)
#define i2c_sda_get()	(I2C_SDA_PORT)

#define i2c_scl_clr()	do {I2C_SCL_PORT = 0; I2C_SCL_TRIS = 0; } while(0)
#define i2c_scl_set()	do {/*I2C_SCL_PORT = 1;*/ I2C_SCL_TRIS = 1; } while(0)

#define i2c_start()	do {i2c_sda_clr(); i2c_scl_clr();} while(0)
#define i2c_stop()	do {i2c_scl_set(); i2c_sda_set();} while(0)


typedef unsigned char uns8;

void i2c_write_bit(uns8 x)
{
	x &= 0x01;
	if (x)
		i2c_sda_set();
	else
		i2c_sda_clr();
	i2c_scl_set();
	asm("nop");
	i2c_scl_clr();
}

uns8 i2c_check_ack()
{
	uns8 tmp;
	i2c_sda_set();
	i2c_scl_set();
	tmp = i2c_sda_get();
	i2c_scl_clr();

	if (tmp != 0) {
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
	/* set LSB to zero */
	byte &= 0xFE;

	i2c_start();

	uns8 i;
	for (i=0; i<8; i++) {
		/* send one bit*/
		i2c_write_bit((byte >> (7-i)) & 0x01);
	}
	if (i2c_check_ack())
		return 0xFF;

	return 0;
}

uns8 i2c_write(uns8 dev, uns8 addr, uns8 data)
{
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

