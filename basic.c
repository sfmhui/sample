/*
 * COPYRIGHT (C) 2015 MICHAEL HUI. ALL RIGHTS RESERVED.
 *
 * A very simple kernel module driver.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Hui");
MODULE_DESCRIPTION("basic device driver");


/*****************************************************************
 * parameters - specify when insmod
 */
static unsigned int debugging = 0;

module_param(debugging, int, 0);


/*****************************************************************
 * init / exit - called when insmod / rmmod
 */
static int __init
basic_init(void)
{
	int ret = 0;

	if (debugging)
		pr_info("basic_init\n");

	return ret;
}

static void __exit
basic_exit(void)
{
	if (debugging)
		pr_info("basic_exit\n");
}

module_init(basic_init);
module_exit(basic_exit);

