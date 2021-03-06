#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/gpio.h>

#include <plat/gpmc.h>
#include "plat/gpio.h"
#include "plat/dma.h"

#include "asm/uaccess.h"
#include "asm/io.h"
#include "asm/atomic.h"
#include <linux/serial_8250.h>
#include <linux/delay.h>
#include <linux/irq.h>

u32 gpmc_read_reg(int idx);
static void gpmc_release(struct device *dev);


#if 1
#define PORT(dev,_base,_irq)                                \
        {                                               \
                dev.mapbase    = (unsigned long)_base;                \
                dev.irq            = _irq;                 \
                dev.uartclk        = 1843200;              \
                dev.iotype         = UPIO_MEM;            \
                dev.flags          = (UPF_BOOT_AUTOCONF | UPF_IOREMAP ) | UPF_SHARE_IRQ;    \
                dev.regshift   = 0;		\
        }
#else
#define PORT(dev,_base,_irq)                                \
        {                                               \
                dev.iobase         = _base;                \
                dev.irq            = _irq;                 \
                dev.uartclk        = 1843200;              \
                dev.iotype         = UPIO_PORT;            \
                dev.flags          = UPF_BOOT_AUTOCONF | UPF_FOURPORT | UPF_SHARE_IRQ;    \
        }
#endif

static struct plat_serial8250_port ch438_data[] = {
		{ },
		{ },
		{ },
		{ },
		{ },
		{ },
		{ },
		{ },
        { },
};

static struct platform_device ch438_device = {
        .name                   = "serial8250",
//		.owner  				= THIS_MODULE,
        .id                     = PLAT8250_DEV_EXAR_ST16C554,
        .dev                    = {
		        .release = gpmc_release,
                .platform_data  = ch438_data,
        },
};

static void gpmc_release(struct device *dev)
{
	printk(KERN_INFO "%s\n", __FUNCTION__);
}

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

#define GPMC_CH438_CS			3

#if 0
#define STNOR_GPMC_CONFIG1	0x28601000			
#define STNOR_GPMC_CONFIG2	0x00011001
#define STNOR_GPMC_CONFIG3	0x00020201
#define STNOR_GPMC_CONFIG4	0x08031003
#define STNOR_GPMC_CONFIG5	0x000f1111
#define STNOR_GPMC_CONFIG6	0x0f030080
#endif

#define STNOR_GPMC_CONFIG1	0x28600003	//0x28600000			//8bit
#define STNOR_GPMC_CONFIG2	0x00011001	//0x001f1f01			//		
#define STNOR_GPMC_CONFIG3	0x00020201	//0x701f1f8f
#define STNOR_GPMC_CONFIG4	0x08031003	//0x1f0f1f0f	//
#define STNOR_GPMC_CONFIG5	0x000f1111	//0x0f1f1f1f
#define STNOR_GPMC_CONFIG6	0x0f030080	//0x1f0f0fcf

static const u32 gpmc_nor[7] = 
{
	STNOR_GPMC_CONFIG1,
	STNOR_GPMC_CONFIG2,
	STNOR_GPMC_CONFIG3,
	STNOR_GPMC_CONFIG4,
	STNOR_GPMC_CONFIG5,
	STNOR_GPMC_CONFIG6, 0
};

