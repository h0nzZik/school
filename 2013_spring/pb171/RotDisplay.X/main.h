/* 
 * File:   main.h
 * Author: vyvoj
 *
 * Created on 6. červen 2013, 13:52
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#define LED_BIT		0x02
#define DATA_BIT	0x04

/* FIXME: Chyba v dokumentaci */
#define SHCLK_BIT	0x10    /* shift clock */
#define STCLK_BIT	0x08    /* store clock */

/* buttons */
#define BUTTON_LEFT	RB5
#define BUTTON_RIGHT	RA6
#define TRIS_BUTTON_RIGHT	TRISAbits.TRISA6

#define RTC_CLOCK_SRC	RB3
#define RTC_INT		RB0	// BTW interrupt is there

typedef unsigned char uns8;


#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

