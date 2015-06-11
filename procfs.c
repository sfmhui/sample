/*
 * COPYRIGHT (C) 2015 MICHAEL HUI. ALL RIGHTS RESERVED.
 *
 * A very simple kernel module driver with procfs
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Hui");
MODULE_DESCRIPTION("procfs device driver");


/*****************************************************************
 * parameters - specify when insmod
 */
static unsigned int debugging = 0;

module_param(debugging, int, 0);


/*****************************************************************
 * procfs
 */

/*****************************************************************
 * init / exit - called when insmod / rmmod
 */
#include <linux/proc_fs.h>
#include <linux/fs.h>

struct proc_dir_entry *	root;
struct proc_dir_entry * readme;

static ssize_t
procfs_read(struct file *file, char *buf, size_t len, loff_t * f_pos)
{
	if (debugging) {
		pr_info("read\n");
	}
	return 0;
}

static ssize_t
procfs_write(struct file *file, const char *buf, size_t len, loff_t * f_pos)
{
	if (debugging) {
		pr_info("write\n");
	}
	return len;
}


static const struct file_operations procfs_fops = {
	.owner = THIS_MODULE,
	.read = procfs_read,
	.write = procfs_write
};

static int __init
procfs_init(void)
{
	int ret = 0;

	if ((root = proc_mkdir("basic", 0L)) == 0L)
	{
		pr_err("failed to create root procfs directory\n");
		ret = -ENOMEM;
	}
	else if ((readme = proc_create("readme", 0644, root, &procfs_fops)) == 0L)
	{
		pr_err("failed to create root procfs readme\n");
		ret = -ENOMEM;
	}

	if (debugging)
		pr_info("procfs_init\n");

	return ret;
}

static void __exit
procfs_exit(void)
{
	if (readme != 0L)
	{
		proc_remove(readme);
		readme = 0L;
	}

	if (root != 0L)
	{
		proc_remove(root);
		root = 0L;
	}

	if (debugging)
		pr_info("procfs_exit\n");
}

module_init(procfs_init);
module_exit(procfs_exit);

