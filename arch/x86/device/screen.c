/**
 * @file    arch/x86/device/screen.c
 * @author  
 * @brief	Screen output functions for Console I/O
 *
 * modified from bkerndev
 * bkerndev - Bran's Kernel Development Tutorial
 *  By:   Brandon F. (friesenb@gmail.com)
 *  Desc: Screen output functions for Console I/O
 *
 *  Notes: No warranty expressed or implied. Use at own risk. */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 */

#include "kernel/types.h"
#include "lib/string.h"
#include "device/screen.h"
#include "device/io.h"

/* These define our textpointer, our background and foreground
*  colors (attributes), and x and y cursor coordinates */
unsigned short *_l_screen_ptr;
int _l_attrib = 0x0F;
int _l_scr_px = 0, _l_scr_py = 0;

/* Scrolls the screen */
void scroll(void)
{
    unsigned blank, temp;

    /* A blank is defined as a space... we need to give it
    *  backcolor too */
    blank = 0x20 | (_l_attrib << 8);

    /* Row 25 is the end, this means we need to scroll up */
    if(_l_scr_py >= 25)
    {
        /* Move the current text chunk that makes up the screen
        *  back in the buffer by a line */
        temp = _l_scr_py - 25 + 1;
        memcpy (_l_screen_ptr, _l_screen_ptr + temp * 80, (25 - temp) * 80 * 2);

        /* Finally, we set the chunk of memory that occupies
        *  the last line of text to our 'blank' character */
        memsetw (_l_screen_ptr + (25 - temp) * 80, blank, 80);
        _l_scr_py = 25 - 1;
    }
}

/* Updates the hardware cursor: the little blinking line
*  on the screen under the last character pressed! */
void set_cursor(void)
{
    unsigned temp;

    /* The equation for finding the index in a linear
    *  chunk of memory can be represented by:
    *  Index = [(y * width) + x] */
    temp = _l_scr_py * 80 + _l_scr_px;

    /* This sends a command to indicies 14 and 15 in the
    *  CRT Control Register of the VGA controller. These
    *  are the high and low bytes of the index that show
    *  where the hardware cursor is to be 'blinking'. To
    *  learn more, you should look up some VGA specific
    *  programming documents. A great start to graphics:
    *  http://www.brackeen.com/home/vga */
    outb(0x3D4, 14);
    outb(0x3D5, temp >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, temp);
}


/* Puts a single character on the screen */
void screen_putch(char c)
{
    unsigned short *where;
    unsigned att = _l_attrib << 8;

    /* Handle a backspace, by moving the cursor back one space */
    if(c == 0x08)
    {
        if(_l_scr_px != 0) _l_scr_px--;
    }
    /* Handles a tab by incrementing the cursor's x, but only
    *  to a point that will make it divisible by 8 */
    else if(c == 0x09)
    {
        _l_scr_px = (_l_scr_px + 8) & ~(8 - 1);
    }
    /* Handles a 'Carriage Return', which simply brings the
    *  cursor back to the margin */
    else if(c == '\r')
    {
        _l_scr_px = 0;
    }
    /* We handle our newlines the way DOS and the BIOS do: we
    *  treat it as if a 'CR' was also there, so we bring the
    *  cursor to the margin and we increment the 'y' value */
    else if(c == '\n')
    {
        _l_scr_px = 0;
        _l_scr_py++;
    }
    /* Any character greater than and including a space, is a
    *  printable character. The equation for finding the index
    *  in a linear chunk of memory can be represented by:
    *  Index = [(y * width) + x] */
    else if(c >= ' ')
    {
        where = _l_screen_ptr + (_l_scr_py * 80 + _l_scr_px);
        *where = c | att;	/* Character AND attributes: color */
        _l_scr_px++;
    }

    /* If the cursor has reached the edge of the screen's width, we
    *  insert a new line in there */
    if(_l_scr_px >= 80)
    {
        _l_scr_px = 0;
        _l_scr_py++;
    }

    /* Scroll the screen if needed, and finally move the cursor */
    scroll();
    set_cursor();
}


/* Sets the forecolor and backcolor that we will use */
void settextcolor(unsigned char forecolor, unsigned char backcolor)
{
    /* Top 4 bytes are the background, bottom 4 bytes
    *  are the foreground color */
    _l_attrib = (backcolor << 4) | (forecolor & 0x0F);
}

// ........................................................................... //
// Public Function
// ........................................................................... //
/* Clears the screen */
void screen_clear()
{
    unsigned blank;
    int i;

    /* Again, we need the 'short' that will be used to
    *  represent a space with color */
    blank = 0x20 | (_l_attrib << 8);

    /* Sets the entire screen to spaces in our current
    *  color */
    for(i = 0; i < 25; i++)
        memsetw (_l_screen_ptr + i * 80, blank, 80);

    /* Update out virtual cursor, and then move the
    *  hardware cursor */
    _l_scr_px = 0;
    _l_scr_py = 0;
    set_cursor();
}
/* Sets our text-mode VGA pointer, then clears the screen for us */
void screen_init(void)
{
    _l_screen_ptr = (unsigned short *)0xB8000;
    screen_clear();
}

/* Uses the above routine to output a string... */
void screen_puts(const char *text)
{
	while(*text)
		screen_putch(*text++);
}

/* move cursor and puts */
void screen_pos_puts(const char *text, int new_x, int new_y)
{
	int old_x = _l_scr_px;
	int old_y = _l_scr_py;
	
	_l_scr_px = new_x;
    _l_scr_py = new_y;
    screen_puts(text);
    
    _l_scr_px = old_x;
    _l_scr_py = old_y;
}

/* move cursor and putch */
void screen_pos_putch(char c, int new_x, int new_y)
{
	int old_x = _l_scr_px;
	int old_y = _l_scr_py;
	
	_l_scr_px = new_x;
    _l_scr_py = new_y;
    screen_putch(c);
    
     _l_scr_px = old_x;
    _l_scr_py = old_y;
}
