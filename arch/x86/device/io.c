/**
 * @file    arch/x86/device/io.c
 * @author  Ying-Shiuan Pan
 * @brief	very low level in/out operations
 */

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

/* Notes: No warranty expressed or implied. Use at own risk. */

#include "device/io.h"

unsigned char inb(int port)
{
    unsigned char ret;
    __asm__ __volatile__ (
		"inb %w1, %b0" 
		: "=a" (ret) 
		: "Nd" (port)
	);
    return ret;
}

unsigned short inw(int port) 
{
	unsigned short ret;
	__asm__( 
		"inw %w1, %w0" 
		: "=a" (ret) 
		: "Nd" (port) 
	);
	return ret;
}

unsigned long inl(int port) 
{
	unsigned long ret;
	__asm__( 
		"inl %w1, %0" 
		: "=a" (ret) 
		: "Nd" (port) 
	);

	return ret;
}

void outb(int port, unsigned char value)
{
    __asm__ __volatile__ (
		"outb %b0, %w1" 
		: : "a" (value), "Nd" (port) 
	);
}

void outw(int port, unsigned short value) 
{
	__asm__(
		"outw %w0, %w1"
		: : "a" (value), "Nd" (port) 
	);
}

void outl(int port, unsigned long value) 
{
	__asm__ __volatile__ ( 
		"outl %0, %w1"
		: 
		: "a" (value), "Nd" (port) 
	);
}

