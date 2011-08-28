/**
 * @file    apic.S
 * @author  Ying-Shiuan Pan
 * @brief   Advanced programmable interrupt chip
 */

#include "apic_addr.h"

.code32

.text
	.global apic_init

apic_init:
	//Enable APIC
	movl	$0x0000001B, %ecx
	rdmsr
	orl		$0x00000800, %eax	//global apic enable
	wrmsr

	//setup timer
	movl	$0x0000000B, (APIC_TimerDIV_ADDR) 	//APIC Timer division ratio(2)
	movl	$0x00F0000B, (APIC_TimerINIT_ADDR)	//APIC Timer initial value
	movl	$0x0000000B, (APIC_TimerCURR_ADDR)	//APIC Timer current value
	
	//enable local apic
	movl	$0x00000100, (APIC_SPURINT_ADDR)
	
	//enable timer bit17 periodic, bit7:0 vector ISR#31
	movl	$0x0002001F, (APIC_Timer_ADDR)

	ret
