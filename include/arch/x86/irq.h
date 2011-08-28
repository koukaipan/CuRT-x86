/**
 * @file    include/arch/x86/irq.h
 * @author  Ying-Shiuan Pan
 * @brief   IRQ routines and operations
 */
 
#ifndef __X86_IRQ_H__
#define __X86_IRQ_H__


#include "x86.h"

/****************************************************************************
*						FUNCTION PROTOTYPES
*****************************************************************************/
/* These are own ISRs that point to our special IRQ handler
 * instead of the regular 'fault_handler' function 
 * irq_asm.asm */
void irq0();
void irq1();
void irq2();
void irq3();
void irq4();
void irq5();
void irq6();
void irq7();
void irq8();
void irq9();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();

/* IRQ.C */
void irq_install_handler(int irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(int irq);
void irq_install();

#endif
