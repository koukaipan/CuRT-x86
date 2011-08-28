/**
 * @file    include/arch/x86/apic_addr.h
 * @author  Ying-Shiuan Pan
 * @brief   APIC specific address
 */

#ifndef __APIC_ADDR_H__
#define __APIC_ADDR_H__

/****************************************************************************
*						CONSTANTS
*****************************************************************************/
#define APIC_BASE_ADDR		(0xFEE00000)
#define APIC_Timer_ADDR		(APIC_BASE_ADDR + 0x320)
#define APIC_TimerDIV_ADDR	(APIC_BASE_ADDR + 0x3E0)
#define APIC_TimerINIT_ADDR	(APIC_BASE_ADDR + 0x380)
#define APIC_TimerCURR_ADDR	(APIC_BASE_ADDR + 0x390)
#define APIC_SPURINT_ADDR	(APIC_BASE_ADDR + 0xF0)
#define APIC_EOI_ADDR		(APIC_BASE_ADDR + 0xB0)

#define APIC_LINT0_ADDR		(APIC_BASE_ADDR + 0x350)
#define APIC_LINT1_ADDR		(APIC_BASE_ADDR + 0x360)
#define APIC_LERROR_ADDR	(APIC_BASE_ADDR + 0x370)
#define APIC_PERFCNT_ADDR	(APIC_BASE_ADDR + 0x340)
#define APIC_THERMAL_ADDR	(APIC_BASE_ADDR + 0x330)

#endif  //_APIC_ADDR_H_ 
