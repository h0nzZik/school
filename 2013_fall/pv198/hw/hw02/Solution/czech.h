#ifndef CZECH_H
#define CZECH_H

#include <stdint.h>
#include "lcd.h"

#define N_LCD_CHARS 6

void czech_init(void);
void lcd_cz_putc(char c, display_t *dis);

#endif
