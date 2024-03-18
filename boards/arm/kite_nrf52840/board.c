#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#if !defined(CONFIG_REGULATOR)
/* Note: PMOD1 has an enable FET which generally should be enabled at boot. If
 * one uses the regulator driver, this will be handled at boot, if the regulator
 * DTS node has the `regulator-boot-on` property. But we can't trust
 * CONFIG_REGULATOR=y to be true always (partly since it's a pretty bad API
 * overall and partly because it introduces dependencies that are undesirable
 * for MCUBoot). This block ensures the `regulator-boot-on` property is
 * satisfied even when CONFIG_REGULATOR=n.
 */

static int enable_regulator_at_boot(const struct device *unused)
{
	if (DT_NODE_HAS_PROP(DT_NODELABEL(en_pmod1), regulator_boot_on)) {
		const struct gpio_dt_spec power_en =
			GPIO_DT_SPEC_GET(DT_NODELABEL(en_pmod1), enable_gpios);
		gpio_pin_configure_dt(&power_en, GPIO_OUTPUT_ACTIVE);
	}
	return 0;
}

SYS_INIT(enable_regulator_at_boot, PRE_KERNEL_1, 30);

#endif /* !CONFIG_REGULATOR */
