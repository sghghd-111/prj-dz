#include "pulse.h"
#include "../../include/ioctl_cmd.h"
#include <linux/irq.h>
//#include <time.h>

unsigned int gpio_no = -1;
static int major = 0;

static void pulse_tasklet(unsigned long data);

struct tasklet_struct pulse_task = 
{
    NULL, 0, ATOMIC_INIT(0), pulse_tasklet, 0,
};

static pPULSE_DEV dev_pulse = NULL;

static int gpo_init(pGPO_INFO gpo_info, unsigned int gpo_no)
{
	int ret = 0;

	memset(gpo_info, 0, sizeof(tpGPO_INFO));
	gpo_info->gpo_no = gpo_no;
	ret = gpio_request(gpo_info->gpo_no, PULSE_GPO_NAME);
	if (ret)
	{
		printk(KERN_ERR "request gpio[%d] error[%d]\n", gpo_no, ret);
		return ret;
	}
	
	gpio_direction_output(gpo_info->gpo_no, 1);
	gpo_info->val = 1;
	
	return 0;
}

static void gpo_set(pGPO_INFO gpo_info, int val)
{
	gpio_set_value(gpo_info->gpo_no, val);
	gpo_info->val = val;
	gpo_info->cnt = (gpo_info->cnt+1)&0x1f;
}

static void gpo_release(pGPO_INFO gpo_info)
{
	gpio_free(gpo_info->gpo_no);
}

static enum hrtimer_restart hrtimer_handler(struct hrtimer *timer)
{
	int ret = 0;
	pTIMER_INFO timer_info = &dev_pulse->timer;

	if (TIMER_OUT_CNT > timer_info->keepalive_cnt)
		timer_info->keepalive_cnt++;
	else
	{
		ret = gpio_get_value_cansleep(dev_pulse->irq_info.irq_gpi_no);
		if (dev_pulse->gpo.val)
			gpo_set(&dev_pulse->gpo, 0);
		else
			gpo_set(&dev_pulse->gpo, 1);
	}

	hrtimer_forward_now(timer, timer_info->kt);

	return HRTIMER_RESTART;
}

static void reset_timer_keepalive_cnt(pTIMER_INFO timer_info)
{
	hrtimer_forward_now(&timer_info->mytimer, timer_info->kt);
	timer_info->keepalive_cnt = 0;
}

