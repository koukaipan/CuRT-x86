/**
 * @file    include/arch/x86/idt.h
 * @author  Ying-Shiuan Pan
 * @brief   Interrupt description table
 */

#ifndef __X86_IDT_H__
#define __X86_IDT_H__

/****************************************************************************
*				CONSTANTS
*****************************************************************************/
#define NR_IDT_ENTRIES		256

/****************************************************************************
*				STRUCTURES
*****************************************************************************/
/* Defines an IDT entry */
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
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
void idt_set_gate(unsigned char num, unsigned long base, 
			unsigned short sel, unsigned char flags);
void idt_install();

#endif
