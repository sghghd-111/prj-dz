#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>

#include <plat/gpmc.h>
#include "plat/gpio.h"
#include "plat/dma.h"
#include <linux/gpio.h>

#include "asm/uaccess.h"
#include "asm/io.h"
#include "asm/atomic.h"

u32 gpmc_read_reg(int idx);


/* GPMC register offsets */
#define GPMC_REVISION           0x00
#define GPMC_SYSCONFIG          0x10
#define GPMC_SYSSTATUS          0x14
#define GPMC_IRQSTATUS          0x18
#define GPMC_IRQENABLE          0x1c
#define GPMC_TIMEOUT_CONTROL    0x40
#define GPMC_ERR_ADDRESS        0x44
#define GPMC_ERR_TYPE           0x48
#define GPMC_CONFIG             0x50
#define GPMC_STATUS             0x54
#define GPMC_PREFETCH_CONFIG1   0x1e0
#define GPMC_PREFETCH_CONFIG2   0x1e4
#define GPMC_PREFETCH_CONTROL   0x1ec
#define GPMC_PREFETCH_STATUS    0x1f0
#define GPMC_ECC_CONFIG         0x1f4
#define GPMC_ECC_CONTROL        0x1f8
#define GPMC_ECC_SIZE_CONFIG    0x1fc
#define GPMC_ECC1_RESULT        0x200
#define GPMC_ECC_BCH_RESULT_0   0x240

#define GPMC_CS0_OFFSET         0x60
#define GPMC_CS_SIZE            0x30

#define GPMC_MEM_START          0x00000000
#define GPMC_MEM_END            0x3FFFFFFF
#define BOOT_ROM_SPACE          0x100000        /* 1MB */

#define GPMC_CHUNK_SHIFT        24              /* 16 MB */
#define GPMC_SECTION_SHIFT      28              /* 128 MB */

#define CS_NUM_SHIFT            24
#define ENABLE_PREFETCH         (0x1 << 7)
#define DMA_MPU_MODE            2

#define GPMC_NET3_CS			1
#define GPMC_NET4_CS			2

#define STNOR_GPMC_CONFIG1	0x28601000			
#define STNOR_GPMC_CONFIG2	0x00011001
#define STNOR_GPMC_CONFIG3	0x00020201
#define STNOR_GPMC_CONFIG4	0x08031003
#define STNOR_GPMC_CONFIG5	0x000f1111
#define STNOR_GPMC_CONFIG6	0x01030000

static const u32 gpmc_nor[7] = 
{
	STNOR_GPMC_CONFIG1,
	STNOR_GPMC_CONFIG2,
	STNOR_GPMC_CONFIG3,
	STNOR_GPMC_CONFIG4,
	STNOR_GPMC_CONFIG5,
	STNOR_GPMC_CONFIG6, 0
};

static struct resource ksnet1_resource[] = {
	[0] = 
	{
		.start = 0,
		.end = 0,
		.flags = IORESOURCE_MEM,
	},
	[1] = 
	{
		.start = 0,
		.end = 0,
		.flags = IORESOURCE_MEM,
	},
	[2] = 
	{
		.start = 0,
		.end = 0,
		.flags = IORESOURCE_IRQ | IORESOURCE_IRQ_LOWEDGE,
	},
};

static struct resource ksnet2_resource[] = {
	[0] = 
	{
		.start = 0,
		.end = 0,
		.flags = IORESOURCE_MEM,
	},
	[1] = 
	{
		.start = 0,
		.end = 0,
		.flags = IORESOURCE_MEM,
	},
	[2] = 
	{
		.start = 0,
		.end = 0,
		.flags = IORESOURCE_IRQ | IORESOURCE_IRQ_LOWEDGE | IORESOURCE_IRQ_HIGHEDGE,
	},
};

static void ksznet1_release(struct device *dev)
{
	printk(KERN_INFO "%s\n", __FUNCTION__);
}

#if 1
static struct platform_device ksznet1_device = 
{
    .name           = "ks8851_mll",
    .id             = 0,
    .num_resources  = ARRAY_SIZE(ksnet1_resource),
    .resource       = ksnet1_resource,
	.dev =
	{
		.release = ksznet1_release,
	}
};

static struct platform_device ksznet2_device = 
{
    .name           = "ks8851_mll",
    .id             = 1,
    .num_resources  = ARRAY_SIZE(ksnet2_resource),
    .resource       = ksnet2_resource,
	.dev =
	{
		.release = ksznet1_release,
	}
};

