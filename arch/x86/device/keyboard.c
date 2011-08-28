/**
 * @file    arch/x86/device/keyboard.c
 * @author  Ying-Shiuan Pan
 * @brief	keyborad driver
 *
 * modified from bkerndev
 * bkerndev - Bran's Kernel Development Tutorial
 *  By:   Brandon F. (friesenb@gmail.com)
 *  Desc: Keyboard driver
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
 * 
 * Notes: No warranty expressed or implied. Use at own risk. 
 */

#include "device/keyboard.h"
#include "device/io.h"
#include "kernel/console.h"

#define KB_DATA_PORT	0x60

#define	KB_BUF_SIZE		32	/* size of the ring buffer (in byte) */

/* ring buffer for store pressed key */
static struct _kb_buf{
	int head, tail;
	int	cnt;
	char buf[KB_BUF_SIZE];	/* stores ascii code */
}kb_buf;

/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
static unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,					/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

/**
 *
 */
static void put_buf(char c)
{
	/* FIXME: critical section */
	if (kb_buf.cnt < (KB_BUF_SIZE - 1))
	{
		kb_buf.buf[kb_buf.head] = c;
		kb_buf.head = (kb_buf.head + 1) % KB_BUF_SIZE;
		kb_buf.cnt++;
	}
}

/**
 *
 */
static int get_buf()
{
	unsigned char c;

	/* wait for key presses */
	while(kb_buf.cnt == 0);

	disable_int();
	c = kb_buf.buf[kb_buf.tail];
	kb_buf.tail = (kb_buf.tail + 1) % KB_BUF_SIZE;
	kb_buf.cnt--;
	enable_int();

	return c;
}

/**
 *
 */
int keyboard_read()
{
	return get_buf();
}

/* Handles the keyboard interrupt */
void keyboard_handler(struct regs *r)
{
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
    }
    else
    {
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */
        //console_putc(kbdus[scancode]);
		put_buf(kbdus[scancode]);
    }
}

/* Installs the keyboard handler into IRQ1 */
void init_keyboard()
{
	kb_buf.cnt = 0;
	kb_buf.head = kb_buf.tail = 0;

    irq_install_handler(1, keyboard_handler);
}
