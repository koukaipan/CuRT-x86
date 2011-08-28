/**
 * @file    arch/x86/gdt.c
 * @author  Ying-Shiuan Pan
 * @brief   Global Descriptor Table management
 * 
 * modified from bkerndev
 * bkerndev - Bran's Kernel Development Tutorial
 *  By:   Brandon F. (friesenb@gmail.com)
 *  Desc: Global Descriptor Table management
 *
 *  Notes: No warranty expressed or implied. Use at own risk.
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


#include "gdt.h"
#include "kernel/types.h"

/* Our GDT, with 3 entries, and finally our special GDT pointer */
static struct gdt_entry gdt[NR_GDT_ENTRIES];

/**
 * @brief to properly reload the new segment registers
 *
 * @param gp gdt table pointer
 */
static void gdt_flush(struct gdt_ptr gp)
{
	__asm__
	(
		"lgdtw %0\t\n"
    	"movl $0x10, %%eax\t\n"
    	"movl %%eax, %%ds\t\n"
    	"movl %%eax, %%es\t\n"
    	"movl %%eax, %%fs\t\n"
    	"movl %%eax, %%gs\t\n"
    	"movl %%eax, %%ss\t\n"
		:	/* y is output operand */
		:"m"(gp)		/* x is input operand */
		:"%eax"	/* clobbered registers */
	);
}

/** 
 * @brief Setup a descriptor in the Global Descriptor Table 
 */
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
	/* Setup the descriptor base address */
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;

	/* Setup the descriptor limits */
	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = ((limit >> 16) & 0x0F);

	/* Finally, set up the granularity and access flags */
	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access = access;
}

/**
 * @brief setup GDT
 *
 * Should be called by main. This will setup the special GDT
 * pointer, set up the first 3 entries in our GDT, and then
 * finally call gdt_flush() in our assembler file in order
 * to tell the processor where the new GDT is and update the
 * new segment registers 
 */
void gdt_install()
{
	/* Setup the GDT pointer and limit */
	struct gdt_ptr gp;
	gp.limit = (sizeof(struct gdt_entry) * NR_GDT_ENTRIES) - 1;
	gp.base = (unsigned int)&gdt;

	/* Our NULL descriptor */
	gdt_set_gate(0, 0, 0, 0, 0);

	/* The second entry is our Code Segment. The base address
	*  is 0, the limit is 4GBytes, it uses 4KByte granularity,
	*  uses 32-bit opcodes, and is a Code Segment descriptor.
	*  Please check the table above in the tutorial in order
	*  to see exactly what each value means */
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

	/* The third entry is our Data Segment. It's EXACTLY the
	*  same as our code segment, but the descriptor type in
	*  this entry's access byte says it's a Data Segment */
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

	/* The 4th entry is 
	gdt_video:	Descriptor 0B8000h, 0ffffh, DA_DRW | 0xcf00 */
	gdt_set_gate(3, 0xB8000, 0xFFFFFFFF, 0x92, 0xCF);

	/* Flush out the old GDT and install the new changes! */
	gdt_flush(gp);
}
