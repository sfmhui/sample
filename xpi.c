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

#include <linux/clk.h>
#include <linux/io.h>

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
	struct clk *		clk;

	void __iomem *		iomem;
	unsigned char		unit;
	unsigned char		irq;
};
struct custom_driver	CDR;


#define	MOD_NAME	"tcb"
#define	NUM_UNITS()	2


static struct of_device_id xpi_dt_ids[] = 
	{
		{
			.compatible = "xc3s50a"
		},
		{
		}
	};
MODULE_DEVICE_TABLE(of, xpi_dt_ids);


static inline u32
spi_readl(int reg)
{
	u32 ret = __raw_readl(CDR.iomem + reg);

	return ret;
}

static inline void 
spi_writel(u32 val, int reg)
{
	__raw_writel(val, CDR.iomem + reg);
}

#define	CR	0x0000
#define	MR	0x0004
#define	RDR	0x0008
#define	TDR	0x000c
#define	SR	0x0010
#define	IER	0x0014
#define	IDR	0x0018
#define	IMR	0x001c
#define	CSR	0x0030

static int xpi_remove(struct platform_device * pdev)
{
	int ret = 0;
	int unit = 0;

	if (debugging)
		pr_info("xpi_remove %d\n", unit);

	if (CDR.iomem != 0)
	{
		spi_writel(1<<7, CR);		// WRST
		spi_writel(1<<1, CR);		// SPIDIS
		spi_readl(SR);

		iounmap(CDR.iomem);
		CDR.iomem = 0L;
	}

	if (CDR.clk != 0L)
	{
		clk_disable_unprepare(CDR.clk);
		CDR.clk = 0L;
	}

	return ret;
}

static int xpi_probe(struct platform_device * pdev)
{
	int ret = 0;
	int unit = 0;

	if (debugging)
		pr_info("xpi_probe %d\n", unit);

	CDR.pdev = pdev;

	// get clock
	{
		struct clk *	clk;
		unsigned long hz;

		clk = devm_clk_get(&pdev->dev, "spi_clk");
		if (IS_ERR(clk))
		{
			pr_err("failed to get spi_clk\n");
			return -EINVAL;
		}

		if (clk_prepare_enable(clk))
		{
			pr_err("failed to enable clock\n");
			return -EINVAL;
		}
		hz = clk_get_rate(clk);

		pr_info("clock running at %lu\n", hz);

		CDR.clk = clk;
	}

	// memmap
	{
		struct resource *	rsrc;

		rsrc = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		if (rsrc == 0L)
		{
			pr_err("failed to get regs\n");
			return -EINVAL;
		}
		CDR.resource = *rsrc;

		pr_info("resource to spi %x %x\n", rsrc->start, rsrc->end);
		CDR.iomem = ioremap(rsrc->start, resource_size(rsrc));
	}

	// pinctrl
	{
		struct pinctrl * pinctrl = 0L;

		pinctrl = devm_pinctrl_get(&pdev->dev);
		if (IS_ERR(pinctrl))
		{
			pr_err("failed to configure spi pin\n");
			return -EINVAL;
		}
	}


	// no interrupts
	{
		spi_writel( (0x3<<8)|0xf, IDR);
	}

	// initialize
	{
		spi_writel(1<<7, CR);		// WRST
		spi_writel(1<<1, CR);		// SPIDIS

		// MSTR=1
		// PS=0
		// PCSDEC=0
		// MODFDIS=1
		// WDRBT=1
		// LLB=0
		// PCS=0
		// DLYBCS=0
		spi_writel( (1<<0)|(0<<1)|(0<<2)|(1<<4)|(1<<5)|(0<<7), MR);
		spi_writel(1<<0, CR);		// SPIEN
	}

	// test
	{
		unsigned char tx[] = { 0, 0, 0, 0, };
		unsigned char rx[8];

		int len = sizeof(tx);

		int i;
		int err = 0;

		u32 s = spi_readl(SR);

		memset(&rx[0], '\0', sizeof(rx));

		pr_info("init sr is %x\n", s);
		if ((s & 0x1) != 0)
		{
			spi_readl(RDR);
		}

		// CPOL=0
		// NCPHA=0
		// CSAAT=0
		// BITS=0
		// SCBR=4
		// DLYBS=
		// DLYBCT=

		spi_writel( 0<<0 | 1<<1 | 0<<3 | 0<<4 | 4<<8 | 0<<16 | 0<<24, CSR + 0);

		for (i = 0; i < len && err == 0; i++)
		{
			u32 c = tx[i];
			int j;

			s = spi_readl(SR);


			if (len == (i+1))
			{
				c |= (1<<24);		// lastxfer
			}
			spi_writel(c, TDR);

			err = 1;
			for (j = 0; j < 100; j++)
			{
				s = spi_readl(SR);

				if ((s & 0x3) == 0x3)
				{
					pr_info("start sr is %x %d\n", s, j);
					pr_info("sent a char\n");
					rx[i] = spi_readl(RDR) & 0xff;
					err = 0;
					break;
				}
			}
		}

		if (err)
		{
			ret = -1;
		}

		pr_info("read %02x %02x %02x %02x %02x %02x\n", 
			rx[0], rx[1], rx[2], rx[3], rx[4], rx[5]);
	}

	return ret;
}

static struct platform_driver xpi_driver = 
	{
		.probe	= xpi_probe,
		.remove = xpi_remove,
		.driver	= 
		{
			.name		= "xpi",
			.owner		= THIS_MODULE,
			.of_match_table	= xpi_dt_ids
		}
	};


























/*****************************************************************
 * init / exit - called when insmod / rmmod
 */
static int __init
xpi_init(void)
{
	int ret = 0;

	if (debugging)
		pr_info("xpi_init\n");

	ret = platform_driver_register(&xpi_driver);

	return ret;
}

static void __exit
xpi_exit(void)
{
	platform_driver_unregister(&xpi_driver);

	if (debugging)
		pr_info("xpi_exit\n");
}

module_init(xpi_init);
module_exit(xpi_exit);

