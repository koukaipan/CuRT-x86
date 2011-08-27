/*
 * sync.c
 */

#include "kernel/sync.h"
#include "kernel/kernel.h"
#include "kernel/thread.h"
#include "port.h"

err_t sem_init(sem_struct *sem, u8_t value)
{
	if (sem == NULL)
		return RET_ERR;

	sem->value = value;
	init_list(&sem->wait_list);
	return RET_NO_ERR;
}

void sem_pend(sem_struct *sem)
{
	cpu_sr_t cpu_sr;
	list_node_t *pnode;
	thread_struct *pthread;

	cpu_sr = save_cpu_sr();
	if (sem->value == 0) {
		if (!is_empty_list(&sem->wait_list)) {
			for (pnode = begin_list(&sem->wait_list);
			     pnode != end_list(&sem->wait_list); 
			     pnode = next_list(pnode)) {
				pthread = entry_list(pnode, thread_struct, node);
				if (current_thread->prio < pthread->prio) {
					current_thread->state = EVENT_WAIT;
					insert_before_list(
						pnode,
						&current_thread->node);
					break;
				}
			}
		}
		if (current_thread->state != EVENT_WAIT) {
			current_thread->state = EVENT_WAIT;
			insert_back_list(&sem->wait_list, &current_thread->node);
		}
		schedule(SCHED_THREAD_REQUEST);
		return;
	}
	sem->value--;
	restore_cpu_sr(cpu_sr);
}

err_t sem_try_pend(sem_struct *sem)
{
	cpu_sr_t cpu_sr;

	cpu_sr = save_cpu_sr();
	if (sem->value > 0) {
		sem->value--;
		restore_cpu_sr(cpu_sr);
		return RET_NO_ERR;
	}
	else {
		restore_cpu_sr(cpu_sr);
		return RET_ERR;
	}
}

void sem_post(sem_struct *sem)
{
	thread_struct *pthread;
	cpu_sr_t cpu_sr;

	cpu_sr = save_cpu_sr();
	if (!is_empty_list(&sem->wait_list)) {
		pthread = entry_list(delete_front_list(&sem->wait_list),
		                     thread_struct, node);
		pthread->state = READY;
		insert_back_list(&ready_list[pthread->prio], &pthread->node);
		prio_exist_flag[pthread->prio] = true;
		restore_cpu_sr(cpu_sr);
		schedule(SCHED_THREAD_REQUEST);
		return;
	}
	sem->value++;
	restore_cpu_sr(cpu_sr);
}

