/**
 * @file    include/arch/x86/port.h
 * @author  Ying-Shiuan Pan
 * @brief   
 */

#ifndef __X86_PORT_H__
#define __X86_PORT_H__

#include "kernel/types.h"
#include "kernel/thread.h"

#define OS_TICKS_PER_SEC 10

/* port.c */
stk_t *init_thread_stack(THREAD_FUNC func,
                         void *pdata,
                         stk_t *stk_top_ptr);

void arch_init();
void arch_start();
void init_os_timer();

void interrupt_handler();

void init_interrupt_control();

/* context switching routines : architecture implementations */
extern void restore_ctx();
#define restore_context() restore_ctx()
//extern void context_switch();
extern void os_ctx_sw();
#define context_switch(); os_ctx_sw()
//extern int context_switch_in_interrupt();
extern void os_ctx_sw_int();
#define context_switch_in_interrupt() os_ctx_sw_int()

/* port.c */
extern cpu_sr_t save_cpu_sr();
extern void restore_cpu_sr();


#endif /* ! __X86_PORT_H__ */
