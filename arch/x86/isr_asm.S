/**
 * @file    arch/x86/irq_asm.S
 * @author  Ying-Shiuan Pan
 * @brief   ISR entry
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

#include "apic_addr.h"

.section	.text
	.global isr0
	.global isr1
	.global isr2
	.global isr3
	.global isr4
	.global isr5
	.global isr6
	.global isr7
	.global isr8
	.global isr9
	.global isr10
	.global isr11
	.global isr12
	.global isr13
	.global isr14
	.global isr15
	.global isr16
	.global isr17
	.global isr18
	.global isr19
	.global isr20
	.global isr21
	.global isr22
	.global isr23
	.global isr24
	.global isr25
	.global isr26
	.global isr27
	.global isr28
	.global isr29
	.global isr30
	.global isr31

//  0: Divide By Zero Exception
isr0:
    cli
    push $0
    push $0
    jmp isr_common_stub

//  1: Debug Exception
isr1:
    cli
    push $0
    push $1
    jmp isr_common_stub

//  2: Non Maskable Interrupt Exception
isr2:
    cli
    push $0
    push $2
    jmp isr_common_stub

//  3: Int 3 Exception
isr3:
    cli
    push $0
    push $3
    jmp isr_common_stub

//  4: INTO Exception
isr4:
    cli
    push $0
    push $4
    jmp isr_common_stub

//  5: Out of Bounds Exception
isr5:
    cli
    push $0
    push $5
    jmp isr_common_stub

//  6: Invalid Opcode Exception
isr6:
    cli
    push $0
    push $6
    jmp isr_common_stub

//  7: Coprocessor Not Available Exception
isr7:
    cli
    push $0
    push $7
    jmp isr_common_stub

//  8: Double Fault Exception (With Error Code!)
isr8:
    cli
    push $8
    jmp isr_common_stub

//  9: Coprocessor Segment Overrun Exception
isr9:
    cli
    push $0
    push $9
    jmp isr_common_stub

// 10: Bad TSS Exception (With Error Code!)
isr10:
    cli
    push $10
    jmp isr_common_stub

// 11: Segment Not Present Exception (With Error Code!)
isr11:
    cli
    push $11
    jmp isr_common_stub

// 12: Stack Fault Exception (With Error Code!)
isr12:
    cli
    push $12
    jmp isr_common_stub

// 13: General Protection Fault Exception (With Error Code!)
isr13:
    cli
    push $13
    jmp isr_common_stub

// 14: Page Fault Exception (With Error Code!)
isr14:
    cli
    push $14
    jmp isr_common_stub

// 15: Reserved Exception
isr15:
    cli
    push $0
    push $15
    jmp isr_common_stub

// 16: Floating Point Exception
isr16:
    cli
    push $0
    push $16
    jmp isr_common_stub

// 17: Alignment Check Exception
isr17:
    cli
    push $0
    push $17
    jmp isr_common_stub

// 18: Machine Check Exception
isr18:
    cli
    push $0
    push $18
    jmp isr_common_stub

// 19: Reserved
isr19:
    cli
    push $0
    push $19
    jmp isr_common_stub

// 20: Reserved
isr20:
    cli
    push $0
    push $20
    jmp isr_common_stub

// 21: Reserved
isr21:
    cli
    push $0
    push $21
    jmp isr_common_stub

// 22: Reserved
isr22:
    cli
    push $0
    push $22
    jmp isr_common_stub

// 23: Reserved
isr23:
    cli
    push $0
    push $23
    jmp isr_common_stub

// 24: Reserved
isr24:
    cli
    push $0
    push $24
    jmp isr_common_stub

// 25: Reserved
isr25:
    cli
    push $0
    push $25
    jmp isr_common_stub

// 26: Reserved
isr26:
    cli
    push $0
    push $26
    jmp isr_common_stub

// 27: Reserved
isr27:
    cli
    push $0
    push $27
    jmp isr_common_stub

// 28: Reserved
isr28:
    cli
    push $0
    push $28
    jmp isr_common_stub

// 29: Reserved
isr29:
    cli
    push $0
    push $29
    jmp isr_common_stub

// 30: Reserved
isr30:
    cli
    push $0
    push $30
    jmp isr_common_stub

// 31: Reserved, used for APIC Timer ISR
isr31:
	cli
	push $0
	push $31
	jmp isr_common_stub

// We call a C function in here. We need to let the assembler know
// that 'fault_handler' exists in another file

// This is our common ISR stub. It saves the processor state, sets
// up for kernel mode segments, calls the C-level fault handler,
// and finally restores the stack frame.
isr_common_stub:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
	mov %ax, %gs
    mov %esp, %eax
    push %eax
    call fault_handler
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp
    iret
