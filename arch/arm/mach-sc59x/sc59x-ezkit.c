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

static void sc59x_ezkit_restart(enum reboot_mode mode, const char *cmd)
{
	sc59x_restart(mode, cmd);
}

extern void __init adsp_sc5xx_timer_core_init(void);

DT_MACHINE_START(SC59X_DT, "SC59x-EZKIT (Device Tree Support)")
	.map_io		= sc59x_map_io,
	.init_time	= adsp_sc5xx_timer_core_init,
	.init_machine	= sc59x_init,
	.dt_compat	= sc59x_dt_board_compat,
	.restart        = sc59x_ezkit_restart,
MACHINE_END
