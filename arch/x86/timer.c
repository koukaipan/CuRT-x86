/**
 * @file    arch/x86/timer.c
 * @author  
 * @brief   timer interrupt operation
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
 * 
 * Notes: No warranty expressed or implied. Use at own risk. 
 */

#include "irq.h"
#include "x86.h"
#include "kernel/kernel.h"
#include "kernel/console.h"
/* This will keep track of how many ticks that the system
*  has been running for */
static int _g_timer_ticks = 0;

/**
 * @brief timer interrupt handler
 * @param r   register snapshots
 * 
 * Handles the timer. In this case, it's very simple: We
 * increment the 'timer_ticks' variable every time the
 * timer fires. By default, the timer fires 18.222 times
 * per second. Why 18.222Hz? Some engineer at IBM must've
 * been smoking something funky 
 * 
 * note: it should be called by irq_handler
 */
void timer_handler(struct regs *r)
{
    /* Increment our 'tick count' */
    _g_timer_ticks++;
    advance_time_tick();
}

/** 
 * @brief current ticks
 * @return current ticks
 */
int timer_get_tick()
{
	return _g_timer_ticks;
}

/** 
 * @brief wait
 * @param how many ticks should we wait for
 * 
 * This will continuously loop until the given time has
 * been reached
 */
void timer_wait(int ticks)
{
    unsigned long eticks;

    eticks = _g_timer_ticks + ticks;
    while(_g_timer_ticks < eticks);
}

/** 
 * @brief install timer interrupt handler to IRQ0
 * 
 * Sets up the system clock by installing the timer handler
 * into IRQ0 
 */
void timer_install()
{
    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);
    _g_timer_ticks = 0;
}
