/**
 * @file    include/arch/x86/isr.h
 * @author  Ying-Shiuan Pan
 * @brief   ISR routines and operations
 */
 
#ifndef __X86_ISRS_H__
#define __X86_ISRS_H__

/****************************************************************************
*						FUNCTION PROTOTYPES
*****************************************************************************/
/* isr.c */
void isrs_install();

/* These are function prototypes for all of the exception
*  handlers: The first 32 entries in the IDT are reserved
*  by Intel, and are designed to service exceptions! 
*  isr_asm.asm */
void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();

void sys_call();

#endif 
