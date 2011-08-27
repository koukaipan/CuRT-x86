/*
 * ipc.c
 */

#include "kernel/ipc.h"
#include "kernel/list.h"
#include "kernel/thread.h"
#include "kernel/kernel.h"
#include "port.h"

/**
 * @param mbox - message box structure
 * @param pmsg - 
 * @retval RET_NO_ERR
 * @retval RET_ERR
 */
err_t mbox_init(mbox_struct *mbox, void *pmsg)
{
	/* Validation */
	if (mbox == NULL)
		return RET_NO_ERR;

	/* initialization */
	mbox->msg = pmsg;
	init_list(&mbox->wait_list);
	return RET_NO_ERR;
}

/**
 * obtain a message if something waiting in wait_list.
 * @param mbox
 */
void *mbox_recv(mbox_struct *mbox)
{
	cpu_sr_t cpu_sr;
	list_node_t *pnode;
	thread_struct *pthread;

	cpu_sr = save_cpu_sr();
	void *pmsg = mbox->msg;
	/* incoming message in mbox */
	if (pmsg != (void *) 0) {
		mbox->msg = (void *) 0;
		restore_cpu_sr(cpu_sr);
		return pmsg;
	}
	/* if there is no message, then wait. */
	if (!is_empty_list(&mbox->wait_list)) {
		for (pnode = begin_list(&mbox->wait_list);
		     pnode != end_list(&mbox->wait_list);
		     pnode = next_list(pnode)) {
			pthread = entry_list(pnode, thread_struct, node);
			if (current_thread->prio < pthread->prio) {
				current_thread->state = EVENT_WAIT;
				insert_before_list(pnode, &current_thread->node);
				break;
			}
		}
	}
	if (current_thread->state != EVENT_WAIT) {
		current_thread->state = EVENT_WAIT;
		insert_back_list(&mbox->wait_list, &current_thread->node);
	}
	restore_cpu_sr(cpu_sr);
	schedule(SCHED_THREAD_REQUEST);

	cpu_sr = save_cpu_sr();
	pmsg = current_thread->msg;
	restore_cpu_sr();
	return pmsg;
}

/**
 * @brief Attempt to receive messages.
 * @retval NULL
 * @retval !NULL
 */
void *mbox_try_recv(mbox_struct *mbox)
{
	cpu_sr_t cpu_sr;
	void *pmsg;

	cpu_sr = save_cpu_sr();
	pmsg = mbox->msg;
	mbox->msg = (void *) 0;
	restore_cpu_sr(cpu_sr);
	return pmsg;
}

/**
 * @param mbox
 * @retval RET_NO_ERR
 * @retval RET_ERR
 */
err_t mbox_send(mbox_struct *mbox, void *pmsg)
{
	cpu_sr_t cpu_sr;
	thread_struct *pthread;

	cpu_sr = save_cpu_sr();
	/* If threads are waiting... */
	if (!is_empty_list(&mbox->wait_list)) {
		/* find out Threads of the highest priority */
		pthread = entry_list(delete_front_list(&mbox->wait_list),
		                     thread_struct, node);
		pthread->msg = pmsg;
		pthread->state = READY;
		insert_back_list(&ready_list[pthread->prio], &pthread->node);
		prio_exist_flag[pthread->prio] = true;
		restore_cpu_sr(cpu_sr);
		schedule(SCHED_THREAD_REQUEST);
		return RET_NO_ERR;
	}

	/* If there is no pending threads / there is no message to mbox */
	if (mbox->msg == (void *) 0) {
		mbox->msg = pmsg;
		restore_cpu_sr(cpu_sr);
		return RET_NO_ERR;
	}

	/* already have a message */
	restore_cpu_sr(cpu_sr);
	return RET_ERR;
}

/**
 * @brief Initialize the message queue
 * @retval RET_NO_ERR
 * @retval RET_ERR
 */
err_t msgq_init(msgq_struct *msgq)
{
	if (msgq == NULL)
		return RET_ERR;

	init_list(&msgq->msg_list);
	init_list(&msgq->wait_list);
	return RET_NO_ERR;
}

/**
 * @brief receive from message queue
 * @param msgq
 * @retval
 */
void *msgq_recv(msgq_struct *msgq)
{
	cpu_sr_t cpu_sr;
	list_node_t *pnode;
	thread_struct *pthread;
	msg_struct *spmsg;
	void *pmsg;

	cpu_sr = save_cpu_sr();
	if (!is_empty_list(&msgq->msg_list)) {
		pnode = delete_front_list(&msgq->msg_list);
		spmsg = entry_list(pnode, msg_struct, node);
		restore_cpu_sr(cpu_sr);
		return spmsg->msg;
	}
	if (!is_empty_list(&msgq->wait_list)) {
		for (pnode = begin_list(&msgq->wait_list);
		     pnode != end_list(&msgq->wait_list);
		     pnode = next_list(pnode) ) {
			pthread = entry_list(pnode, thread_struct, node);
			if (current_thread->prio < pthread->prio) {
				current_thread->state = EVENT_WAIT;
				insert_before_list(pnode, &current_thread->node);
				break;
			}
		}
	}
	if (current_thread->state != EVENT_WAIT) {
		current_thread->state = EVENT_WAIT;
		insert_back_list(&msgq->wait_list, &current_thread->node);
	}
	restore_cpu_sr(cpu_sr);
	schedule(SCHED_THREAD_REQUEST);
	cpu_sr = save_cpu_sr();
	pmsg = current_thread->msg;
	restore_cpu_sr();
	return pmsg;
}

/**
 * @brief attempt to receive from message queue
 * @param msgq
 * @retval NULL
 * @retval !NULL
 */
void *msgq_try_recv(msgq_struct *msgq)
{
	cpu_sr_t cpu_sr;
	list_node_t *pnode;
	msg_struct *spmsg;

	cpu_sr = save_cpu_sr();
	if (!is_empty_list(&msgq->msg_list)) {
		pnode = delete_front_list(&msgq->msg_list);
		spmsg = entry_list(pnode, msg_struct, node);
		restore_cpu_sr(cpu_sr);
		return spmsg->msg;
	}
	return NULL;
}

/**
 * @brief send message to message queue
 * @param msgq
 * @param spmsg
 * @retval RET_NO_ERR
 * @retval RET_ERR
 */
err_t msgq_send(msgq_struct *msgq, msg_struct *spmsg)
{
	cpu_sr_t cpu_sr;
	thread_struct *pthread;

	cpu_sr = save_cpu_sr();
	if (!is_empty_list(&msgq->wait_list)) {
		pthread = entry_list(delete_front_list(&msgq->wait_list),
		                     thread_struct, node);
		pthread->msg = spmsg->msg;
		pthread->state = READY;
		insert_back_list(&ready_list[pthread->prio], &pthread->node);
		prio_exist_flag[pthread->prio] = true;
		restore_cpu_sr(cpu_sr);
		schedule(SCHED_THREAD_REQUEST);
		return 0;
	}
	insert_back_list(&msgq->msg_list, &spmsg->node);
	restore_cpu_sr(cpu_sr);
	return 0;
}

