/*
 * asm_port.S
 */

#include "pxa255.h"
		
#define NO_INT 0xc0
#define NO_IRQ 0x80
#define NO_FIQ 0x40
#define SVC32_MODE 0x13
#define FIQ32_MODE 0x11
#define IRQ32_MODE 0x12

	.text
	.global restore_context
	.global context_switch
	.global context_switch_in_interrupt
	.global irq_service_routine
	.global save_cpu_sr
	.global restore_cpu_sr


/**
 * @brief Save CPU status.  Disable interrupts.
 *
 * IRQ interrupt status register value is returned to be disabled.
 * @param
 * @retval CPU status value
 * Usage: cpu_sr = save_cpu_sr(); 
 */
save_cpu_sr:
	mrs r0, CPSR
	orr r1, r0, #NO_INT
	msr CPSR_c, r1
	bx lr

	
/**
 * @brief The state will restore the value of registers
 *
 * Restore the given value of status register.
 * @param CPU status value
 * @retval
 * Usage: restore_cpu_sr(cpu_sr); 
 */
restore_cpu_sr:
	msr CPSR_c, r0
	bx lr

/**
 * @brief IRQ interrupt service routine
 *
 * IRQ interrupt service routine is called when raised.  Execution order:
 *   (1) IRQ registers are stored on the IRQ stack.
 *   (2) Were running to save the context of a thread.
 *   (3) About to issue the interrupt - interrupt_nesting
 *   (4) Stack pointer is stored in the structure of the current thread.
 *   (5) Call interrupt handler (tick increment).
 *   (6) Change interrupt_nesting in context - exit_interrupt,
 *       otherwise, the thread context of the existing restores.
 * @param
 * @retval
 */
irq_service_routine:
	msr CPSR_c, #(NO_INT | IRQ32_MODE)
	stmfd sp!, {r1-r3}		// push working registers onto IRQ stack
	mov r1, sp			// save IRQ stack pointer
	add sp, sp, #12			// adjust IRQ stack pointer
	sub r2, lr, #4			// adjust pc for return

	mrs r3, SPSR			// copy SPSR (interrupted thread's CPSR)
	msr CPSR_c, #(NO_INT | SVC32_MODE)	// change to SVC mode

					// save thread's context onto thread's stack
	stmfd sp!, {r2}			// push thread's return pc
	stmfd sp!, {lr}			// push thread's LR
	stmfd sp!, {r4-r12}		// push thread's r12-r4

	ldmfd r1!, {r4-r6}		// move thread's r1-r3 from IRQ stack to
					// SVC stack
	stmfd sp!, {r4-r6}		
	stmfd sp!, {r0}			// push thread's r0 onto thread's stack
	stmfd sp!, {r3}			// push thread's CPSR(IRQ's SPSR)

	bl enter_interrupt

	ldr r0, =interrupt_nesting
	ldrb r1, [r0]
	cmp r1, #1
	bne irq_service_routine_half
	
        ldr r4, =current_thread		// current_thread->stack_ptr = sp
        ldr r4, [r4]
        add r4, r4, #8
        str sp, [r4]
	
irq_service_routine_half:	
	msr CPSR_c, #(NO_IRQ | IRQ32_MODE) // re-enable FIQ, chagen to IRQ mode
		
	bl interrupt_handler

	msr CPSR_c, #(NO_INT | SVC32_MODE) // chagne to SVC mode

	bl exit_interrupt
	
        ldmfd sp!, {r4}
        msr SPSR_cxsf, r4
        ldmfd sp!, {r0-r12, lr, pc}^


/**
 * @brief Restore the context of the current thread.
 *
 * In multi-tasking environment, restore the context of the thread.
 * @param
 * @retval
 */
restore_context:
        ldr r4, =current_thread            // sp = current_thread->sp
        ldr r4, [r4]
        add r4, r4, #8
        ldr sp, [r4]
	
        ldr r4, [sp], #4
        msr SPSR_cxsf, r4
        ldmfd sp!, {r0-r12, lr, pc}^


/**
 * @brief Context switching
 *
 * During context switching, the following actions are performed:
 *   (1) Save the registers in thecurrent thread.
 *   (2) The stack pointer is stored in the current structure of the thread.
 *   (3) Run the next thread pointed by scheduler.
 *   (4) The stack pointer in the thread is read and executed.
 *   (5) Restore the registers of thread.
 * @param
 * @retval
 */
context_switch:
	stmfd sp!, {lr}			// save current thread's context
        stmfd sp!, {lr}
        stmfd sp!, {r0-r12, lr}
        mrs r4, SPSR
        stmfd sp!, {r4}
	
	ldr r4, =current_thread		// current_thread->stack_ptr = sp
	ldr r4, [r4]
	add r4, r4, #8
	str sp, [r4]
	
        ldr r4, =current_thread		// current_thread = next_thread
        ldr r6, =next_thread
        ldr r6, [r6]
        str r6, [r4]

	ldr r4, =next_thread		// sp = next_thread->sp
	ldr r4, [r4]
	add r4, r4, #8
	ldr sp, [r4]
		
        ldmfd sp!, {r4}		// restore next thread's context
        msr SPSR_cxsf, r4
        ldmfd sp!, {r0-r12, lr, pc}^


/**
 * @brief Context switching during interruption.
 *
 * Interrupt the context occurs later due to preemptive threads.
 * @param
 * @retval
 */
context_switch_in_interrupt:
        ldr r4, =current_thread		// current_thread = next_thread
        ldr r6, =next_thread
        ldr r6, [r6]
        str r6, [r4]

        ldr r4, =next_thread            // sp = next_thread->sp
	ldr r4, [r4]
        add r4, r4, #8
	ldr sp, [r4]
					// restore next thread's context
        ldmfd sp!, {r4}
        msr SPSR_cxsf, r4
        ldmfd sp!, {r0-r12, lr, pc}^
