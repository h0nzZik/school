#include <stdint.h>

#include "lcd.h"
#include "czech.h"

static const char *cz_chars = "бипймнтушљќъщэћ";
static const char *cz_ascii_chars = "acdeeinorstuuyz";


static const uint8_t cz_char_cgrom[][8] = {
	{ 0x02, 0x04, 0x00, 0x0e, 0x01, 0x0f, 0x11, 0x0f }, // б
	{ 0x0a, 0x04, 0x00, 0x0e, 0x10, 0x10, 0x11, 0x0e }, // и
	{ 0x0b, 0x05, 0x01, 0x0d, 0x13, 0x11, 0x11, 0x0e }, // п
	{ 0x00, 0x02, 0x02, 0x0e, 0x11, 0x1f, 0x10, 0x0e }, // й
	{ 0x00, 0x05, 0x02, 0x0e, 0x11, 0x1f, 0x10, 0x0e }, // м
	{ 0x00, 0x01, 0x02, 0x0c, 0x04, 0x04, 0x04, 0x0e }, // н
	{ 0x09, 0x04, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11 }, // т
	{ 0x02, 0x04, 0x00, 0x0e, 0x11, 0x11, 0x11, 0x0e }, // у
	{ 0x0a, 0x04, 0x00, 0x16, 0x19, 0x10, 0x10, 0x10 }, // ш
	{ 0x0a, 0x04, 0x00, 0x0e, 0x10, 0x0e, 0x01, 0x1e }, // љ
	{ 0x09, 0x1b, 0x08, 0x08, 0x08, 0x09, 0x06, 0x00 }, // ќ
	{ 0x02, 0x04, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0d }, // ъ
	{ 0x04, 0x0a, 0x04, 0x11, 0x11, 0x11, 0x13, 0x0d }, // щ
	{ 0x00, 0x04, 0x04, 0x11, 0x11, 0x0f, 0x01, 0x0e }, // э
	{ 0x0a, 0x04, 0x1f, 0x02, 0x02, 0x04, 0x08, 0x1f }, // ћ
};


/**
 * @brief lcd_cz[i] - czech character ID, holded by slot i.
 */
static uint8_t lcd_cz[N_LCD_CHARS];


/**
 * @brief Loads czech character into LCD slot.
 * @param czech_char_id - identification of czech character
 * @param lcd_slot_id - identification of lcd slot
 */
static void lcd_load_cz_char(uint8_t cz_char_id, uint8_t lcd_slot_id)
{

	LCD_sendcmd(LCD_EN1| cmd_set_cgram_addr | (lcd_slot_id << 3) );

	uint8_t i = 0;
	for (i=0; i < 8; i++)
	{
		LCD_senddata(LCD_EN1 | cz_char_cgrom[cz_char_id][i]);
	}
	LCD_sendcmd(LCD_EN1 | cmd_set_ddram_addr);
	LCD_cursor_yx(coord.y+1,coord.x);

}


/**
 * @brief Makes lcd slots empty by clearing them
 * @param lcd_cz_slots
 * @param n_slots
 */
static void lcd_cz_make_slots_empty(uint8_t *lcd_cz_slots, uint8_t n_slots)
{
	uint8_t i;
	for (i=0; i<n_slots; i++)
	{
		lcd_cz_slots[i] = 0xFF;
	}
}


/**
 * Prints character from slot
 * @param slot_id - slot to print
 */
static void lcd_print_slot(uint8_t slot_id, display_t *dis)
{
	LCD_senddata( ((dis->y < 2) ? LCD_EN1 : LCD_EN2)  | slot_id);
}

/**
 * Prints czech character into screen
 * @param cz_id - character identification
 */
static void print_czech_character(uint8_t cz_id, display_t *dis)
{
	// Does this character have its bitmap in LCD?
	uint8_t i;
	uint8_t empty = 0xFF;
	for (i=0; i<N_LCD_CHARS; i++)
	{
		// yes
		if (lcd_cz[i] == cz_id)
		{
			break;
		}

		if (lcd_cz[i] == 0xFF && empty == 0xFF)
		{
			empty = i;
		}

	}

	// No. So we must load it
	if (i == N_LCD_CHARS)
	{
		// We _do_ have an empty slot
		if (empty != 0xFF)
		{
			i = empty;
			lcd_cz[i] = cz_id;
			lcd_load_cz_char(cz_id, i);
		}
		// No empty slot left. Use default czech chars
		else
		{
			lcd_print_slot(cz_ascii_chars[cz_id], dis);
			return;
		}

	}

	lcd_print_slot(i, dis);
}


void czech_init(void)
{
	lcd_cz_make_slots_empty(lcd_cz, N_LCD_CHARS);
}

/**
 * @brief Gets czech id of character 'c' or 0xFF i 'c' is not czech character
 */
static uint8_t lcd_czech_get_cz_id(char c)
{
	uint8_t i = 0;
	while(cz_chars[i])
	{
		if (cz_chars[i] == c)
		{
			return i;
		}
		i++;
	}

	return 0xFF;
}

/**
 * @brief Puts czech cp-1250 encoded character into display
 */
void lcd_cz_putc(char c, display_t *dis)
{
	uint8_t id = lcd_czech_get_cz_id(c);

	// Czech character
	if (id != 0xFF)
	{
		print_czech_character(id, &coord);
	} else {
		LCD_senddata( ((coord.y < 2) ? LCD_EN1 : LCD_EN2) | c);
	}	
}
