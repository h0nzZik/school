#ifndef _TERM_H
#define _TERM_H

#include <stdio.h>

void Term_CursorUp(FILE *f, uint8_t n);
void Term_CursorDown(FILE *f, uint8_t n);
void Term_CursorLeft(FILE *f, uint8_t n);
void Term_CursorRight(FILE *f, uint8_t n);

#define TERM_COLOR_BLACK	30
#define TERM_COLOR_RED		31
#define TERM_COLOR_GREEN	32
#define TERM_COLOR_YELLOW	33
#define TERM_COLOR_BLUE		34
#define TERM_COLOR_MAGENTA	35
#define TERM_COLOR_CYAN		36
#define TERM_COLOR_WHITE	37

void Term_FgColorSet(FILE *f, uint8_t color);
void Term_FgColorClear(FILE *f);
void Term_Back(FILE *f, char c);

#endif
