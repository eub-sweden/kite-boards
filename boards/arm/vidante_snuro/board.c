#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static int vidante_snuro_init(void)
{
	/* Nothing here yet */
	return 0;
}

SYS_INIT(vidante_snuro_init, PRE_KERNEL_1, 30);
