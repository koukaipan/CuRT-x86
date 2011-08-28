/**
 * @file    include/x86/io.h
 * @author  Ying-Shiuan Pan koukai
 * @brief   IO port operations
 */

#ifndef __X86_IO_H__
#define __X86_IO_H__

/****************************************************************************
*				FUNCTION PROTOTYPES
*****************************************************************************/
unsigned char inb(int port);
unsigned short inw(int port);
unsigned long inl(int port);

void outb(int port, unsigned char value);
void outw(int port, unsigned short value); 
void outl(int port, unsigned long value);

#endif
