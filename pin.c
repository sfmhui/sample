/*
 * COPYRIGHT (C) 2015 MICHAEL HUI. ALL RIGHTS RESERVED.
 *
 * A very simple kernel module driver.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/gpio.h>

#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Hui");
MODULE_DESCRIPTION("pin device driver");


/*****************************************************************
 * parameters - specify when insmod
 */
static unsigned int debugging = 0;
static unsigned int value = 0;

module_param(debugging, int, 0);
module_param(value, int, 0);


/*****************************************************************
 * init / exit - called when insmod / rmmod
 */
static int __init
pin_init(void)
{
	int ret = 0;
	unsigned long pin = AT91_PIN_PC15;

	if (debugging)
		pr_info("pin_init\n");

	
	if (! gpio_is_valid(pin))
	{
		pr_err("pin is not valid\n");
	}
	else if ( gpio_request(pin, "clock24mhz-gpio") != 0 )
	{
		pr_err("failed to request pin\n");
	}
	else if ((ret = at91_set_C_periph(pin, 0)) < 0)
	{
		pr_err("failed to set c periph %d\n", ret);
	}

	{
		gpio_free(pin);
	}

	return ret;
}

static void __exit
pin_exit(void)
{
	if (debugging)
		pr_info("pin_exit\n");
}

module_init(pin_init);
module_exit(pin_exit);

