/* 
 * File:   i2c.h
 * Author: vyvoj
 *
 * Created on 6. červen 2013, 13:52
 */

#ifndef I2C_H
#define	I2C_H

#include "main.h"

#ifdef	__cplusplus
extern "C" {
#endif


#define I2C_RTC_DEV	0xA3

#define I2C_SDA_PORT	RB1
#define I2C_SDA_TRIS	TRISB1
#define I2C_SCL_PORT	RB4
#define I2C_SCL_TRIS	TRISB4

#define RTC_CLKOUT	RB3


#define CHECK_ACKNOWLEDGE  1

uns8 i2c_write(uns8 dev, uns8 addr, uns8 data);





#define i2c_sda_clr()	do {asm("nop"); I2C_SDA_PORT = 0; I2C_SDA_TRIS = 0; } while(0)
#define i2c_sda_set()	do {asm("nop");  I2C_SDA_TRIS = 1; } while(0)
#define i2c_sda_get()	(I2C_SDA_PORT)

#define i2c_scl_clr()	do {asm("nop"); I2C_SCL_PORT = 0; I2C_SCL_TRIS = 0; } while(0)
#define i2c_scl_set()	do {asm("nop");  I2C_SCL_TRIS = 1; } while(0)

/* assume sda is set */
#define i2c_start()	do {i2c_scl_set(); i2c_sda_clr(); i2c_scl_clr();} while(0)
/* assume scl is clear */
#define i2c_stop()	do {i2c_sda_clr(); i2c_scl_set(); i2c_sda_set();} while(0)
uns8 i2c_read(uns8 dev, uns8 addr, uns8 *data);



#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

