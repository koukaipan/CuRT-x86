/**
 * @file    include/arch/x86/gdt.h
 * @author  Ying-Shiuan Pan
 * @brief   Global description table
 */

#ifndef __X86_GDT_H__
#define __X86_GDT_H__

/****************************************************************************
*				CONSTANTS
*****************************************************************************/
#define NR_GDT_ENTRIES		10

/****************************************************************************
*				STRUCTURES
*****************************************************************************/
/* Defines a GDT entry */
struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

/****************************************************************************
*				GLOBAL VARIABLES
*****************************************************************************/

/****************************************************************************
*				FUNCTION PROTOTYPES
*****************************************************************************/
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void gdt_install();

#endif
