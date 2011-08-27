/*
 * kernel.c
 */

#include "lib/stdio.h"
#include "port.h"
#include "kernel/kernel.h"
#include "kernel/list.h"
#include "kernel/thread.h"

/* the priority of system-wide idle thread */
#define IDLE_THREAD_PRIO 31   

/* list of priority for ready threads */
list_t ready_list[MAX_PRIO];  

/* The waiting list is being requested by the end of the executing threads.
 * It can not be completely removed by the idle thread. */
list_t termination_wait_list; 

/* list for threads requesting delay, whose status is soon to change. */
list_t delayed_list;          

/* Requesting blocked by a thread.  Once thread_resume, status returns back. */
list_t blocked_list;

/* Pointers to the currently executing thread */
thread_struct *current_thread;

/* Pointer to the executive running the context during transition. */
thread_struct *next_thread;    

/* OS wide timer incremented by tick. */
int os_time_tick;    

int current_top_prio;
bool prio_exist_flag[MAX_PRIO];
bool is_start_os;

/* Nested interrupts */
int interrupt_nesting;

/* context switching */
int total_csw_cnt;
int total_thread_cnt;

thread_struct *thread_table[MAX_THREAD];

/* idle thread */
thread_struct idle_thread;
stk_t idle_thread_stk[THREAD_STACK_SIZE];

void start_curt()
{
	cpu_sr_t cpu_sr;
	list_node_t *pnode;
	int top_prio;

	cpu_sr = save_cpu_sr();
	is_start_os = true;
	/* examine the highest priority thread executed */
	top_prio = get_top_prio();
	pnode = delete_front_list(&ready_list[top_prio]);
	if (is_empty_list(&ready_list[top_prio]))
		prio_exist_flag[top_prio] = false;
	current_thread = entry_list(pnode, thread_struct, node);
	current_thread->state = RUNNING;
	restore_cpu_sr(cpu_sr);
	restore_context();
}

tid_t get_tid()
{
	int i;
	for (i = 0; i < MAX_THREAD; i++) {
		if (thread_table[i] == NULL)
			return i;
	}
	return RET_ERR;
}

u8_t get_top_prio()
{
	int i;
	for( i = 0; i < MAX_PRIO; i++ ) {
		if( prio_exist_flag[i] == true )
			return i;
	}
	return MAX_PRIO;
}

void init_curt()
{
	int i;
	for (i = 0; i < MAX_PRIO; i++) {
		prio_exist_flag[i] = false;
		init_list(&ready_list[i]);
	}
	init_list(&delayed_list);
	init_list(&blocked_list);
	init_list(&termination_wait_list);

	for (i = 0; i < MAX_THREAD; i++)
		thread_table[i] = NULL;

	is_start_os = false;
	interrupt_nesting = 0;
	os_time_tick = 0;
	current_top_prio = MAX_PRIO;
	total_csw_cnt = 0;
	total_thread_cnt = 0;
	current_thread = NULL;
	next_thread = NULL;

	/* create idle thread internally */
	thread_create(&idle_thread,
			&idle_thread_stk[THREAD_STACK_SIZE-1],
			idle_thread_func,
			"idle-thread",
			IDLE_THREAD_PRIO,
			NULL);
}


/**
 * @brief Invoke the scheduler
 * The scheduler is called in the following two cases:
 *   - thread's time_quantum value is 0 (SCHED_TIME_EXPIRE) and then stop.
 *   - Request the execution for higher priority thread (SCHED_THREAD_REQUEST).
 */
