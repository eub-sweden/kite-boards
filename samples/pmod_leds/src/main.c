/*
 * Copyright (c) 2023 Elektronikutvecklingsbyrån EUB AB
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/led.h>
#include <zephyr/kernel.h>

static const struct device *led_devs[] = {
	/* TODO: Check if node exists or otherwise handle boards
	 * with different numbers of Pmod connectors. */
	DEVICE_DT_GET(DT_PATH(leds_pmod1)),
	DEVICE_DT_GET(DT_PATH(leds_pmod2)),
	DEVICE_DT_GET(DT_PATH(leds_pmod3)),
	DEVICE_DT_GET(DT_PATH(leds_pmod4)),
	DEVICE_DT_GET(DT_PATH(leds_pmod5)),
};

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
							      {0});
static struct gpio_callback button_cb_data;
static K_SEM_DEFINE(button_sem, 0, 1);

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	k_sem_give(&button_sem);
}

void button_wait(k_timeout_t timeout)
{
	k_sem_reset(&button_sem);
	k_sem_take(&button_sem, timeout);
}

int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&button)) {
		printk("Error: button device %s is not ready\n",
		       button.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button.port->name, button.pin);
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&button,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button.port->name, button.pin);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);
	printk("Set up button at %s pin %d\n", button.port->name, button.pin);

	size_t group = 0;

	while (1) {
		const struct device *dev = led_devs[group];

		/* Light first led in the group while waiting for a button press. */
		led_on(dev, 0);

		button_wait(K_FOREVER);
		printk("Button pressed\n");

		/* */
		for (uint32_t i = 0; i < 8; i++) {
			ret = led_on(dev, i);
			if (ret) {
				/* Break on error to handle 6-pin Pmod connectors gracefully. */
				break;
			}
			k_sleep(K_MSEC(200));
			led_off(dev, i);
		}

		group++;
		if (group == ARRAY_SIZE(led_devs)) {
			group = 0;
		}
	}

	return 0;
}
