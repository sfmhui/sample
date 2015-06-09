/*
 * COPYRIGHT (C) 2015 MICHAEL HUI. ALL RIGHTS RESERVED.
 *
 * This is a sample program that implements a platform device
 * driver. It toggles a led on my arm9g25 board.
 *
 * compile and load the module:
 * # insmod led.ko
 *
 * in order for this to work, you need to have an entry in
 * the device tree with compatible atmel,at91sam9x5-tcb. This
 * should work with other at91 tcbs.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/of_address.h>
#include <asm/io.h>

#include <linux/clk.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Hui");
MODULE_DESCRIPTION("led");

/*****************************************************************
 * parameters - specify when insmod
 */
static unsigned int debugging = 0;

module_param(debugging, int, 0);

/*****************************************************************
 * platform driver
 */
#include <linux/platform_device.h>

struct led
{
	struct platform_device *	pdev;	


	void *	iomem;
	int	irq;
};

struct led LED = { 0L };


#include <linux/of_device.h>
static struct of_device_id led_dt_ids[] = 
	{
		{
			.compatible = "atmel,at91sam9x5-tcb"
		},
		{
		}
	};

static int led_remove(struct platform_device * pdev)
{
	struct led * data = &LED;

	pr_info("led_remove\n");

	if (pdev == data->pdev)
	{
		if (data->iomem)
		{
			iounmap(data->iomem);
			data->iomem = 0L;
		}
		data->pdev = 0L;
	}
	else
	{
		pr_info("LED not registered; not removing\n");
	}

	return 0;
}

static int led_probe(struct platform_device * pdev)
{
	struct led *		data = &LED;

	int	irq = platform_get_irq(pdev, 0);

	pr_info("led_probe: name %s, id %d, id_auto %d, nres %d irq %d\n",
		pdev->name, pdev->id, pdev->id_auto, pdev->num_resources, irq);

	if (pdev->num_resources > 0)
	{
		int i;
		for (i = 0; i < pdev->num_resources; i++)
		{
			pr_info("\tresource %d: %s\n", i, pdev->resource[0].name);
		}
	}

	if (pdev->dev.of_node)
	{
		pr_info("device has of_node\n");
	}

	if ( data->pdev )
	{
		pr_info("already registered\n");
		return 0;
	}

	data->pdev	= pdev;
	data->irq	= irq;

	{
		struct resource *	rsrc;
		rsrc = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		if (rsrc == 0L)
		{
			pr_err("failed to get mem resource\n");
		}
		else
		{
			unsigned long size = rsrc->end - rsrc->start + 1;
			data->iomem	= ioremap(rsrc->start, size);
			if (data->iomem != 0L)
			{
				pr_info("remapped %lx size %ld to %lx\n",
					(unsigned long)rsrc->start, size, (unsigned long)data->iomem);
			}
			else
			{
				pr_err("failed to ioremap\n");
			}
		}
	}

	return 0;
}

static struct platform_driver led_driver = 
	{
		.probe	= led_probe,
		.remove = led_remove,
		.driver	= 
		{
			.name		= "led-at91",
			.owner		= THIS_MODULE,
			.of_match_table	= led_dt_ids
		}
	};


/*****************************************************************
 * init / exit - called when insmod / rmmod
 */
static int __init
led_init(void)
{
	int ret = 0;

	pr_info("led_init\n");

	ret = platform_driver_register(&led_driver);

	return ret;
}

static void __exit
led_exit(void)
{
	platform_driver_unregister(&led_driver);

	pr_info("led_exit\n");
}

module_init(led_init);
module_exit(led_exit);