void schedule(SCHED_TYPE sched_type)
{
	int top_prio;
	cpu_sr_t cpu_sr;
	list_node_t *pnode;

	cpu_sr = save_cpu_sr();
	top_prio = get_top_prio();
	/* If the timer expires... */
	if (sched_type == SCHED_TIME_EXPIRE) {
		/* Threads that are currently running will continue to run it
		 * at the highest priority */
		if (current_thread->prio < top_prio) {
			current_thread->time_quantum = TIME_QUANTUM;
			restore_cpu_sr(cpu_sr);
			return;
		}
		/* otherwise, threads in a ready state, then run the highest
		 * priority one. */
		pnode = delete_front_list(&ready_list[top_prio]);
		if (is_empty_list(&ready_list[top_prio]))
			prio_exist_flag[top_prio] = false;
		next_thread = entry_list(pnode, thread_struct, node);
		next_thread->state = RUNNING;
		next_thread->time_quantum = TIME_QUANTUM;

		/* Ready to change the status of the currently executing thread */
		current_thread->state = READY;
		insert_back_list(&ready_list[current_thread->prio],
		                 &current_thread->node);
		prio_exist_flag[current_thread->prio] = true;
		total_csw_cnt++;
		/* actual context switching */
		context_switch_in_interrupt();
	}
	/* requested by a thread */
	else if (sched_type == SCHED_THREAD_REQUEST ) {
		/* Threads that are currently running will continue to run it
		 * at the highest priority. */
		if (current_thread->state == RUNNING &&
		    current_thread->prio < top_prio ) {
			current_thread->time_quantum = TIME_QUANTUM;
			restore_cpu_sr(cpu_sr);
			return;
		}
		/* otherwise, threads in a ready state, then run the highest
		 * priority one. */
		pnode = delete_front_list(&ready_list[top_prio]);
		if (is_empty_list(&ready_list[top_prio]))
			prio_exist_flag[top_prio] = false;
		next_thread = entry_list(pnode, thread_struct, node);
		next_thread->state = RUNNING;
		next_thread->time_quantum = TIME_QUANTUM;

		/* Ready to change the status of the currently executing thread */
		if (current_thread->state == RUNNING) {
			current_thread->state = READY;
			insert_back_list(&ready_list[current_thread->prio],
			                 &current_thread->node);
			prio_exist_flag[current_thread->prio] = true;
		}
		total_csw_cnt++;
		/* context switching */
		context_switch();
	}
	restore_cpu_sr(cpu_sr);
}

/**
 * @brief time tick advanced
 * Maintain time_quantum
 */
void advance_time_tick()
{
	cpu_sr_t cpu_sr;
	list_node_t *pnode;
	thread_struct *pthread;
	thread_struct *readyed_thread = NULL;

	cpu_sr = save_cpu_sr();
	os_time_tick++;
	/* If there are delays in the list of threads... */
	if (!is_empty_list(&delayed_list)) {
		for (pnode = begin_list(&delayed_list);
		     pnode != end_list(&delayed_list);
		     pnode = next_list(pnode) ) {
			pthread = entry_list(pnode, thread_struct, node);
			pthread->delayed_time--;
			/* ready to change the status */
			if (readyed_thread != NULL) {
				delete_list(&readyed_thread->node);
				readyed_thread->state = READY;
				readyed_thread->time_quantum = TIME_QUANTUM;
				insert_back_list(
					&ready_list[readyed_thread->prio],
					&readyed_thread->node);
				prio_exist_flag[readyed_thread->prio] = true;
				readyed_thread = NULL;
			}
			if (pthread->delayed_time <= 0) {
				readyed_thread = pthread;
			}
		}
		if (readyed_thread != NULL) {
			delete_list(&readyed_thread->node);
			readyed_thread->state = READY;
			readyed_thread->time_quantum = TIME_QUANTUM;
			insert_back_list(
				&ready_list[readyed_thread->prio],
				&readyed_thread->node);
			prio_exist_flag[readyed_thread->prio] = true;
		}
	}
	current_thread->time_quantum--;
	restore_cpu_sr(cpu_sr);
}

/**
 * UNIMPLEMNTED
 */
void enter_interrupt()
{
}

void exit_interrupt()
{
	if (interrupt_nesting > 0) 
		interrupt_nesting--;

	if (current_thread->time_quantum <= 0) {
		schedule(SCHED_TIME_EXPIRE);
	}
}

/**
 * @brief rountine for idle thread
 * @param data
 */
void idle_thread_func(void *data)
{
	cpu_sr_t cpu_sr;
	thread_struct *pthread;
	list_node_t *pnode;

	while (1) {
		cpu_sr = save_cpu_sr();
		if (!is_empty_list(&termination_wait_list)) {
			pnode = delete_front_list(&termination_wait_list);
			pthread = entry_list(pnode, thread_struct, node);
			thread_table[pthread->tid] = NULL;
			total_thread_cnt--;
		}
		restore_cpu_sr(cpu_sr);
		/* always does this */
		thread_yield();
	}
}

