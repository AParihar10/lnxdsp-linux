/*
 * machine start entries for ADI processor on-chip memory
 *
 * Copyright 2018 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <linux/init.h>
#include <linux/device.h>
#include <linux/io.h>

#include <linux/soc/adi/hardware.h>
#include <asm/irq.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/sc59x.h>
#include <sound/sc5xx-dai.h>
#include <linux/of_gpio.h>
#include <mach/gpio.h>

#include "core.h"

void pads_init(void)
{
	writel(0xffffffff, __io_address(REG_PADS0_DAI0_IE));
	writel(0xffffffff, __io_address(REG_PADS0_DAI1_IE));
}
EXPORT_SYMBOL(pads_init);

static const char * const sc59x_dt_board_compat[] __initconst = {
	"adi,sc59x",
	NULL
};

/**
 * softconfig_of_set_active_pin_output()
 * @dev:	device that will be interacted with
 * @np:		device node to get pin property from
 * @propname:	property name of gpio list
 * @index:		index of the specified GPIO
 * @pin:		pin number pointer to fill in
 * @active_flag:    active flag pointer to fill in
 * @active:		if active as the gpio flag property in dts
 *
 * Set the state of the specified active pin for peripheral from softconfig
 * with a given gpio list property name, index of the specified GPIO and a
 * given node. If @dev is NULL, requested GPIO will be freed at the end of this
 * function, otherwise requested GPIO in this function will be automatically
 * freed on driver detach. It returns error if it fails otherwise 0 on success.
 */
int softconfig_of_set_active_pin_output(struct device *dev,
				struct device_node *np, const char *propname, int index,
				int *pin, int *active_flag, bool active)
{
	int ret = 0;
	int pin_num, flag_active_low, output;
	bool need_free = true;
	enum of_gpio_flags flag;

	pin_num = of_get_named_gpio_flags(np, propname, index, &flag);
	if (!gpio_is_valid(pin_num)) {
		pr_err("%s, invalid %s %d\n", __func__, propname, pin_num);
		return -ENODEV;
	}

	if (dev == NULL)
		ret = gpio_request(pin_num, propname);
	else {
		need_free = false;
		ret = devm_gpio_request(dev, pin_num, dev_name(dev));
	}
	if (ret == -EBUSY) {
		need_free = false;
		pr_debug("%s, %s %d is busy now!\n", __func__, propname, pin_num);
	} else if (ret < 0) {
		pr_err("%s, can't request %s %d, err: %d\n",
							__func__, propname, pin_num, ret);
		return ret;
	}

	flag_active_low = (flag & OF_GPIO_ACTIVE_LOW ? 1 : 0);

	if (active)
		output = flag_active_low ? 0 : 1;
	else
		output = flag_active_low ? 1 : 0;
	ret = gpio_direction_output(pin_num, output);
	if (ret < 0) {
		pr_err("%s, can't set direction for %s pin %d, err: %d\n",
							__func__, propname, pin_num, ret);
		goto out;
	}

	*pin = pin_num;
	*active_flag = flag_active_low;

out:
	if (need_free)
		gpio_free(pin_num);
	return ret;
}
EXPORT_SYMBOL(softconfig_of_set_active_pin_output);

/**
 * softconfig_of_set_group_active_pins_output()
 * @dev:	device that will be interacted with
 * @np:		device node to get pin property from
 * @propname:	property name of gpio list
 * @active:		if active as the gpio flag property in dts
 *
 * Set the state of the group active pins for peripheral from softconfig with
 * a given gpio list property name and a given node. If @dev is NULL, requested
 * GPIO will be freed at the end of this function, otherwise requested GPIO in
 * this function will be automatically freed on driver detach.
 * It returns error if it fails otherwise 0 on success.
 */
int softconfig_of_set_group_active_pins_output(struct device *dev,
				struct device_node *np, const char *propname, bool active)
{
	int ret = 0;
	int i, nb = 0;

	nb = of_gpio_named_count(np, propname);
	for (i = 0; i < nb; i++) {
		int pin, flag;
		ret = softconfig_of_set_active_pin_output(
						dev, np, propname, i, &pin, &flag, active);
		if (ret)
			return ret;
	}

	return ret;
}
EXPORT_SYMBOL(softconfig_of_set_group_active_pins_output);

/**
 * Active all reboot-pins from softconfig as listed in the board dts file
 */
static void sc59x_ezkit_restart(enum reboot_mode mode, const char *cmd)
{
	struct device_node *np;

	if (!of_machine_is_compatible(sc59x_dt_board_compat[0]))
		goto restart_out;

	np = of_find_node_by_name(NULL, "softconfig_default");
	if (!np)
		goto restart_out;
	softconfig_of_set_group_active_pins_output(
						NULL, np, "reboot-pins", true);
	of_node_put(np);

restart_out:
	sc59x_restart(mode, cmd);
}

extern void __init adsp_sc5xx_timer_core_init(void);

DT_MACHINE_START(SC59X_DT, "SC59x-EZKIT (Device Tree Support)")
	.map_io		= sc59x_map_io,
	.init_early	= sc59x_init_early,
	.init_time	= adsp_sc5xx_timer_core_init,
	.init_machine	= sc59x_init,
	.dt_compat	= sc59x_dt_board_compat,
	.restart        = sc59x_ezkit_restart,
MACHINE_END
