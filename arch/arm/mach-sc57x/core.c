/*
 * core timer and machine init for ADI processor on-chip memory
 *
 * Copyright 2014 - 2018 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <linux/init.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irqdomain.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/io.h>
#include <linux/gfp.h>
#include <linux/bitops.h>
#include <linux/irqchip/arm-gic.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/phy.h>
#include <linux/sched_clock.h>

#include <asm/irq.h>
#include <asm/hardware/cache-l2x0.h>
#include <asm/mach-types.h>

#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>
#include <linux/soc/adi/hardware.h>
#include <linux/soc/adi/cpu.h>
#include <mach/sc57x.h>
#include <mach/irqs.h>

#include "core.h"

static struct map_desc sc57x_io_desc[] __initdata __maybe_unused = {
	{
		.virtual	=  IO_ADDRESS(SYS_MMR_BASE),
		.pfn		= __phys_to_pfn(SYS_MMR_BASE),
		.length		= SYS_MMR_SIZE,
		.type		= MT_DEVICE,
	}, {
		.virtual	=  IO_ADDRESS(SYS_L2_START),
		.pfn		= __phys_to_pfn(SYS_L2_START),
		.length		= SZ_16K,
		.type		= MT_MEMORY_RWX_NONCACHED,
	}, {
		.virtual	=  IO_ADDRESS(SYS_SRAM_BASE),
		.pfn		= __phys_to_pfn(SYS_SRAM_BASE),
		.length		= SYS_SRAM_SIZE,
#ifdef CONFIG_ICC
		.type		= MT_MEMORY_RWX_NONCACHED,
#else
		.type		= MT_MEMORY_RWX,
#endif
	},
};

void __init sc57x_map_io(void)
{
	iotable_init(sc57x_io_desc, ARRAY_SIZE(sc57x_io_desc));
}

void sc57x_restart(enum reboot_mode mode, const char *cmd)
{
	writel(1, __io_address(REG_RCU0_CTL));
}


#include <asm/siginfo.h>
#include <asm/signal.h>

static bool first_fault = true;

static int sc57x_abort_handler(unsigned long addr, unsigned int fsr,
		struct pt_regs *regs)
{
	if (fsr == 0x1c06 && first_fault) {
		first_fault = false;

		/*
		 * These faults with code 0x1c06 happens for no good reason,
		 * possibly left over from the CFE boot loader.
		 */
		pr_warn("External imprecise Data abort at addr=%#lx, fsr=%#x ignored.\n",
				addr, fsr);

		/* Returning non-zero causes fault display and panic */
		return 0;
	}

	/* Others should cause a fault */
	return 1;
}

/* Early initializations */
void __init sc57x_init_early(void)
{
	/* Install our hook */
	hook_fault_code(16 + 6, sc57x_abort_handler, SIGBUS, BUS_OBJERR,
			"imprecise external abort");
}

#if IS_ENABLED(CONFIG_VIDEO_ADI_CAPTURE)
#include <linux/videodev2.h>
#include <media/adi/adi_capture.h>
#include <media/adi/ppi.h>

#if IS_ENABLED(CONFIG_VIDEO_ADV7842)
#include <media/i2c/adv7842.h>
static struct v4l2_input adv7842_inputs[] = {
	{
		.index = 0,
		.name = "Composite",
		.type = V4L2_INPUT_TYPE_CAMERA,
		.std = V4L2_STD_ALL,
		.capabilities = V4L2_IN_CAP_STD,
	},
	{
		.index = 1,
		.name = "S-Video",
		.type = V4L2_INPUT_TYPE_CAMERA,
		.std = V4L2_STD_ALL,
		.capabilities = V4L2_IN_CAP_STD,
	},
	{
		.index = 2,
		.name = "Component",
		.type = V4L2_INPUT_TYPE_CAMERA,
		.capabilities = V4L2_IN_CAP_CUSTOM_TIMINGS,
	},
	{
		.index = 3,
		.name = "VGA",
		.type = V4L2_INPUT_TYPE_CAMERA,
		.capabilities = V4L2_IN_CAP_CUSTOM_TIMINGS,
	},
	{
		.index = 4,
		.name = "HDMI",
		.type = V4L2_INPUT_TYPE_CAMERA,
		.capabilities = V4L2_IN_CAP_CUSTOM_TIMINGS,
	},
};

static struct adi_cap_route adv7842_routes[] = {
	{
		.input = ADV7842_SELECT_SDP_CVBS,
		.output = 0,
		.ppi_control = (PACK_EN | DLEN_8 | EPPI_CTL_FLDSEL
				| EPPI_CTL_ACTIVE656),
	},
	{
		.input = ADV7842_SELECT_SDP_YC,
		.output = 0,
	},
	{
		.input = ADV7842_SELECT_VGA_COMP,
		.output = 0,
	},
	{
		.input = ADV7842_SELECT_VGA_RGB,
		.output = 0,
	},
	{
		.input = ADV7842_SELECT_HDMI_PORT_A,
		.output = 1,
		.ppi_control = (EPPI_CTL_SPLTWRD | PACK_EN | DLEN_16
				| EPPI_CTL_FS1HI_FS2HI | EPPI_CTL_POLC2
				| EPPI_CTL_SYNC2 | EPPI_CTL_NON656),
	},
};

