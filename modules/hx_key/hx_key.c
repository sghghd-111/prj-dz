#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>
#include "plat/gpio.h"
#include "plat/dma.h"
#include <linux/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/input/matrix_keypad.h>

#include "asm/uaccess.h"
#include "asm/io.h"
#include "asm/atomic.h"

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

/* Configure GPIOs for GPIO Keys */
static struct gpio_keys_button gpio_buttons[] = {
	{
	        .code                   = KEY_UNDO,
	        .gpio                   = GPIO_TO_PIN(3, 16),
	        .active_low             = true,
	        .desc                   = "recover",
	        .type                   = EV_KEY,
	        .debounce_interval		= 2000,
	},
};

static struct gpio_keys_platform_data am335x_evm_gpio_key_info = {
    .buttons        = gpio_buttons,
    .nbuttons       = ARRAY_SIZE(gpio_buttons),
};

static struct platform_device gpio_keys = {
    .name   = "gpio-keys",
    .id     = -1,
    .dev    = {
            .platform_data  = &am335x_evm_gpio_key_info,
    },
};

static int gpio_init(void)
{
	int ret;
	int size = sizeof(gpio_buttons)/sizeof(struct gpio_keys_button);
	int j = 0;

	for (j=0; j<size; j++)
	{
		ret = gpio_request(gpio_buttons[j].gpio, gpio_buttons[j].desc);
		if (ret)
		{
			printk(KERN_ERR "request gpio[%d] error[%d]\n", gpio_buttons[j].gpio, ret);
			return ret;
		}
		
		gpio_direction_input(gpio_buttons[j].gpio);	
		gpio_free(gpio_buttons[j].gpio);
	}
	return 0;
}

static int __init key_mod_init(void)
{
	int err;

	gpio_init();

	err = platform_device_register(&gpio_keys);
	if (err)
			printk(KERN_ERR "failed to register gpio key device\n");

	return err;
}

static void __exit key_mod_release(void)
{
	platform_device_unregister(&gpio_keys);
}

module_init(key_mod_init);
module_exit(key_mod_release);

MODULE_AUTHOR("NJHX SW Dept.");
MODULE_DESCRIPTION("Gpio device Driver");
MODULE_LICENSE("GPL");

