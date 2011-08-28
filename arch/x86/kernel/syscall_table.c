/**
 * @file syscall_table.c
 * @author Ying-Shiuan Pan koukai
 */

#include "kernel/unistd.h"
#include "x86/timer.h"

static int sys_get_ticks();

/* progress of calling a system call 
  get_tick() -> arch/x86/syscall.asm -> push syscall NR to register
  -> issue interrupt (system call number 0x90) -> save register status
  -> lookup system call table to issue *REAL* syscall method
  -> put the return value to register -> restore register status
  -> return
*/

void* sys_call_table[NR_SYSCALLS] = {
	[__NR_get_tick] = sys_get_ticks, 
	0
};

static int sys_get_ticks()
{
	return timer_get_tick();
}

