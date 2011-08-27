/**
 * Sample shell for CuRT.
 *
 * Supported commands:
 *   ps, stat, help, clear, hello
 */

#include "lib/stdio.h"
#include "kernel/thread.h"
#include "kernel/kernel.h"
#include "kernel/sync.h"
#include "port.h"	/* inclusion specific to hardware */

static thread_struct shell_thread, stat_thread, info_thread;
static thread_struct hello_thread, hello2_thread;

static stk_t thread_stk[5][THREAD_STACK_SIZE - 1];

static void shell_thread_func(void *pdata);
static void print_statistics(void *pdata);
static void print_thread_info(void *pdata);
static inline void print_help_msg();
static void hello_world(void *pdata);

static tid_t shell_tid, stat_tid, info_tid;
static tid_t hello_tid, hello2_tid;

static sem_struct sem;

extern void SerialInit();

int main()
{
	SerialInit();
	init_interrupt_control();
	init_curt();

	sem_init(&sem, 1);

	shell_tid = thread_create(&shell_thread,
			&thread_stk[0][THREAD_STACK_SIZE-1],
			shell_thread_func,
			"shell_thread",
			5,
			NULL);
	info_tid = thread_create(&info_thread,
			&thread_stk[1][THREAD_STACK_SIZE-1],
			print_thread_info,
			"info_thread",
			1,
			NULL);

	stat_tid = thread_create(&stat_thread,
			&thread_stk[2][THREAD_STACK_SIZE-1],
			print_statistics,
			"statistics_thread",
			1,
			NULL);

	hello_tid = thread_create(&hello_thread,
			&thread_stk[3][THREAD_STACK_SIZE-1],
			hello_world,
			"hello_thread",
			21,
			NULL);

	hello2_tid = thread_create(&hello2_thread,
			&thread_stk[4][THREAD_STACK_SIZE-1],
			hello_world,
			"hello2_thread",
			21,
			NULL);

	printf("##################################\n"
	       "#       Start CuRT....           #\n"
	       "##################################\n");

	start_curt();

	/* endless loop */
	while (1)
		;

	/* Never reach here. */
	return 0;
}

static void shell_thread_func(void *pdata)
{
	init_os_timer();

	thread_suspend(info_tid);
	thread_suspend(stat_tid);
	thread_suspend(hello_tid);

	char buf[80] = { '\0' };
	while (1) {
		printf("$ ");
		gets(buf);
		printf("\n");
		if (!strcmp(buf, "")) {
			/* Do nothing */
		}
		else if (!strcmp(buf, "stat")) {
			thread_resume(stat_tid);
			thread_delay(1);
		}
		else if (!strcmp(buf, "ps")) {
			thread_resume(info_tid);
			thread_delay(1);
		}
		else if (!strcmp(buf, "help")) {
			print_help_msg();
		}
		else if (!strcmp(buf, "clear")) {
			/* FIXME: dirty trick to clean screen */
			printf("\n\n\n\n\n\n\n\n\n\n"
			       "\n\n\n\n\n\n\n\n\n\n"
			       "\n\n\n\n\n\n\n\n\n\n"
			       "\n\n\n\n\n\n\n\n\n\n"
			       "\n\n\n\n\n\n\n\n\n\n"
			       "\n\n\n\n\n\n\n\n\n\n"
			       "\n\n\n\n\n\n\n\n\n\n");
		}
		else if (!strcmp(buf, "hello")) {
			thread_resume(hello_tid);
			thread_resume(hello2_tid);
			thread_delay(1);
		}
		else {
			printf("shell: %s: command not found\n", buf);
			printf("try help\n");
		}
	}
}

static void print_statistics(void *pdata)
{
	while (1) {
		printf("*********** CuRT statistics info ***************\n");
		printf("Total Thread Count : %x\n", total_thread_cnt);
		printf("Total Context Switch Count : %x\n", total_csw_cnt);
		printf("Current Time Tick : %x\n", os_time_tick);
		printf("**************************************************\n");
		thread_suspend(current_thread->tid);
	}
}

static char *state_to_string[7] = {
	[RUNNING] 	= "Running   ",
	[READY] 	= "Ready     ",
	[DELAY] 	= "Delay     ",
	[BLOCK] 	= "Block     ",
	[TERMINATE] 	= "Terminate ",
	[EVENT_WAIT] 	= "Event wait"
};

static void print_thread_info(void *pdata)
{
	int i;
	thread_struct *pthread;

	while (1) {
		printf(" %s        %s        %s\n", "ID", "State", "Name");
		for (i = 0; i < MAX_THREAD; i++) {
			if (thread_table[i] != NULL) {
				pthread = thread_table[i];
				printf("%3x        %s   %s\n",
				       pthread->tid,
				       state_to_string[pthread->state],
				       pthread->name);
			}
		}
		thread_suspend(current_thread->tid);
	}
}

static inline void print_help_msg()
{
	printf("*********** CuRT Help Info ********************\n"
	       "stat - display statistics.\n"
	       "ps - display thread informations.\n"
	       "clear - clean screen.\n"
	       "hello - sample RT task say greeting.\n"
	       "*************************************************\n");
}

static void hello_world(void *pdata)
{
	/* shared local variable, one instance among all threads using
	 * this function, so a semaphore is needed to enforce mutual
	 * exclusion.
	 */
	static int count = 0;

	/* local variable, one instance per thread using this function */
	int s = 0, t;

	while (1) {
		/* enforce mutual exclusion for access to count */
		sem_pend(&sem);
		count++;
		sem_post(&sem);

		thread_delay(1);

		sem_pend(&sem);
		count++;
		t = count;
		sem_post(&sem);
		s++;

		/* because count was copied to t, no semaphore is needed here */
		printf("Hello World: count = %d, s = %d\n", t, s);

		thread_suspend(current_thread->tid);
	}
}