static struct adv7842_platform_data adv7842_data = {
	.bus_order = ADV7842_BUS_ORDER_RGB,
	.op_format_mode_sel = ADV7842_OP_FORMAT_MODE0,
	.ain_sel = ADV7842_AIN10_11_12_NC_SYNC_4_1,
	.mode = ADV7842_MODE_SDP,
	.vid_std_select = ADV7842_SDP_VID_STD_CVBS_SD_4x1,
	.i2c_sdp_io = 0x40,
	.i2c_sdp = 0x41,
	.i2c_cp = 0x42,
	.i2c_vdp = 0x43,
	.i2c_afe = 0x44,
	.i2c_hdmi = 0x45,
	.i2c_repeater = 0x46,
	.i2c_edid = 0x47,
	.i2c_infoframe = 0x48,
	.i2c_cec = 0x49,
	.i2c_avlink = 0x4a,
	.blank_data = 1,
	.llc_dll_phase = 3,
	.hdmi_free_run_enable = 1,
	.sdp_csc_coeff = {
		.manual = true,
		.scaling = 2,
		.A1 = 0x03a7,
		.A2 = 0x1e91,
		.A3 = 0x1de2,
		.A4 = 0x7d00,
		.B1 = 0x03a7,
		.B2 = 0x0761,
		.B3 = 0x0000,
		.B4 = 0x7900,
		.C1 = 0x03a7,
		.C2 = 0x0000,
		.C3 = 0x0429,
		.C4 = 0x7900,
	},
};

static struct adi_capture_config adi_capture_data = {
	.inputs = adv7842_inputs,
	.num_inputs = ARRAY_SIZE(adv7842_inputs),
	.routes = adv7842_routes,
	.board_info = {
		.type = "adv7842",
		.addr = 0x20,
		.platform_data = (void *)&adv7842_data,
	},
	.ppi_control = (PACK_EN | DLEN_8 | EPPI_CTL_FLDSEL
			| EPPI_CTL_ACTIVE656),
};
#endif
#endif

#if IS_ENABLED(CONFIG_VIDEO_ADI_DISPLAY)
#include <linux/videodev2.h>
#include <media/adi/adi_display.h>
#include <media/adi/ppi.h>

#if IS_ENABLED(CONFIG_VIDEO_ADV7343)
#include <media/i2c/adv7343.h>

static struct v4l2_output adv7343_outputs[] = {
	{
		.index = 0,
		.name = "Composite",
		.type = V4L2_OUTPUT_TYPE_ANALOG,
		.std = V4L2_STD_ALL,
		.capabilities = V4L2_OUT_CAP_STD,
	},
	{
		.index = 1,
		.name = "S-Video",
		.type = V4L2_OUTPUT_TYPE_ANALOG,
		.std = V4L2_STD_ALL,
		.capabilities = V4L2_OUT_CAP_STD,
	},
	{
		.index = 2,
		.name = "Component",
		.type = V4L2_OUTPUT_TYPE_ANALOG,
		.std = V4L2_STD_ALL,
		.capabilities = V4L2_OUT_CAP_STD,
	},
};

static struct disp_route adv7343_routes[] = {
	{
		.output = ADV7343_COMPOSITE_ID,
	},
	{
		.output = ADV7343_SVIDEO_ID,
	},
	{
		.output = ADV7343_COMPONENT_ID,
	},
};

static struct adv7343_platform_data adv7343_data = {
	.mode_config = {
		.sleep_mode = false,
		.pll_control = false,
		.dac = {1, 1, 1, 1, 1, 1},
	},
	.sd_config = {
		.sd_dac_out = {0},
	},
};

static struct adi_display_config adi_display_data = {
	.outputs = adv7343_outputs,
	.num_outputs = ARRAY_SIZE(adv7343_outputs),
	.routes = adv7343_routes,
	.board_info = {
		.type = "adv7343",
		.addr = 0x2b,
		.platform_data = (void *)&adv7343_data,
	},
	.ppi_control = (PACK_EN | DLEN_8 | EPPI_CTL_FS1LO_FS2LO
			| EPPI_CTL_POLC3 | EPPI_CTL_BLANKGEN | EPPI_CTL_SYNC2
		| EPPI_CTL_NON656 | EPPI_CTL_DIR),
};
#endif

#if IS_ENABLED(CONFIG_VIDEO_ADV7511)
#include <media/i2c/adv7511.h>

static struct v4l2_output adv7511_outputs[] = {
	{
		.index = 0,
		.name = "HDMI",
		.type = V4L2_INPUT_TYPE_CAMERA,
		.capabilities = V4L2_OUT_CAP_CUSTOM_TIMINGS,
	},
};

static struct disp_route adv7511_routes[] = {
	{
		.output = ADV7511_PORT_HDMI_ID,
	},
};

