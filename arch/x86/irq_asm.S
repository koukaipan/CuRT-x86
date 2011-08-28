/**
 * @file    arch/x86/irq_asm.S
 * @author  Ying-Shiuan Pan koukai
 * @brief   IRQ handler entry
 */
.section	.text
	.global irq0
	.global irq1
	.global irq2
	.global irq3
	.global irq4
	.global irq5
	.global irq6
	.global irq7
	.global irq8
	.global irq9
	.global irq10
	.global irq11
	.global irq12
	.global irq13
	.global irq14
	.global irq15

// 32: IRQ0 Timer Interrupt
irq0:
    cli
    push $0
    push $32
    jmp irq_common_stub

// 33: IRQ1
irq1:
    cli
    push $0
    push $33
    jmp irq_common_stub

// 34: IRQ2
irq2:
    cli
    push $0
    push $34
    jmp irq_common_stub

// 35: IRQ3
irq3:
    cli
    push $0
    push $35
    jmp irq_common_stub

// 36: IRQ4
irq4:
    cli
    push $0
    push $36
    jmp irq_common_stub

// 37: IRQ5
irq5:
    cli
    push $0
    push $37
    jmp irq_common_stub

// 38: IRQ6
irq6:
    cli
    push $0
    push $38
    jmp irq_common_stub

// 39: IRQ7
irq7:
    cli
    push $0
    push $39
    jmp irq_common_stub

// 40: IRQ8
irq8:
    cli
    push $0
    push $40
    jmp irq_common_stub

// 41: IRQ9
irq9:
    cli
    push $0
    push $41
    jmp irq_common_stub

// 42: IRQ10
irq10:
    cli
    push $0
    push $42
    jmp irq_common_stub

// 43: IRQ11
irq11:
    cli
    push $0
    push $43
    jmp irq_common_stub

// 44: IRQ12
irq12:
    cli
    push $0
    push $44
    jmp irq_common_stub

// 45: IRQ13
irq13:
    cli
    push $0
    push $45
    jmp irq_common_stub

// 46: IRQ14
irq14:
    cli
    push $0
    push $46
    jmp irq_common_stub

// 47: IRQ15
irq15:
    cli
    push $0
    push $47
    jmp irq_common_stub

irq_common_stub:
    pusha
    push 	%ds
    push 	%es
    push 	%fs
    push 	%gs

    //the 2nd entry of GDT
    mov 	$0x10, %ax
    mov 	%ax, %ds
    mov 	%ax, %es
    mov 	%ax, %fs
    mov 	%ax, %gs
    
    // 保留目前的stack到current_thread->stack_ptr中
    mov		current_thread, %eax
    add		$8, %eax
    mov		%esp, (%eax)

    mov 	%esp, %eax	//作為irq_handler的參數(指向stack頂端的指標)
    push 	%eax
    
    call 	irq_handler
    pop 	%eax		//caller has to clean the stack

	//call task_resched_int
	jmp os_ctx_sw_int
