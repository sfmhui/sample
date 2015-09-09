/*
 * COPYRIGHT (C) 2015 MICHAEL HUI. ALL RIGHTS RESERVED.
 *
 * A very simple kernel module driver.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/kthread.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Hui");
MODULE_DESCRIPTION("thread device driver");


/*****************************************************************
 * parameters - specify when insmod
 */
static unsigned int debugging = 0;

module_param(debugging, int, 0);

/*****************************************************************
 * global driver variable
 */
static struct task_struct * THREAD = 0L;

static int
timer_task(void * data)
{
	struct task_struct * this;
	
	
	this = current;
	set_current_state(TASK_INTERRUPTIBLE);

	for (;;)
	{
		int i;

		pr_info("timer task\n");
		schedule();
	}
	return 0;
}

/*****************************************************************
 * init / exit - called when insmod / rmmod
 */
static void __exit
thread_exit(void)
{
	if (THREAD != 0L)
	{
		kthread_stop(THREAD);
		THREAD = 0L;
	}

	if (debugging)
		pr_info("thread_exit\n");
}

static int __init
thread_init(void)
{
	int ret = 0;

	if (debugging)
		pr_info("thread_init\n");
	
	THREAD = kthread_run(timer_task, 0L, "timer");
	if (THREAD == 0L)
	{
		ret = -EINVAL;
	}

	return ret;
}

module_init(thread_init);
module_exit(thread_exit);


