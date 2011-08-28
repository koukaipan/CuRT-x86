/**
 * @file    include/arch/x86/screen.h
 * @author  Ying-Shiuan Pan
 * @brief   x86 screen operations
 *
 */

#ifndef __X86_SCREEN_H__
#define __X86_SCREEN_H__

/****************************************************************************
*				CONSTANTS
*****************************************************************************/

/****************************************************************************
*				GLOBAL VARIABLES
*****************************************************************************/

/****************************************************************************
*				FUNCTION PROTOTYPES
*****************************************************************************/
void screen_init(void);
void screen_puts(const char *text);
void screen_putch(char c);
void screen_clear();
void set_cursor(void);
void settextcolor(unsigned char forecolor, unsigned char backcolor);
void screen_pos_puts(const char *text, int new_x, int new_y);
void screen_pos_putch(char c, int new_x, int new_y);

#endif