#if 0
static struct gpmc_timings ch438_timings = {
	/* Minimum clock period for synchronous mode (in picoseconds)*/
	.sync_clk = 0,
	/* CS signal timings corresponding to GPMC_CONFIG2 */
	.cs_on = 10, /*T6s */ 
	.cs_rd_off = 80, /*oe_off + T7h */ 
	.cs_wr_off = 60,  /*Tcs for write: we_off + T13h */ 
	/* ADV signal timings corresponding to GPMC_CONFIG3 */// 
	.adv_on = 0, 
	/* Address setup Tas*/// 
	.adv_rd_off = 110, 
	/* Read deassertion time?Trd + Tdd*/// 
	.adv_wr_off = 90, 
	/* Write deassertion time?Twr + Tdh*/ 
	/* WE signals timings corresponding to GPMC_CONFIG4 */ 
	.we_on = 20, 
	/*WE assertion time: cs_on + T13d */ 
	.we_off = 60, 
	/*WE deassertion time: we_on + T13w */ 
	/* OE signals timings corresponding to GPMC_CONFIG4 */ 
	.oe_on = 20, 
	/*OE assertion time: cs_on + T7d */ 
	.oe_off = 70, 
	/*OE deassertion time: oe_on + T7w */ 
	/* Access time and cycle time timings corresponding toGPMC_CONFIG5 */// 
	.page_burst_access = 2 * 10, /* Multiple access word delay */ 
	.access = 60, 
	/*Start-cycle to first data valid delay: at leaset oe_on + T12d */ 
	.rd_cycle = 110, 
	/*Total read cycle time: cs_rd_off + (T9d - T7h) - oe_on */ 
	.wr_cycle = 90, 
	/*Total write cycle time: cs_wr_off + (T15d - T13h) - we_on */ 
	/* The following are only on OMAP3430 */ 
	.wr_access = 40, 
	/*WRACCESSTIME: max we_off - T16s? */  
	.wr_data_mux_bus = 0, /*WRDATAONADMUXBUS */
};
#else
static struct gpmc_timings ch438_timings = {

	.sync_clk = 0,

	.cs_on = 10,
	.cs_rd_off = 90,
	.cs_wr_off = 70,

	.adv_on = 0,
	.adv_rd_off = 15,
	.adv_wr_off = 15,
	.we_off = 65,
	.oe_off = 70,

	.access = 90,
	.rd_cycle = 140,
	.wr_cycle = 90,

	.wr_access = 90,
	.wr_data_mux_bus = 0,
};
#endif

extern int gpmc_cs_set_timings(int cs, const struct gpmc_timings *t);

static void gpmc_time_set(int cs, struct gpmc_timings *in)
{
#if 1
	int ret = 0;

	ret = gpmc_cs_set_timings(cs, in);printk(KERN_INFO "gpmc_cs_set_timings ret %d\n", ret);
#else
	struct gpmc_timings t, *out;

	out = &t;
	memset(out, 0, sizeof(struct gpmc_timings));

	out->sync_clk = in->sync_clk;
	out->cs_on = gpmc_round_ns_to_ticks(in->cs_on);
	out->adv_on = gpmc_round_ns_to_ticks(in->adv_on);
	
	/* Read */
	out->adv_rd_off = gpmc_round_ns_to_ticks(
							in->adv_rd_off);
	out->oe_on  = out->adv_on;
	out->access = gpmc_round_ns_to_ticks(in->access);
	out->oe_off = gpmc_round_ns_to_ticks(in->oe_off);
	out->cs_rd_off = gpmc_round_ns_to_ticks(in->cs_rd_off);
	out->rd_cycle	= gpmc_round_ns_to_ticks(in->rd_cycle);
	
	/* Write */
	out->adv_wr_off = gpmc_round_ns_to_ticks(
							in->adv_wr_off);
	out->we_on  = out->oe_on;
	if (cpu_is_omap34xx()) {
		out->wr_data_mux_bus = gpmc_round_ns_to_ticks(
							in->wr_data_mux_bus);
		out->wr_access = gpmc_round_ns_to_ticks(
							in->wr_access);
	}
	out->we_off = gpmc_round_ns_to_ticks(in->we_off);
	out->cs_wr_off = gpmc_round_ns_to_ticks(in->cs_wr_off);
	out->wr_cycle	= gpmc_round_ns_to_ticks(in->wr_cycle);
	
	/* Configure GPMC */
	gpmc_cs_configure(cs, GPMC_CONFIG_DEV_SIZE, 0);
//	gpmc_cs_configure(cs, GPMC_CONFIG_DEV_TYPE, GPMC_DEVICETYPE_NAND);
	gpmc_cs_set_timings(cs, out);
#endif
}

