/**
 * @file    arch/x86/port.c
 * @author  Ying-Shiuan Pan
 * @brief   implement necessary functions for porting
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

#include "port.h"
#include "lib/stdio.h"
#include "kernel/list.h"
#include "kernel/thread.h"
#include "kernel/kernel.h"
#include "x86.h"

#include "kernel/types.h"

/**
 * @brief Initialize the thread stack.
 *
 * Initialize the thread stack value with the given thread function, and
 * then save registers.
 * @param func
 * @param pdata
 * @param stk_top_ptr
 * @retval
 */
stk_t *init_thread_stack(THREAD_FUNC func, 
                         void *pdata,
                         stk_t *stk_top_ptr)
{
	stk_t *pstk = stk_top_ptr;
	
	*pstk-- = (stk_t)pdata;		/* Simulate call to function with argument */
	*pstk-- = (stk_t)func;		/* return address 中斷發生時的PC, 此處用假的*/
	*pstk-- = (stk_t)0x000000ff;	/* err_code  */
	*pstk-- = (stk_t)0x000000ff;	/* int_no */	
	
	*pstk-- = (stk_t)0xAAAAAAAA;	/* EAX = 0xAAAAAAAA */
	*pstk-- = (stk_t)0xCCCCCCCC;	/* ECX = 0xCCCCCCCC */
	*pstk-- = (stk_t)0xDDDDDDDD;	/* EDX = 0xDDDDDDDD */
	*pstk-- = (stk_t)0xBBBBBBBB;	/* EBX = 0xBBBBBBBB */
	*pstk-- = (stk_t)0x00000000;	/* ESP = 0x00000000 */
	*pstk-- = (stk_t)0x11111111;	/* EBP = 0x11111111 */
	*pstk-- = (stk_t)0x22222222;	/* ESI = 0x22222222 */
	*pstk-- = (stk_t)0x33333333;	/* EDI = 0x33333333 */
	
	*pstk-- = (stk_t)0x00000010;	/* DS = 0x00000010	the 2nd GDT	*/
	*pstk-- = (stk_t)0x00000010;	/* ES = 0x00000010				*/
	*pstk-- = (stk_t)0x00000010;	/* FS = 0x00000010				*/
	*pstk   = (stk_t)0x00000010;	/* GS = 0x00000010				*/

	return ((stk_t*)pstk);
}

/**
 * @brief Initialize CuRT Timer
 *
 * Once OS Timer is initialized, the timer is about to work.
 * Note: the multi-tasking environment (setup by start_curt) will be invoked
 * after this routine.
 */
void init_os_timer()
{
	
}

/**
 * @brief Interrupt handler
 *
 * This function is invoked in the IRQ service routine.
 * source: when calling advance_time_tick, ticks advance.
 * 
 * The function is implemented in timer_handler() @timer.c
 */
/* void interrupt_handler(){} */

/**
 * @brief Initialize CPU interrupt control
 *
 * Enable the issue of interrupts.
 *
 * we use arch_init() */
/* void init_interrupt_control() */

/**
 * @brief Architecture dependent Initialization
 *
 * call other init fns.
 */
void arch_init()
{
	x86_init();
}

void arch_start()
{
	enable_int();
}

cpu_sr_t save_cpu_sr()
{
	return 0;
}

void restore_cpu_sr(cpu_sr_t cpu_sr)
{
	return;
}

