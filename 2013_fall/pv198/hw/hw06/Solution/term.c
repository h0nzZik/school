#include <stdio.h>
#include <stdint.h>
#include "term.h"


/**
 * @brief Moves cursor n character up
 * @param n
 */
void Term_CursorUp(FILE *f, uint8_t n)
{
	fprintf(f, "\033[%uA", n & 0xFF);
}

/**
 * @brief Moves cursor n character down
 * @param n
 */
void Term_CursorDown(FILE *f, uint8_t n)
{
	fprintf(f, "\033[%uB", n & 0xFF);
}

/**
 * @brief Moves cursor n character left
 * @param n
 */
void Term_CursorLeft(FILE *f, uint8_t n)
{
	fprintf(f, "\033[%uD", n & 0xFF);
}


/**
 * @brief Moves cursor n character right
 * @param n
 */
void Term_CursorRight(FILE *f, uint8_t n)
{
	fprintf(f, "\033[%uC", n & 0xFF);
}

void Term_Back(FILE *f, char c)
{
	fprintf(f, "%c\b\b", c);
}


void Term_FgColorSet(FILE *f, uint8_t color)
{
	fprintf(f, "\033[%um", 0xff & color);
}

void Term_FgColorClear(FILE *f)
{
	fprintf(f, "\033[0m");
}