static int timer_init(pTIMER_INFO timer_info, int ms)
{
	unsigned int tmp = ms%1000;

	memset(timer_info, 0, sizeof(tpTIMER_INFO));
	timer_info->ms = ms;
	hrtimer_init(&timer_info->mytimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	timer_info->mytimer.function = hrtimer_handler;
	tmp = tmp*1000000;
	timer_info->kt = ktime_set(ms/1000, tmp);
//	hrtimer_start(&timer_info->mytimer, timer_info->kt, HRTIMER_MODE_REL);
	return 0;
}

static int timer_start(pTIMER_INFO timer_info)
{
	return hrtimer_start(&timer_info->mytimer, timer_info->kt, HRTIMER_MODE_REL);
}

static int timer_stop(pTIMER_INFO timer_info)
{
	return hrtimer_cancel(&timer_info->mytimer);
}

static void pulse_tasklet(unsigned long data)
{
    int ret, i;

	ret = gpio_get_value_cansleep(dev_pulse->irq_info.irq_gpi_no);
	if (ret)
		gpo_set(&dev_pulse->gpo, 1);
	else
		gpo_set(&dev_pulse->gpo, 0);

	dev_pulse->gpo.val = ret;
	reset_timer_keepalive_cnt(&dev_pulse->timer);
}

irqreturn_t pulse_irq_handler(int irq, void *dev_id)
{
	tasklet_schedule(&pulse_task);
    return IRQ_HANDLED;
}

int irq_init(pIRQ_INFO irq_info, int gpi_no)
{
	int ret = 0;
	int irq;
	
	memset(irq_info, 0, sizeof(tpIRQ_INFO));

	irq_info->irq_gpi_no = gpi_no;//GPIO_TO_PIN(gpi_no/32, gpi_no%32);
	printk("irq_gpi_no=%d\n", gpi_no);
	irq = gpio_to_irq(gpi_no);
	ret = irq_set_irq_type(irq, IRQ_TYPE_EDGE_BOTH);
	printk("set irq type %d", ret);
	ret = request_irq(irq, pulse_irq_handler, IRQF_DISABLED, "PulseIRQ", NULL);
	printk("req irq %d", ret);
	disable_irq(irq);
	return ret;
}

static int irq_start(pIRQ_INFO irq_info)
{
	int ret = 0;
	int irq = gpio_to_irq(irq_info->irq_gpi_no);
	
	enable_irq(irq);
	return ret;
}

static int irq_stop(pIRQ_INFO irq_info)
{
	int ret = 0;
	int irq = gpio_to_irq(irq_info->irq_gpi_no);
	
	disable_irq(irq);
	return ret;
}

void irq_release(pIRQ_INFO irq_info)
{
	int irq = gpio_to_irq(irq_info->irq_gpi_no);

	disable_irq(irq);
	free_irq(irq, NULL);
}

int pulse_open(struct inode *inode, struct file *filp)
{
    pPULSE_DEV dev;
 
    dev = container_of(inode->i_cdev, tpPULSE_DEV, cdev);
    if (dev->opened)
        return -EINVAL;
    dev->opened = 1;
    filp->private_data = dev;
//    kfifo_reset(&dev->write_fifo);
//    kfifo_reset(&dev->read_fifo);
 
    return 0;
}
 
int pulse_release(struct inode *inode, struct file *filp)
{
    pPULSE_DEV dev = filp->private_data;
 
    dev->opened = 0;

    return 0;
}
//new kernel modify
static int pulse_ioctl(struct file *filp, unsigned
        int cmd, unsigned long arg)
{
    int ret = 0;
    pPULSE_DEV dev = filp->private_data;
 
    switch(cmd) 
    {
    case CMD_PULSE_START:
        {
//	        ret = irq_init(&dev_pulse->irq_info, 96);
			irq_start(&dev_pulse->irq_info);
			printk(KERN_INFO "PULSE_START, %d", ret);
        }
        break;
    case CMD_PULSE_STOP:
        {
//	        irq_release(&dev_pulse->irq_info);
			irq_stop(&dev_pulse->irq_info);
			printk(KERN_INFO "PULSE_STOP");
        }
        break;
    case CMD_PULSE_PR:

        break;
    case CMD_PULSE_STAT_GET:
        {

        }
        break;
    default:
    	{
			printk(KERN_ERR "what's up... |_|!!");
		}
         return  - EINVAL;
    }
    return 0;
}

static const struct file_operations pulse_fops = {
    .owner  = THIS_MODULE,
    .unlocked_ioctl = pulse_ioctl,
    .open = pulse_open,
    .release = pulse_release,
};

static int pulse_mod_init(void)
{
	int ret;
	pPULSE_DEV dev;

	dev_pulse = kzalloc(sizeof(tpPULSE_DEV), GFP_KERNEL);
	if (!dev_pulse)
		return -1;

	gpo_init(&dev_pulse->gpo, 116);			//3-20
	irq_init(&dev_pulse->irq_info, 115);		//3-19
	/*初始化cdev结构*/
	cdev_init(&dev_pulse->cdev, &pulse_fops);
	/* 注册字符设备 */
	alloc_chrdev_region(&dev_pulse->devno, 0, 1, "pulse");
	cdev_add(&dev_pulse->cdev, dev_pulse->devno, 1);

	dev_pulse->pulse_class = class_create(THIS_MODULE, "pulse");
	dev_pulse->pulse_dev = device_create(dev_pulse->pulse_class, NULL, dev_pulse->devno, NULL, "pulse");

	dev_pulse->timer.keepalive_cnt = 0;

	hrtimer_init(&dev_pulse->timer.mytimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	dev_pulse->timer.mytimer.function = hrtimer_handler;
	dev_pulse->timer.kt = ktime_set(0, 500000000);
	hrtimer_start(&dev_pulse->timer.mytimer, dev_pulse->timer.kt, HRTIMER_MODE_REL);

	printk(KERN_INFO "%s ok...%x\n", __FUNCTION__, CMD_PULSE_START);
	return 0;	
}

static void pulse_mod_release(void)
{
	hrtimer_cancel(&dev_pulse->timer.mytimer);
	irq_release(&dev_pulse->irq_info);
	device_unregister(dev_pulse->pulse_dev);//Unload the device under the class
	class_destroy(dev_pulse->pulse_class);//unload the class
	cdev_del(&dev_pulse->cdev);
	/*注销设备*/
	unregister_chrdev_region(dev_pulse->devno, 1);
	gpo_release(&dev_pulse->gpo);
	kfree(dev_pulse);
	/*释放设备号*/
}

module_init(pulse_mod_init);
module_exit(pulse_mod_release);

MODULE_AUTHOR("NJHX SW Dept.");
MODULE_DESCRIPTION("pulse device Driver");
MODULE_LICENSE    ("Dual BSD/GPL");


