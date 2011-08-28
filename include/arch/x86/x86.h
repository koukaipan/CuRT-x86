/**
 * @file    include/arch/x86/x86.h
 * @author  Ying-Shiuan Pan
 * @brief   
 */
 
#ifndef _X86_H_
#define _X86_H_ 

/****************************************************************************
*				CONSTANTS & STRUCTURES
*****************************************************************************/
/* This defines what the stack looks like after an ISR was running */
struct regs
{
	unsigned int gs, fs, es, ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int int_no, err_code;
	unsigned int eip, cs, eflags, useresp, ss;    
};

/****************************************************************************
*				GLOBAL VARIABLES
*****************************************************************************/

/****************************************************************************
*				FUNCTION PROTOTYPES
*****************************************************************************/
void x86_init();

#include "int.h"
#define ENTER_CS()	{disable_int();}
#define EXIT_CS()	{enable_int();}

#endif
