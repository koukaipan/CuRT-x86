/*
 * port.c
 */

#include "port.h"
#include "lib/stdio.h"
#include "kernel/list.h"
#include "kernel/thread.h"
#include "kernel/kernel.h"

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
	stk_t *pstk;

	pstk = stk_top_ptr;
	*(pstk) = (stk_t) func;			/* r15 (pc) thread address */
	*(--pstk) = (stk_t) 0x14141414L;	/* r14 (lr) */
	*(--pstk) = (stk_t) 0x12121212L;	/* r12 */
	*(--pstk) = (stk_t) 0x11111111L;	/* r11 */
	*(--pstk) = (stk_t) 0x10101010L;	/* r10 */
	*(--pstk) = (stk_t) 0x09090909L;	/* r9 */
	*(--pstk) = (stk_t) 0x08080808L;	/* r8 */
	*(--pstk) = (stk_t) 0x07070707L;	/* r7 */
	*(--pstk) = (stk_t) 0x06060606L;	/* r6 */
	*(--pstk) = (stk_t) 0x05050505L;	/* r5 */
	*(--pstk) = (stk_t) 0x04040404L;	/* r4 */
	*(--pstk) = (stk_t) 0x03030303L;	/* r3 */
	*(--pstk) = (stk_t) 0x02020202L;	/* r2 */
	*(--pstk) = (stk_t) 0x01010101L;	/* r1 */
	*(--pstk) = (stk_t) pdata;		/* r0 */
	*(--pstk) = (stk_t) 0x13;		/* cpsr : sys_mode */
	return pstk;
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
	INT_REG(INT_ICLR) &= ~BIT26;
	TMR_REG(TMR_OSMR0) = PXA255_TMR_CLK / OS_TICKS_PER_SEC;
	TMR_REG(TMR_OSMR1) = 0x3FFFFFFF;
	TMR_REG(TMR_OSMR2) = 0x7FFFFFFF;
	TMR_REG(TMR_OSMR3) = 0xBFFFFFFF;
	TMR_REG(TMR_OSCR) = 0x00;
	TMR_REG(TMR_OSSR) = BIT0;
	TMR_REG(TMR_OIER) = BIT0;
	INT_REG(INT_ICMR) |= BIT26;
}

/**
 * @brief Interrupt handler
 *
 * This function is invoked in the IRQ service routine.
 * source: when calling advance_time_tick, ticks advance.
 */
void interrupt_handler()
{
	if (INT_REG(INT_ICIP) & BIT26) {
		TMR_REG(TMR_OSCR) = 0x00;
		advance_time_tick();
		TMR_REG(TMR_OSSR) = BIT0;
	}
}

/**
 * @brief Initialize CPU interrupt control
 *
 * Enable the issue of interrupts.
 */
void init_interrupt_control()
{
	INT_REG(INT_ICMR) = 0;
}