static struct adv7511_platform_data adv7511_data = {
	.i2c_edid = 0x7e,
	.i2c_cec = 0x78,
	.i2c_pktmem = 0x70,
	.cec_clk = 12000000,
};

static struct adi_display_config adi_display_data = {
	.outputs = adv7511_outputs,
	.num_outputs = ARRAY_SIZE(adv7511_outputs),
	.routes = adv7511_routes,
	.board_info = {
		.type = "adv7511",
		.addr = 0x39,
		.platform_data = (void *)&adv7511_data,
	},
	.ppi_control = (EPPI_CTL_SPLTWRD | PACK_EN | DLEN_16
			| EPPI_CTL_FS1LO_FS2LO | EPPI_CTL_POLC3
			| EPPI_CTL_IFSGEN | EPPI_CTL_SYNC2
			| EPPI_CTL_NON656 | EPPI_CTL_DIR),
};
#endif
#endif

#ifdef CONFIG_OF
static const struct of_dev_auxdata sc57x_auxdata_lookup[] __initconst = {
	OF_DEV_AUXDATA("adi,adi2-pinctrl", 0, "pinctrl-adi2.0", NULL),
	OF_DEV_AUXDATA("arm,adi-uart4", UART0_REVID, "adi-uart4.0", NULL),
	OF_DEV_AUXDATA("arm,adi-watchdog", REG_WDOG0_CTL, "adi-watchdog.0", NULL),
	OF_DEV_AUXDATA("adi,spi3", 0, "adi-spi3.2", NULL),
#if IS_ENABLED(CONFIG_VIDEO_ADI_DISPLAY)
	OF_DEV_AUXDATA("adi,disp", 0x3102D000, "adi_display.0", &adi_display_data),
#endif
#if IS_ENABLED(CONFIG_VIDEO_ADI_CAPTURE)
	OF_DEV_AUXDATA("adi,cap", 0x3102D000, "adi_capture.0", &adi_capture_data),
#endif
	{},
};

static struct of_device_id sc57x_of_bus_ids[] __initdata = {
	{ .compatible = "simple-bus", },
	{},
};
#endif

#define DP83865_PHY_ID          0x20005c7a
#define REG_DP83865_AUX_CTRL    0x12
#define BITP_AUX_CTRL_RGMII_EN  12
#define RGMII_3COM_MODE         3
static int sc57x_dp83865_fixup(struct phy_device *phydev)
{
	int  phy_data = 0;

	phy_data = phy_read(phydev, REG_DP83865_AUX_CTRL);

	/* enable 3com mode for RGMII */
	phy_write(phydev, REG_DP83865_AUX_CTRL,
			     (RGMII_3COM_MODE << BITP_AUX_CTRL_RGMII_EN) | phy_data);

	return 0;
}

static void sc57x_init_ethernet(void)
{
	if (IS_BUILTIN(CONFIG_PHYLIB)) {
		/* select RGMII as the external PHY interface for EMAC0 */
		writel((readl(__io_address(REG_PADS0_PCFG0)) &
		     (~(BITM_PADS_PCFG0_EMACPHYISEL))), __io_address(REG_PADS0_PCFG0));
		writel((readl(__io_address(REG_PADS0_PCFG0)) |
		     (1 << BITP_PADS_PCFG0_EMACPHYISEL) | BITM_PADS_PCFG0_EMACRESET),
		     __io_address(REG_PADS0_PCFG0));
		/* register a fixup to be run for PHY DP83865 */
		phy_register_fixup_for_uid(DP83865_PHY_ID, 0xffffffff,
				sc57x_dp83865_fixup);
	}
}

#if IS_ENABLED(CONFIG_SND_SC5XX_PCM)
static struct platform_device sc57x_pcm = {
	.name = "sc5xx-pcm-audio",
	.id = -1,
};
#endif

static struct platform_device *ezkit_devices[] __initdata = {
#if IS_ENABLED(CONFIG_SND_SC5XX_PCM)
	&sc57x_pcm,
#endif
};

void __init sc57x_init(void)
{
#ifdef CONFIG_CACHE_L2X0
	l2x0_of_init(0, ~0UL);
#endif

	pr_info("%s: registering device resources\n", __func__);

#ifdef CONFIG_OF
	of_platform_populate(NULL, sc57x_of_bus_ids,
				sc57x_auxdata_lookup, NULL);
#endif
	sc57x_init_ethernet();
	platform_add_devices(ezkit_devices, ARRAY_SIZE(ezkit_devices));
}

static void __iomem *spu_base;

void set_spu_securep_msec(uint16_t n, bool msec)
{
	void __iomem *p = (void __iomem *)(spu_base + 0xA00 + 4 * n);
	u32 securep = ioread32(p);

	if (msec)
		iowrite32(securep | 0x3, p);
	else
		iowrite32(securep & ~0x3, p);
}
EXPORT_SYMBOL(set_spu_securep_msec);

static int __init spu_init(void)
{
	spu_base = ioremap(REG_SPU0_CTL, 0x1000);

	return 0;
}
arch_initcall(spu_init);
