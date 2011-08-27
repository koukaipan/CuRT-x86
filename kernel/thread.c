/*
 * thread.c
 */

#include "kernel/thread.h"
#include "kernel/kernel.h"
#include "kernel/list.h"
#include "kernel/types.h"
#include "port.h"


/**
 * @brief Create thread
 *
 * @param thread - thread of the information contained threads structure
 * @param thread_stk - Created a pointer to the thread stack space
 * @param func - Generated a thread of the function
 * @param name - Thread name
 * @param prio - The priority of threads
 * @param pdata - Pass parameters to the function of a thread running
 * @retval RET_NO_ERR
 * @retval RET_ERR
 */     
tid_t thread_create(thread_struct *thread,
		    stk_t *thread_stk,
		    THREAD_FUNC func,
		    char *name,
		    u8_t prio,
		    void *pdata)
{
	cpu_sr_t cpu_sr;
	stk_t *pstk;
	thread_struct *pthread;
	tid_t tid;

	if (thread == NULL || thread_stk == NULL || func == NULL ||
	    name == NULL || (prio >= MAX_PRIO))
		return RET_ERR;

	pstk = thread_stk;
	pthread = thread;

	/* no failback */
	if ((tid = get_tid()) == RET_ERR) {
		return RET_ERR;
	}

	/* constrct thread_struct */
	pthread->tid = tid;
	pthread->stack_ptr = init_thread_stack(func, pdata, pstk);
	pthread->name = name;
	pthread->prio = prio;
	pthread->time_quantum = TIME_QUANTUM;
	pthread->delayed_time = 0;
	pthread->state = READY;

	cpu_sr = save_cpu_sr();
	thread_table[tid] = pthread;
	prio_exist_flag[prio] = true;
	total_thread_cnt++;
	insert_back_list(&ready_list[prio], &pthread->node);
	restore_cpu_sr(cpu_sr);

	/* if priority higher than existing thread, invoke the scheduler. */
	if (is_start_os == true && current_thread->prio > prio) {
		schedule(SCHED_THREAD_REQUEST);
	}
	return tid;
}

err_t thread_delete(tid_t tid)
{
	cpu_sr_t cpu_sr;

	if (tid < IDLE_THREAD_TID || tid > MAX_THREAD)
		return RET_ERR;

	if (thread_table[tid] == NULL)
		return RET_ERR;

	cpu_sr = save_cpu_sr();
	if (tid == current_thread->tid) {
		current_thread->state = TERMINATE;
		insert_back_list(&termination_wait_list, &current_thread->node); 
		restore_cpu_sr(cpu_sr);
		schedule(SCHED_THREAD_REQUEST);
		return RET_NO_ERR;
	}
	return RET_ERR;
}

void thread_yield()
{
	schedule(SCHED_THREAD_REQUEST);
}

err_t thread_delay(u32_t tick)
{
	cpu_sr_t cpu_sr;

	if (tick > 0) { /* Delay must be greater than 0. */
		cpu_sr = save_cpu_sr();
		current_thread->state = DELAY;
		current_thread->delayed_time = tick;
		insert_back_list(&delayed_list, &current_thread->node);
		restore_cpu_sr(cpu_sr);
		schedule(SCHED_THREAD_REQUEST);
		return RET_NO_ERR;
	}
	return RET_ERR;
}

err_t thread_suspend(tid_t tid)
{
	cpu_sr_t cpu_sr;
	thread_struct *pthread;

	if (tid < IDLE_THREAD_TID || tid > MAX_THREAD)
		return RET_ERR;

	if (thread_table[tid] == NULL)
		return RET_ERR;

	cpu_sr = save_cpu_sr();
	pthread = thread_table[tid];
	if (current_thread->tid == tid) {
		pthread->state = BLOCK;
		insert_back_list(&blocked_list, &pthread->node);
		restore_cpu_sr(cpu_sr);
		schedule(SCHED_THREAD_REQUEST);
		return RET_NO_ERR;
	}
	else if (pthread->prio > current_thread->prio &&
	         pthread->state == READY) {
		pthread->state = BLOCK;
		delete_list(&pthread->node);
		insert_back_list(&blocked_list, &pthread->node);
		restore_cpu_sr(cpu_sr);
		schedule(SCHED_THREAD_REQUEST);
		return RET_NO_ERR;
	}
	restore_cpu_sr(cpu_sr);
	return RET_ERR;
}

err_t thread_resume(tid_t tid)
{
	cpu_sr_t cpu_sr;
	thread_struct *pthread;

	if (tid < IDLE_THREAD_TID || tid > MAX_THREAD)
		return RET_ERR;

	if (thread_table[tid] == NULL)
		return RET_ERR;

	cpu_sr = save_cpu_sr();
	pthread = thread_table[tid];
	if (pthread->state == BLOCK) {
		pthread->state = READY;
		delete_list(&pthread->node);
		insert_back_list(&ready_list[pthread->prio], &pthread->node);
		prio_exist_flag[pthread->prio] = true;
		restore_cpu_sr(cpu_sr);
		schedule(SCHED_THREAD_REQUEST);
		return RET_NO_ERR;
	}
	restore_cpu_sr(cpu_sr);
	return RET_ERR;
}

