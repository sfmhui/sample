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
MODULE_DESCRIPTION("mio");


/*****************************************************************
 * parameters - specify when insmod
 */
static unsigned int debugging = 0;

module_param(debugging, int, 0);


/*****************************************************************
 * procfs
 */
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

struct proc_dir_entry *	root;
struct proc_dir_entry * mio;

static unsigned long	MEM_ADDR = 0;
static int		MEM_LEN = 0;

static ssize_t
procfs_read(struct file *file, char *buf, size_t len, loff_t * f_pos)
{
	int rlen = 0;

	if (debugging) {
		pr_info("read\n");
	}

	if (MEM_LEN > 0)
	{
	}

	return rlen;
}

static ssize_t
procfs_write(struct file *file, const char *buf, size_t len, loff_t * f_pos)
{
	int rlen = len;
	char cmdline[64];

	if (rlen > sizeof(cmdline))
	{
		rlen = sizeof(cmdline) - 1;
	}

	if (copy_from_user(cmdline, buf, rlen))
	{
		return -EFAULT;
	}
	cmdline[rlen] = '\0';

	if (sscanf(cmdline, "%lx%d", &MEM_ADDR, &MEM_LEN) != 2)
	{
		pr_err("invalid mio input %s\n", cmdline);
	}
	if (debugging) {
		pr_info("wrote %d bytes - %s\n", rlen, cmdline);
	}
	return rlen;
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

	if ((root = proc_mkdir("debug", 0L)) == 0L)
	{
		pr_err("failed to create root procfs directory\n");
		ret = -ENOMEM;
	}
	else if ((mio = proc_create("mio", 0660, root, &procfs_fops)) == 0L)
	{
		pr_err("failed to create root procfs mio\n");
		ret = -ENOMEM;
	}

	if (debugging)
		pr_info("procfs_init\n");

	return ret;
}

static void __exit
procfs_exit(void)
{
	if (mio != 0L)
	{
		proc_remove(mio);
		mio = 0L;
	}

	if (root != 0L)
	{
		proc_remove(root);
		root = 0L;
	}

	if (debugging)
		pr_info("procfs_exit\n");
}

/*****************************************************************
 * init / exit - called when insmod / rmmod
 */
static int __init
mio_init(void)
{
	int ret = 0;

	if ((ret = procfs_init()) < 0)
	{
		// EMPTY BODY
	}
	else if (debugging)
	{
		pr_info("mio_init\n");
	}

	return ret;
}

static void __exit
mio_exit(void)
{
	procfs_exit();

	if (debugging)
		pr_info("mio_exit\n");
}

module_init(mio_init);
module_exit(mio_exit);