static void gs_show(void *mem, int offset)
{
	int j = 0;
	int ret = 0;
#if 0
printk(KERN_INFO "show\n");
	for (j=0; j<100; j++)
		writeb(0x13, (mem+offset*8+3));
//		ret = __raw_readl(mem+8*offset+0);
//		printk("mem%p+%d, val[%x]\n", mem, 0, __raw_readl(mem+8*offset+0));
printk(KERN_INFO "show %d\n", ret);
#endif
	printk("mem%p+%d, val[%x]\n", mem, 1, __raw_readl(mem+8*offset+1));
	printk("mem%p+%d, val[%x]\n", mem, 2, __raw_readl(mem+8*offset+2));
	printk("mem%p+%d, val[%x]\n", mem, 3, __raw_readl(mem+8*offset+3));
	printk("mem%p+%d, val[%x]\n", mem, 4, __raw_readl(mem+8*offset+4));
	printk("mem%p+%d, val[%x]\n", mem, 5, __raw_readl(mem+8*offset+5));
	printk("mem%p+%d, val[%x]\n", mem, 6, __raw_readl(mem+8*offset+6));
	printk("mem%p+%d, val[%x]\n", mem, 7, __raw_readl(mem+8*offset+7));
	printk("mem%p+%d, val[%x]\n", mem, 0x4f, __raw_readl(mem+8*offset+0x4f));

}

static void addr_wr(void *base_addr, int offset, char data)
{
	char *addr = base_addr + offset;

	*addr = data;
}

static int gs_init(int cs)
{
	int j = 0;
	int ret = 0;
	unsigned long cs_mem_base;
	void *mem_vir;
	int val = 0;
#if 1
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG1, gpmc_nor[0]);
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG2, gpmc_nor[1]);
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG3, gpmc_nor[2]);
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG4, gpmc_nor[3]);
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG5, gpmc_nor[4]);
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG6, gpmc_nor[5]);

#if 0
val = gpmc_cs_read_reg(3, GPMC_CS_CONFIG1);
printk(KERN_INFO "gpmc c1 value = %x\n", val);

val = gpmc_cs_read_reg(3, GPMC_CS_CONFIG2);
printk(KERN_INFO "gpmc c2 value = %x\n", val);
val = gpmc_cs_read_reg(3, GPMC_CS_CONFIG3);
printk(KERN_INFO "gpmc c3 value = %x\n", val);
val = gpmc_cs_read_reg(3, GPMC_CS_CONFIG4);
printk(KERN_INFO "gpmc c4 value = %x\n", val);
val = gpmc_cs_read_reg(3, GPMC_CS_CONFIG5);
printk(KERN_INFO "gpmc c5 value = %x\n", val);
val = gpmc_cs_read_reg(3, GPMC_CS_CONFIG6);
printk(KERN_INFO "gpmc c6 value = %x\n", val);
val = gpmc_cs_read_reg(3, GPMC_CS_CONFIG7);
printk(KERN_INFO "gpmc c7 value = %x\n", val);
#endif
#else
	ret = gpmc_cs_configure(cs, GPMC_CONFIG_DEV_TYPE, GPMC_DEVICETYPE_NOR);
printk(KERN_INFO "ret=%d\n", ret);
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG1, GPMC_CONFIG1_READTYPE_SYNC|GPMC_CONFIG1_WRITETYPE_SYNC);
#endif
	
