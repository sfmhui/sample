/*
 * COPYRIGHT (C) 2015 MICHAEL HUI. ALL RIGHTS RESERVED.
 *
 * A very simple platform device driver. we use the tcb block
 * as the target device. be sure that they are not already used.
 *
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Hui");
MODULE_DESCRIPTION("platform device driver");


/*****************************************************************
 * parameters - specify when insmod
 */
static unsigned int debugging = 0;

module_param(debugging, int, 0);


/*****************************************************************
 * platform device driver
 *
 * this use the tcb block as example
 */
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/of.h>


struct custom_driver
{
	struct platform_device*	pdev;
	struct resource		resource;

	void *			iomem;
	unsigned char		unit;
	unsigned char		irq;
};

#define	MOD_NAME	"tcb"
#define	NUM_UNITS()	2


int
find_unit(struct platform_device * pdev)
{
	int unit;

	/* figure out unit */
	if (pdev->dev.of_node == 0L)
	{
		return -1;
	}
	else if ((unit = of_alias_get_id(pdev->dev.of_node, MOD_NAME)) < 0 || unit >= NUM_UNITS())
	{
		return -1;
	} 

	return unit;
}

static struct of_device_id pdd_dt_ids[] = 
	{
		{
			.compatible = "atmel,at91sam9x5-" MOD_NAME
		},
		{
			.compatible = "atmel,at91sam9g20-" MOD_NAME
		},
		{
		}
	};
MODULE_DEVICE_TABLE(of, pdd_dt_ids);

static int pdd_remove(struct platform_device * pdev)
{
	int ret = 0;
	int unit = find_unit(pdev);

	if (debugging)
		pr_info("pdd_remove %d\n", unit);

	return ret;
}

static int pdd_probe(struct platform_device * pdev)
{
	int ret = 0;
	int unit = find_unit(pdev);

	if (debugging)
		pr_info("pdd_probe %d\n", unit);

	return ret;
}

static struct platform_driver pdd_driver = 
	{
		.probe	= pdd_probe,
		.remove = pdd_remove,
		.driver	= 
		{
			.name		= "pdd",
			.owner		= THIS_MODULE,
			.of_match_table	= pdd_dt_ids
		}
	};


























/*****************************************************************
 * init / exit - called when insmod / rmmod
 */
static int __init
basic_init(void)
{
	int ret = 0;

	ret = platform_driver_register(&pdd_driver);

	if (debugging)
		pr_info("basic_init\n");

	return ret;
}

static void __exit
basic_exit(void)
{
	platform_driver_unregister(&pdd_driver);

	if (debugging)
		pr_info("basic_exit\n");
}

module_init(basic_init);
module_exit(basic_exit);

