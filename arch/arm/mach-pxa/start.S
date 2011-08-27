/*
 * start.S
 */

#include "pxa255.h"

#define SDRAM_BASE_ADDRESS	0xa0000000

#define CFG_MECR_VAL		0x00000000
#define CFG_MCMEM0_VAL		0x00004204
#define CFG_MCMEM1_VAL		0x00000000
#define CFG_MCATT0_VAL		0x00010504
#define CFG_MCATT1_VAL		0x00000000
#define CFG_MCIO0_VAL		0x00008407
#define CFG_MCIO1_VAL		0x00000000
	
#define FIQ_STACK_SIZE		0x10000
#define IRQ_STACK_SIZE		0x10000
#define ABT_STACK_SIZE		0x10000
#define UND_STACK_SIZE		0x10000
#define SYS_STACK_SIZE		0x10000

	/* Declare the names of stacks */
	.global _start
	.global fiq_stack
	.global irq_stack
	.global abt_stack
	.global und_stack
	.global sys_stack

	/* Stack allocation */
	.comm fiq_stack, FIQ_STACK_SIZE
	.comm irq_stack, IRQ_STACK_SIZE
	.comm abt_stack, ABT_STACK_SIZE
	.comm und_stack, UND_STACK_SIZE
	.comm sys_stack, SYS_STACK_SIZE

	.text
	/* exception handler vector table */
_start:	
	b reset_handler
	b und_handler
	b swi_handler
	b abt_pref_handler
	b abt_data_handler
	b not_used
	b irq_handler
	b fiq_handler

.align 4
und_handler:
	b loop

swi_handler:
	b loop

abt_pref_handler:
	b loop

abt_data_handler:
	b loop

not_used:
	b loop

loop:
	b loop

irq_handler:
	b irq_service_routine

fiq_handler:
	b loop

reset_handler:
	bl set_misc

	bl mask_off_int_reg

	bl init_clock_reg

	bl set_core_clock

	bl set_os_timer

	bl init_gpio

	bl init_mem_ctrl

	bl set_stack_pointer

	bl set_svc_mode
	
	bl relocate
	
	bl jump_to_main
					
set_misc:		
        ldr r0, =0x2001			/* Allows access to all coprocessors */
        mcr     p15, 0, r0, c15, c1, 0
        nop
        nop
        nop
	
        ldr     r0, =0x00000078         /* Disable MMU, caches, write buffer */
        mcr     p15, 0, r0, c1, c0, 0
        nop
        nop
        nop

        ldr     r0, =0x00000000
        mcr     p15, 0, r0, c8, c7, 0 		/* Flush TLB's */
        mcr     p15, 0, r0, c7, c7, 0 		/* Flush Caches */
        mcr     p15, 0, r0, c7, c10, 4 		/* Flush Write Buffer */
        nop
        nop
        nop

        mvn    r0, #0                   /* grant manager access to all domains */
        mcr    p15,0,r0,c3,c0,0


        /*
         * Initializing PXA250 interrupt controller.
         */

mask_off_int_reg:	/* Mask off all IRQs and FIQs */
        ldr r1, =(INT_BASE | INT_ICMR)
        ldr r2, =0x0	/* interrupt gets mask off */
        str r2, [r1]