//	gpmc_time_set(cs, &ch438_timings);

	if (0 > gpmc_cs_request(cs, SZ_16M, &cs_mem_base))
	{
		printk(KERN_ERR "cs[%d], request mem fail...\n", cs);
		return -1;
	}
	mem_vir = ioremap_nocache(cs_mem_base, 1000000);
	printk(KERN_INFO "cs[%d], gpmc req[%x]\n", cs, mem_vir);//cs_mem_base = 0;
	PORT(ch438_data[0], cs_mem_base, gpio_to_irq(19));
	printk(KERN_INFO "%d, %x,%x,%d,%x,%x\n", j, ch438_data[j].iobase, ch438_data[j].irq, ch438_data[j].uartclk, ch438_data[j].iotype, ch438_data[j].flags);
	PORT(ch438_data[4], cs_mem_base + 0x08*1, gpio_to_irq(19));
	j++;
	printk(KERN_INFO "%d, %x,%x,%d,%x,%x\n", j, ch438_data[j].iobase, ch438_data[j].irq, ch438_data[j].uartclk, ch438_data[j].iotype, ch438_data[j].flags);
	PORT(ch438_data[1], cs_mem_base + 0x08*2, gpio_to_irq(19));
	j++;
	printk(KERN_INFO "%d, %x,%x,%d,%x,%x\n", j, ch438_data[j].iobase, ch438_data[j].irq, ch438_data[j].uartclk, ch438_data[j].iotype, ch438_data[j].flags);
	PORT(ch438_data[5], cs_mem_base + 0x08*3, gpio_to_irq(19));
	PORT(ch438_data[2], cs_mem_base + 0x08*4, gpio_to_irq(19));
	PORT(ch438_data[6], cs_mem_base + 0x08*5, gpio_to_irq(19));
	PORT(ch438_data[3], cs_mem_base + 0x08*6, gpio_to_irq(19));
	PORT(ch438_data[7], cs_mem_base + 0x08*7, gpio_to_irq(19));

	ret = irq_set_irq_type(gpio_to_irq(19), IRQ_TYPE_EDGE_FALLING);
#if 0
	for(j=0; j<8; j++)
	{
//		mem_vir = (char *)ioremap_nocache((cs_mem_base+j*8), 7);	  

		writeb(0x13, (mem_vir+j*8+3));printk(KERN_INFO "[%d], addr[%d], val[%x][%x]\n", j, 3, 0x13, readb(mem_vir+j*8+3));
		writeb(0xc7,(mem_vir+j*8+2));printk(KERN_INFO "[%d], addr[%d], val[%x][%x]\n", j, 2, 0xc7, readb(mem_vir+j*8+2));	
		writeb(0x01,(mem_vir+j*8+1));	printk(KERN_INFO "[%d], addr[%d], val[%x][%x]\n", j, 1, 0x01, readb(mem_vir+j*8+1));
		writeb(0x08,(mem_vir+j*8+4));	printk(KERN_INFO "[%d], addr[%d], val[%x][%x]\n", j, 4, 0x08, readb(mem_vir+j*8+4));
//		mem_vir = 0;  
	}
#endif
//	mem_vir = (char *)ioremap_nocache(cs_mem_base, 100000);
#if 0
	gs_show(mem_vir, 0);
	gs_show(mem_vir, 2);
	gs_show(mem_vir, 3);
	gs_show(mem_vir, 4);
	gs_show(mem_vir, 5);
	gs_show(mem_vir, 6);
	gs_show(mem_vir, 7);
#endif
	for(j=0; j<8; j++)
	{
		addr_wr(mem_vir, 0x7, j);
		printk(KERN_INFO "wr val[%x], rd val[%x]\n", j, *((char *)(mem_vir+0x7)));
		udelay(100);
	}

	return 0;
}

static void gs_release(int cs)
{
	gpmc_cs_free(cs);
}

static int __init ch438_mod_init(void)
{
	unsigned int val = 0;
	unsigned long cs_mem_base = 0;
#if 1
	val = gpmc_read_reg(GPMC_REVISION);
	printk(KERN_INFO "gpmc version: %d.%d\n", (val>>4)&0x0f, val&0x0f);
	val = gpmc_read_reg(GPMC_IRQENABLE);
	printk(KERN_INFO "gpmc irq-en flag = %x\n", val);
	val = gpmc_read_reg(GPMC_TIMEOUT_CONTROL);
	printk(KERN_INFO "gpmc tout-ctl [%x]\n", val);
#endif
	gs_init(GPMC_CH438_CS);
#if 1
//printk(KERN_INFO "size of ch438_data = %d\n", sizeof(ch438_data));
#endif
	return platform_device_register(&ch438_device);
}

static void __exit ch438_mod_release(void)
{
	platform_device_unregister(&ch438_device);
	gs_release(GPMC_CH438_CS);
}

module_init(ch438_mod_init);
module_exit(ch438_mod_release);

MODULE_AUTHOR("NJHX SW Dept.");
MODULE_DESCRIPTION("ch438 device Driver");
MODULE_LICENSE("GPL");