#else
static struct platform_device ksznet1_device = 
{
	[0] =
	{
        .name           = "ks8851_mll",
        .id             = 0,
        .num_resources  = ARRAY_SIZE(ksnet1_resource),
        .resource       = ksnet1_resource,
	},
	[1] =
	{
		.name			= "ks8851_mll",
		.id 			= 1,
		.num_resources	= ARRAY_SIZE(ksnet2_resource),
		.resource		= ksnet2_resource,
	}
};
#endif

static int gn_init(int cs, struct resource *res)
{
	unsigned long cs_mem_base;

	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG1, gpmc_nor[0]);
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG2, gpmc_nor[1]);
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG3, gpmc_nor[2]);
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG4, gpmc_nor[3]);
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG5, gpmc_nor[4]);
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG6, gpmc_nor[5]);

	if (0 > gpmc_cs_request(cs, SZ_32M, &cs_mem_base))
	{
		printk(KERN_ERR "cs[%d], request mem fail...\n", cs);
		return -1;
	}
	printk(KERN_INFO "cs[%d], gpmc req[%x]\n", cs, cs_mem_base);
	res[0].start = cs_mem_base;
	res[0].end = cs_mem_base + 3;

	if (GPMC_NET3_CS == cs)
	{
		res[1].start = cs_mem_base + 0x100;
		res[1].end = cs_mem_base + 0x100 + 3;
		
//		res[1].start = cs_mem_base + 0x100;
//		res[1].end = cs_mem_base + 0x100 + 3;	
		
		res[2].start = gpio_to_irq(88);
		res[2].end = gpio_to_irq(88);
	}
	else if (GPMC_NET4_CS == cs)
	{
		res[1].start = cs_mem_base + 0x200;
		res[1].end = cs_mem_base + 0x200 + 3;
		
//		res[1].start = cs_mem_base + 0x200;
//		res[1].end = cs_mem_base + 0x200 + 3;	
		
		res[2].start = gpio_to_irq(89);
		res[2].end = gpio_to_irq(89);
	}

	return 0;
}

static void gn_release(int cs)
{
	gpmc_cs_free(cs);
}

static int __init gn_mod_init(void)
{
	unsigned int val = 0;
	unsigned long cs_mem_base = 0;

	val = gpmc_read_reg(GPMC_REVISION);
	printk(KERN_INFO "gpmc version: %d.%d\n", (val>>4)&0x0f, val&0x0f);
	val = gpmc_read_reg(GPMC_IRQENABLE);
	printk(KERN_INFO "gpmc irq-en flag = %x\n", val);
	val = gpmc_read_reg(GPMC_TIMEOUT_CONTROL);
	printk(KERN_INFO "gpmc tout-ctl [%x]\n", val);

	val = gpmc_cs_read_reg(0, GPMC_CS_CONFIG1);
	printk(KERN_INFO "gpmc c1 value = %x\n", val);
	
	val = gpmc_cs_read_reg(0, GPMC_CS_CONFIG2);
	printk(KERN_INFO "gpmc c2 value = %x\n", val);
	val = gpmc_cs_read_reg(0, GPMC_CS_CONFIG3);
	printk(KERN_INFO "gpmc c3 value = %x\n", val);
	val = gpmc_cs_read_reg(0, GPMC_CS_CONFIG4);
	printk(KERN_INFO "gpmc c4 value = %x\n", val);
	val = gpmc_cs_read_reg(0, GPMC_CS_CONFIG5);
	printk(KERN_INFO "gpmc c5 value = %x\n", val);
	val = gpmc_cs_read_reg(0, GPMC_CS_CONFIG6);
	printk(KERN_INFO "gpmc c6 value = %x\n", val);
	val = gpmc_cs_read_reg(0, GPMC_CS_CONFIG7);
	printk(KERN_INFO "gpmc c7 value = %x\n", val);

	gn_init(GPMC_NET3_CS, ksnet1_resource);
	gn_init(GPMC_NET4_CS, ksnet2_resource);

	//fill value to sources

	platform_device_register(&ksznet1_device);
	platform_device_register(&ksznet2_device);
	return 0;
}

static void __exit gn_mod_release(void)
{
	platform_device_unregister(&ksznet1_device);
	gn_release(GPMC_NET3_CS);

	platform_device_unregister(&ksznet2_device);
	gn_release(GPMC_NET4_CS);

}

module_init(gn_mod_init);
module_exit(gn_mod_release);

MODULE_AUTHOR("NJHX SW Dept.");
MODULE_DESCRIPTION("ksz8851 device Driver");
MODULE_LICENSE("GPL");