init_clock_reg:		/* PXA250 Clock Register initialization */
        ldr r1, =CLK_BASE	/* base of clock registers */
        ldr r2, =0x00000241     /* memory clock: 100MHz,
				   nomal core clock: 200MHz,
				   turbo mode: 400MHz */
        str r2, [r1, #CLK_CCCR]

set_core_clock:		/* PXA250 Core Clock is set to Turbo Mode */
        ldr r0,=0
        mrc p14,0,r0,c6,c0,0
        ldr r0,=3
        mcr p14,0,r0,c6,c0,0

set_os_timer:		/* OS Timer initialization */
        ldr r1, =TMR_BASE	/* reset the OS Timer Count to zero */
        mov r2, #0
        str r2, [r1, #TMR_OSCR]
        ldr r4, =0x300	/* really 0x2E1 (0.27uS * 0x2E1 = 198uS) is about 200usec,   */
			/* so 0x300 should be plenty :	Wait OS Timer stabilize */
1:
        ldr r2, [r1, #TMR_OSCR]
        cmp r4, r2
        bgt 1b

init_gpio:
        // FFUART

        ldr r12, =FFUART_BASE

        ldr r0, =0x00000000
        str r0, [r12, #FFLCR]
        ldr r0, =0x00000000
        str r0, [r12, #FFIER]
        ldr r0, =0x00000080
        str r0, [r12, #FFLCR]
        ldr r0, =0x00000018
        str r0, [r12, #FFDLL]
        ldr r0, =0x00000000
        str r0, [r12, #FFDLH]
        ldr r0, =0x00000000
        str r0, [r12, #FFLCR]
        ldr r0, =0x00000003
        str r0, [r12, #FFLCR]
        ldr r0, =0x00000001
        str r0, [r12, #FFFCR]
        ldr r0, =0x00000007
        str r0, [r12, #FFFCR]
        ldr r0, =0x00000000
        str r0, [r12, #FFIER]
        ldr r0, =0x00000000
        str r0, [r12, #FFMCR]

        ldr r0, =FFISR_VALUE
        str r0, [r12, #FFISR]

        ldr r12, =FFUART_BASE
        ldr r0, =0x00000040
        str r0, [r12, #FFIER]

        // clear the rx fifo errors
        ldr r0, =FFFCR_VALUE
        and r0, r0, #0x00000002
        str r0, [r12, #FFFCR]

        // First set the output values to a safe/disabled state before we change
        // any GPIO's outputs
        // start by settings all of them high which is the safest for most signals

        ldr r12, =GPIO_BASE

        ldr r0, =0xffffffff
        str r0, [r12, #GPIO_GPSR0]
        str r0, [r12, #GPIO_GPSR1]
        str r0, [r12, #GPIO_GPSR2]

        // GPCR0
        // Now clear any high true outputs that need to start low
        ldr r0, =(GPIO_7 | GPIO_13 | GPIO_17 | GPIO_27)	// USB speed = high
							// PRST = deasserted
							// BKL_ON
							// Passive LCD enable = off

        str r0, [r12, #GPIO_GPCR0]

        // Next, set the correct direction for out functions. A one meas output.
        ldr r0, =(GPIO_3 | GPIO_4 | GPIO_6  | GPIO_7 | GPIO_11 | GPIO_13 | GPIO_15 | GPIO_17 | GPIO_23 | GPIO_24 | GPIO_25 | GPIO_27 | GPIO_30 | GPIO_31)

        // GPDR0
        str r0, [r12, #GPIO_GPDR0]

        ldr r0, =(GPIO_32 | GPIO_33 | GPIO_39 | GPIO_40 | GPIO_41 | GPIO_43 | GPIO_45 | GPIO_47  | GPIO_48 | GPIO_49 | GPIO_50 | GPIO_51 | GPIO_52 | GPIO_53 | GPIO_54 | GPIO_55 | GPIO_58 | GPIO_59 | GPIO_60 | GPIO_61 | GPIO_62 | GPIO_63)

        //GPDR1
        str r0, [r12, #GPIO_GPDR1]

        ldr r0, =(GPIO_64 | GPIO_65 | GPIO_66 | GPIO_67 | GPIO_68 | GPIO_69 | GPIO_70 | GPIO_71 | GPIO_72 | GPIO_73 | GPIO_74 | GPIO_75 | GPIO_76 | GPIO_77 | GPIO_78 | GPIO_79 | GPIO_80 | GPIO_81 | GPIO_84)

        // GPDR2
        str r0, [r12, #GPIO_GPDR2]

        // Finally, set the alternate function registers to the correct state
        ldr r0, =GPIO_15_AF_CS1	;

        str r0, [r12, #GPIO_GAFR0L]

        ldr r0, =(GPIO_18_AF_RDY | GPIO_23_AF_SSP_CLK | GPIO_24_AF_SSP_FRM | GPIO_25_AF_SSP_TXD | GPIO_26_AF_SSP_RXD | GPIO_28_AF_AC97_BCLK_IN | GPIO_29_AF_AC97_SDIN0 | GPIO_30_AF_AC97_SDOUT | GPIO_31_AF_AC97_SYNC)

        str r0, [r12, #GPIO_GAFR0U]

        ldr r0, =(GPIO_33_AF_CS5 | GPIO_34_AF_FF_RXD | GPIO_35_AF_FF_CTS | GPIO_36_AF_FF_DCD | GPIO_37_AF_FF_DSR | GPIO_38_AF_FF_RI | GPIO_39_AF_FF_TXD | GPIO_40_AF_FF_DTR | GPIO_41_AF_FF_RTS | GPIO_42_AF_BT_RXD | GPIO_43_AF_BT_TXD | GPIO_44_AF_BT_CTS | GPIO_45_AF_BT_RTS | GPIO_46_AF_IR_RXD | GPIO_47_AF_IR_TXD)

        str r0, [r12, #GPIO_GAFR1L]

        ldr r0, =(GPIO_48_AF_POE | GPIO_49_AF_PWE | GPIO_50_AF_PIOR | GPIO_51_AF_PIOW | GPIO_52_AF_PCE1 | GPIO_53_AF_PCE2 | GPIO_54_AF_PSKTSEL | GPIO_55_AF_PREG | GPIO_56_AF_PWAIT | GPIO_57_AF_IOIS16 | GPIO_58_AF_LDD0 | GPIO_59_AF_LDD1 | GPIO_60_AF_LDD2 | GPIO_61_AF_LDD3 | GPIO_62_AF_LDD4 | GPIO_63_AF_LDD5)

        str r0, [r12, #GPIO_GAFR1U]

        ldr r0, =(GPIO_64_AF_LDD6 | GPIO_65_AF_LDD7 | GPIO_66_AF_LDD8 | GPIO_67_AF_LDD9 | GPIO_68_AF_LDD10 | GPIO_69_AF_LDD11 | GPIO_70_AF_LDD12 | GPIO_71_AF_LDD13 | GPIO_72_AF_LDD14 | GPIO_73_AF_LDD15 | GPIO_74_AF_LCD_FCLK | GPIO_75_AF_LCD_LCLK | GPIO_76_AF_LCD_PCLK | GPIO_77_AF_LCD_BIAS | GPIO_78_AF_CS2 | GPIO_79_AF_CS3)

        str r0, [r12, #GPIO_GAFR2L]

        ldr r0, =GPIO_80_AF_CS4

        str r0, [r12, #GPIO_GAFR2U]

        // Clear the peripheral control registers bits
        ldr r12, =PSSR
        ldr r0, =(PSSR_RDH | PSSR_PH)
        str r0, [r12]

init_mem_ctrl:		/* PXA250 Memory Controller initialization */
        ldr r1, =MSC_BASE	/* base of memory controller */

        /* static memory controller initialization */

        ldr r2, =0x123412c0	/* CS0 : flash , CS1 : SMSC LAN91C1111  */
        str r2, [r1, #MSC_MSC0]
        ldr r3, [r1, #MSC_MSC0]

        ldr r2, =0x3ff923e9	/* CS2 : TD242(USB2.0),
				   CS3 : extended board (FPGA board, iom board) */
        str r2, [r1, #MSC_MSC1]
        ldr r3, [r1, #MSC_MSC1]

        ldr r2, =0x3ff97ff9	/* CS4 : SL811HS(USB1.0) , CS5 : KEY PAD  */
        str r2, [r1, #MSC_MSC2]
        ldr r3, [r1, #MSC_MSC2]

        /* MECR: Memory Expansion Card Register */
        ldr r2, =CFG_MECR_VAL
        str r2, [r1, #MSC_MECR]
        ldr r2, [r1, #MSC_MECR]

        /* MCMEM0: Card Interface slot 0 timing */
        ldr r2, =CFG_MCMEM0_VAL
        str r2, [r1, #MSC_MCMEM0]
        ldr r2, [r1, #MSC_MCMEM0]

        /* MCMEM1: Card Interface slot 1 timing */
        ldr r2, =CFG_MCMEM1_VAL
        str r2, [r1, #MSC_MCMEM1]
        ldr r2, [r1, #MSC_MCMEM1]

        /* MCATT0: Card Interface Attribute Space Timing, slot 0 */
        ldr r2, =CFG_MCATT0_VAL
        str r2, [r1, #MSC_MCATT0]
        ldr r2, [r1, #MSC_MCATT0]

        /* MCATT1: Card Interface Attribute Space Timing, slot 1 */
        ldr r2, =CFG_MCATT1_VAL
        str r2, [r1, #MSC_MCATT1]
        ldr r2, [r1, #MSC_MCATT1]

        /* MCIO0: Card Interface I/O Space Timing, slot 0 */
        ldr r2, =CFG_MCIO0_VAL
        str r2, [r1, #MSC_MCIO0]
        ldr r2, [r1, #MSC_MCIO0]

        /* MCIO1: Card Interface I/O Space Timing, slot 1 */
        ldr r2, =CFG_MCIO1_VAL
        str r2, [r1, #MSC_MCIO1]
        ldr r2, [r1, #MSC_MCIO1]

        /* SDRAM Controller initialized at 100MHz */
        ldr r2, =0x03c00fff		/* build MDREFR in a specific order */
        str r2, [r1, #MSC_MDREFR]

        ldr r2, =0x03c00018		/* REF Rate = 
					     (64MS/8192 Rows) * 100MHz /32 = 24 */
        str r2, [r1, #MSC_MDREFR]

        ldr r2, =0x03c52018		/* Set K0RUN, K1RUN and K2RUN */
        str r2, [r1, #MSC_MDREFR]

        ldr r2, =0x03852018		/* Clear Self Refresh */
        str r2, [r1, #MSC_MDREFR]

        ldr r2, =0x0385b018		/* Set E0PIN and E1PIN */
        str r2, [r1, #MSC_MDREFR]

        ldr r2, =0x0ac80ac8		/* Set CNFG, but don't enable just yet,
					   CL = 3, tRP = 3clk, tRC = 10clk,
					   tRAS = 7clk, tRCD = 3clk */
        str r2, [r1, #MSC_MDCNFG]

        /* SDRAM Controller initialized at 50MHz. */
        /* pause for ~200usec for SDRAM to init */
        ldr r1, =TMR_BASE		/* reset the OS Timer Count to zero */
        mov r2, #0
        str r2, [r1, #TMR_OSCR]
        ldr r4, =0x300			/* really 0x2E1 (0.27uS * 0x2E1 = 198uS) 
					   is about 200usec, */
	                                /* so 0x300 should be plenty */
1:
        ldr r2, [r1, #TMR_OSCR]
        cmp r4, r2
        bgt 1b

        /* force 9 cycles to SDRAM : SDRAM bank all in the CBR (refrash cycle) */
        ldr r2, =SDRAM_BASE_ADDRESS
        str r2, [r2]
        str r2, [r2]
        str r2, [r2]
        str r2, [r2]
        str r2, [r2]
        str r2, [r2]
        str r2, [r2]
        str r2, [r2]
        str r2, [r2]

        ldr r1, =MSC_BASE		/* base of memory controller */

        ldr r2, =0x0acb0acb		/* now enable SDRAM */
        str r2, [r1, #MSC_MDCNFG]

        /* and do an MRS */
        ldr r2, =0x00000000
        str r2, [r1, #MSC_MDMRS]

        /* sdram initialize done */

set_stack_pointer:
        /* FIQ mode */
        mrs r0, cpsr		/* move CPSR to r0 */
        bic r0, r0, #0x1f	/* clear all mode bits */
        orr r0, r0, #0xd1	/* set FIQ mode bits */
        msr CPSR_c, r0		/* move back to CPSR */
        ldr sp, =(fiq_stack + FIQ_STACK_SIZE - 4)	/* initialize the stack ptr */
        /* IRQ mode */
        mrs r0, cpsr		/* move CPSR to r0 */
        bic r0, r0, #0x1f	/* clear all mode bits */
        orr r0, r0, #0xd2	/* set IRQ mode bits */
        msr CPSR_c, r0		/* move back to CPSR */
        ldr sp, =(irq_stack + IRQ_STACK_SIZE - 4)	/* initialize the stack ptr */
        /* Abort mode */
        mrs r0, cpsr		/* move CPSR to r0 */
        bic r0, r0, #0x1f	/* clear all mode bits */
        orr r0, r0, #0xd7	/* set Abort mode bits */
        msr CPSR_c, r0		/* move back to CPSR */
        ldr sp, =(abt_stack + ABT_STACK_SIZE - 4)	/* initialize the stack ptr */
        /* Undef mode */
        mrs r0, cpsr		/* move CPSR to r0 */
        bic r0, r0, #0x1f	/* clear all mode bits */
        orr r0, r0, #0xdb	/* set Undef mode bits */
        msr CPSR_c, r0		/* move back to CPSR */
        ldr sp, =(und_stack + UND_STACK_SIZE - 4)	/* initialize the stack ptr */
        /* System mode */
        mrs r0, cpsr		/* move CPSR to r0 */
        bic r0, r0, #0x1f	/* clear all mode bits */
        orr r0, r0, #0xdf	/* set System mode bits */
        msr CPSR_c, r0		/* move back to CPSR */
        ldr sp, =(sys_stack + SYS_STACK_SIZE - 4)	/* initialize the stack ptr */


.align 4
set_svc_mode:
	/* Change (back) to SVC mode */
	mrs r0, cpsr		/* move CPSR to r0 */
	bic r0, r0, #0x1f	/* clear all mode bits */
	orr r0, r0, #0xd3	/* set System mode bits */
	msr CPSR_c, r0		/* move back to CPSR */
	/* Reset the stack pointer for the SVC mode (our current mode) */
	ldr sp, =(0xa4000000 - 4) ;
	
	/*
	 * trampoline - jump to C function main().
	 */
.align 4
relocate:
	adr r0, _start

	// relocate the second stage loader
	add r2, r0, #(1024 * 1024)
	ldr r1, =0xa0000000

	/* r0 = source address
	 * r1 = target address
	 * r2 = source end address
	 */
copy_loop:
	ldmia   r0!, {r3-r10}
	stmia   r1!, {r3-r10}
	cmp     r0, r2

	ble     copy_loop

jump_to_main:
	bl main

	/* the C code should never return */
	b _start
.align 4
