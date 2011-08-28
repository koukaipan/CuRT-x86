/**
 * @file    arch/x86/ctx_sw.S
 * @author  Ying-Shiuan Pan
 * @brief   context switch (sava and restore)
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
.section	.text
	.global os_ctx_sw_int
	.global os_ctx_sw
	.global restore_ctx

/** 
 * @brief save current task context (in normal context)
 */
os_ctx_sw:
save_ctx:	/* 1. save the current registers */
	push $0x000000FF	//從process context保存
	push $0x000000FF
	pusha	//eax, ecx, edx, ebx, esp, ebp, esi, edi 
	push %ds
	push %es
	push %fs
	push %gs
	
	/* 2. current_thread->stack_ptr = esp */
	mov		current_thread, %eax
	add		$8, %eax
	mov		%esp, (%eax)
    
	/* 3. current_thread = next_thread; */
	movl	next_thread, %eax
	movl	%eax, current_thread


/** 
 * @brief 
 */
os_ctx_sw_int:

/** 
 * @brief Restore context
 * We should check whether the context is in interrupt or not
 * int_no and err_code = 0xff
 */
restore_ctx:
	/* 4. esp = next_thread->sp  :change to new stack */
	movl	current_thread, %eax
	add		$8, %eax
	movl	(%eax), %esp
	pop %gs
	pop %fs
	pop %es
	pop %ds
	/* 5. restore context */
	/* check if it is 0xff, then jmp and popad */
	mov 32(%esp), %ax
	cmp $0x00ff, %ax
	je ret_from_normal

/** 
 * @brief returen from interrupt context
 */
ret_from_int:
	popa
	addl $8, %esp	//handle the 2 parameters(int_no err_code) pushed by irq(n)
	iret

/** 
 * @brief returen from normal context
 */
ret_from_normal:
	popa
	addl $8, %esp
	sti
	ret
