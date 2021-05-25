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

#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/sc59x.h>
#include <sound/sc5xx-dai.h>
#include <sound/sc5xx-sru.h>
#include <linux/of_gpio.h>
#include <mach/gpio.h>

#include "core.h"

#define REG_DAI1_EXTD_CLK0                   0x310CA000            /*  DAI1 Extended Clock Routing Control Register 0 */
#define REG_DAI1_EXTD_CLK1                   0x310CA004            /*  DAI1 Extended Clock Routing Control Register 1 */
#define REG_DAI1_EXTD_CLK2                   0x310CA008            /*  DAI1 Extended Clock Routing Control Register 2 */
#define REG_DAI1_EXTD_CLK3                   0x310CA00C            /*  DAI1 Extended Clock Routing Control Register 3 */
#define REG_DAI1_EXTD_CLK4                   0x310CA010            /*  DAI1 Extended Clock Routing Control Register 4 */
#define REG_DAI1_EXTD_CLK5                   0x310CA014            /*  DAI1 Extended Clock Routing Control Register 5 */
#define REG_DAI1_EXTD_DAT0                   0x310CA018            /*  DAI1 Extended Serial Data Routing Control Register 0 */
#define REG_DAI1_EXTD_DAT1                   0x310CA01C            /*  DAI1 Extended Serial Data Routing Control Register 1 */
#define REG_DAI1_EXTD_DAT2                   0x310CA020            /*  DAI1 Extended Serial Data Routing Control Register 2 */
#define REG_DAI1_EXTD_DAT3                   0x310CA024            /*  DAI1 Extended Serial Data Routing Control Register 3 */
#define REG_DAI1_EXTD_DAT4                   0x310CA028            /*  DAI1 Extended Serial Data Routing Control Register 4 */
#define REG_DAI1_EXTD_DAT5                   0x310CA02C            /*  DAI1 Extended Serial Data Routing Control Register 5 */
#define REG_DAI1_EXTD_DAT6                   0x310CA030            /*  DAI1 Extended Serial Data Routing Control Register 6 */
#define REG_DAI1_EXTD_FS0                    0x310CA034            /*  DAI1 Extended Frame Sync Routing Control Register 0 */
#define REG_DAI1_EXTD_FS1                    0x310CA038            /*  DAI1 Extended Frame Sync Routing Control Register 1 */
#define REG_DAI1_EXTD_FS2                    0x310CA03C            /*  DAI1 Extended Frame Sync Routing Control Register 2 */
#define REG_DAI1_EXTD_FS4                    0x310CA044            /*  DAI1 Extended Frame Sync Routing Control Register 4 */
#define REG_DAI1_EXTD_PIN0                   0x310CA048            /*  DAI1 Extended Pin Buffer Assignment Register 0 */
#define REG_DAI1_EXTD_PIN1                   0x310CA04C            /*  DAI1 Extended Pin Buffer Assignment Register 1 */
#define REG_DAI1_EXTD_PIN2                   0x310CA050            /*  DAI1 Extended Pin Buffer Assignment Register 2 */
#define REG_DAI1_EXTD_PIN3                   0x310CA054            /*  DAI1 Extended Pin Buffer Assignment Register 3 */
#define REG_DAI1_EXTD_PIN4                   0x310CA058            /*  DAI1 Extended Pin Buffer Assignment Register 4 */
#define REG_DAI1_EXTD_MISC0                  0x310CA05C            /*  DAI1 Extended Miscellaneous Control Register 0 */
#define REG_DAI1_EXTD_MISC1                  0x310CA060            /*  DAI1 Extended Miscellaneous Control Register 1 */
#define REG_DAI1_EXTD_MISC2                  0x310CA064            /*  DAI1 Extended Miscellaneous Control Register 2 */
#define REG_DAI1_EXTD_PBEN0                  0x310CA068            /*  DAI1 Extended Pin Buffer Enable Register 0 */
#define REG_DAI1_EXTD_PBEN1                  0x310CA06C            /*  DAI1 Extended Pin Buffer Enable Register 1 */
#define REG_DAI1_EXTD_PBEN2                  0x310CA070            /*  DAI1 Extended Pin Buffer Enable Register 2 */
#define REG_DAI1_EXTD_PBEN3                  0x310CA074            /*  DAI1 Extended Pin Buffer Enable Register 3 */
#define REG_DAI1_CLK0                        0x310CA0C0            /*  DAI1 Clock Routing Control Register 0 */
#define REG_DAI1_CLK1                        0x310CA0C4            /*  DAI1 Clock Routing Control Register 1 */
#define REG_DAI1_CLK2                        0x310CA0C8            /*  DAI1 Clock Routing Control Register 2 */
#define REG_DAI1_CLK3                        0x310CA0CC            /*  DAI1 Clock Routing Control Register 3 */
#define REG_DAI1_CLK4                        0x310CA0D0            /*  DAI1 Clock Routing Control Register 4 */
#define REG_DAI1_CLK5                        0x310CA0D4            /*  DAI1 Clock Routing Control Register 5 */
#define REG_DAI1_DAT0                        0x310CA100            /*  DAI1 Serial Data Routing Control Register 0 */
#define REG_DAI1_DAT1                        0x310CA104            /*  DAI1 Serial Data Routing Control Register 1 */
#define REG_DAI1_DAT2                        0x310CA108            /*  DAI1 Serial Data Routing Control Register 2 */
#define REG_DAI1_DAT3                        0x310CA10C            /*  DAI1 Serial Data Routing Control Register 3 */
#define REG_DAI1_DAT4                        0x310CA110            /*  DAI1 Serial Data Routing Control Register 4 */
#define REG_DAI1_DAT5                        0x310CA114            /*  DAI1 Serial Data Routing Control Register 5 */
#define REG_DAI1_DAT6                        0x310CA118            /*  DAI1 Serial Data Routing Control Register 6 */
#define REG_DAI1_FS0                         0x310CA140            /*  DAI1 Frame Sync Routing Control Register 0 */
#define REG_DAI1_FS1                         0x310CA144            /*  DAI1 Frame Sync Routing Control Register 1 */
#define REG_DAI1_FS2                         0x310CA148            /*  DAI1 Frame Sync Routing Control Register 2 */
#define REG_DAI1_FS4                         0x310CA150            /*  DAI1 Frame Sync Routing Control Register 4 */
#define REG_DAI1_PIN0                        0x310CA180            /*  DAI1 Pin Buffer Assignment Register 0 */
#define REG_DAI1_PIN1                        0x310CA184            /*  DAI1 Pin Buffer Assignment Register 1 */
#define REG_DAI1_PIN2                        0x310CA188            /*  DAI1 Pin Buffer Assignment Register 2 */
#define REG_DAI1_PIN3                        0x310CA18C            /*  DAI1 Pin Buffer Assignment Register 3 */
#define REG_DAI1_PIN4                        0x310CA190            /*  DAI1 Pin Buffer Assignment Register 4 */
#define REG_DAI1_MISC0                       0x310CA1C0            /*  DAI1 Miscellaneous Control Register 0 */
#define REG_DAI1_MISC1                       0x310CA1C4            /*  DAI1 Miscellaneous Control Register 1 */
#define REG_DAI1_MISC2                       0x310CA1C8            /*  DAI1 Miscellaneous Control Register 1 */
#define REG_DAI1_PBEN0                       0x310CA1E0            /*  DAI1 Pin Buffer Enable Register 0 */
#define REG_DAI1_PBEN1                       0x310CA1E4            /*  DAI1 Pin Buffer Enable Register 1 */
#define REG_DAI1_PBEN2                       0x310CA1E8            /*  DAI1 Pin Buffer Enable Register 2 */
#define REG_DAI1_PBEN3                       0x310CA1EC            /*  DAI1 Pin Buffer Enable Register 3 */
#define REG_DAI1_IMSK_FE                     0x310CA200            /*  DAI1 Falling-Edge Interrupt Mask Register */
#define REG_DAI1_IMSK_RE                     0x310CA204            /*  DAI1 Rising-Edge Interrupt Mask Register */
#define REG_DAI1_IMSK_PRI                    0x310CA210            /*  DAI1 Core Interrupt Priority Assignment Register */
#define REG_DAI1_IRPTL_H                     0x310CA220            /*  DAI1 High Priority Interrupt Latch Register */
#define REG_DAI1_IRPTL_L                     0x310CA224            /*  DAI1 Low Priority Interrupt Latch Register */
#define REG_DAI1_IRPTL_HS                    0x310CA230            /*  DAI1 Shadow High Priority Interrupt Latch Register */
#define REG_DAI1_IRPTL_LS                    0x310CA234            /*  DAI1 Shadow Low Priority Interrupt Latch Register */
#define REG_DAI1_PIN_STAT                    0x310CA2E4            /*  DAI1 Pin Status Register */
#define REG_DAI1_GBL_SP_EN                   0x310CA2E8            /*  DAI1 Global SPORT Enable Register */
#define REG_DAI1_GBL_INT_EN                  0x310CA2EC            /*  DAI1 Global SPORT Interrupt Grouping Register */

#if IS_ENABLED(CONFIG_SND_SC5XX_SPORT)
void sru_init(void)
{
	/* Connections:
	*    DAI1_PB03_O - SPT4A_CLK_I
	*    DAI1_PB13_O - SPT5A_CLK_I
	*    DAI1_PB07_O - SPT4B_CLK_I
	*    DAI1_PB13_O - SPT5B_CLK_I
	*    DAI1_PB19_O - SPT6B_CLK_I
	*    DAI1_PB19_O - SPT6A_CLK_I
	*/
	SRU_DIRECT(REG_DAI1_CLK0,0x252630c2);
	SRU_DIRECT(REG_DAI1_EXTD_CLK0,0x00000000);

	/* Connections:
	*    DAI1_LOW - SRC4_CLK_IP_I
	*    DAI1_LOW - SRC6_CLK_OP_I
	*    DAI1_LOW - SRC4_CLK_OP_I
	*    DAI1_LOW - SRC5_CLK_IP_I
	*    DAI1_LOW - SRC6_CLK_IP_I
	*    DAI1_LOW - SRC5_CLK_OP_I
	*/
	SRU_DIRECT(REG_DAI1_CLK1,0x3def7bde);
	SRU_DIRECT(REG_DAI1_EXTD_CLK1,0x00000000);

	/* Connections:
	*    DAI1_LOW - SRC7_CLK_IP_I
	*    DAI1_LOW - PDM1_CLK0_I
	*    DAI1_LOW - SRC7_CLK_OP_I
	*    DAI1_LOW - PDM1_BCLK_I
	*    DAI1_LOW - SPDIF1_TX_CLK_I
	*/
	SRU_DIRECT(REG_DAI1_CLK2,0x3def7bde);
	SRU_DIRECT(REG_DAI1_EXTD_CLK2,0x00000000);

	/* Connections:
	*    DAI1_LOW - SPDIF1_TX_HFCLK_I
	*/
	SRU_DIRECT(REG_DAI1_CLK3,0x3def7bde);
	SRU_DIRECT(REG_DAI1_EXTD_CLK3,0x00000000);

	/* Connections:
	*    DAI1_LOW - PCG0_EXTCLKC_I
	*    DAI1_LOW - PCG0_EXTCLKD_I
	*    DAI1_LOW - SPDIF1_TX_EXT_SYNC_I
	*    DAI1_LOW - PCG0_SYNC_CLKC_I
	*    DAI1_LOW - PCG0_SYNC_CLKD_I
	*/
	SRU_DIRECT(REG_DAI1_CLK4,0x3def7bde);
	SRU_DIRECT(REG_DAI1_EXTD_CLK4,0x00000000);

	/* Connections:
	*    DAI1_LOW - PCG0_SYNC_CLKG_I
	*    DAI1_LOW - PCG0_SYNC_CLKH_I
	*    DAI1_LOW - PCG0_EXTCLKG_I
	*    DAI1_LOW - PCG0_EXTCLKH_I
	*    DAI1_LOW - SPT7B_CLK_I
	*    DAI1_LOW - SPT7A_CLK_I
	*/
	SRU_DIRECT(REG_DAI1_CLK5,0x3def7bde);
	SRU_DIRECT(REG_DAI1_EXTD_CLK5,0x00000000);

	/* Connections:
	*    DAI1_PB06_O - SPT4B_D1_I
	*    DAI1_PB05_O - SPT4B_D0_I
	*    DAI1_PB09_O - SPT5A_D0_I
	*    DAI1_PB02_O - SPT4A_D1_I
	*    DAI1_PB01_O - SPT4A_D0_I
	*/
	SRU_DIRECT(REG_DAI1_DAT0,0x08144040);
	SRU_DIRECT(REG_DAI1_EXTD_DAT0,0x00000000);

	/* Connections:
	*    DAI1_PB10_O - SPT5A_D1_I
	*    DAI1_PB11_O - SPT5B_D0_I
	*    DAI1_PB15_O - SPT6A_D0_I
	*    DAI1_PB12_O - SPT5B_D1_I
	*    DAI1_PB16_O - SPT6A_D1_I
	*/
	SRU_DIRECT(REG_DAI1_DAT1,0x0f38b289);
	SRU_DIRECT(REG_DAI1_EXTD_DAT1,0x00000000);

	/* Connections:
	*    DAI1_PB01_O - SRC5_DAT_IP_I
	*    DAI1_PB18_O - SPT6B_D1_I
	*    DAI1_PB17_O - SPT6B_D0_I
	*    DAI1_PB01_O - SRC4_DAT_IP_I
	*    DAI1_PB01_O - SRC6_DAT_IP_I
	*/
	SRU_DIRECT(REG_DAI1_DAT2,0x00000450);
	SRU_DIRECT(REG_DAI1_EXTD_DAT2,0x00000000);

	/* Connections:
	*    DAI1_PB01_O - SRC4_DAT_TDM_OP_I
	*    DAI1_PB01_O - SRC5_DAT_TDM_OP_I
	*    DAI1_PB01_O - SRC7_DAT_IP_I
	*    DAI1_PB01_O - SRC7_DAT_TDM_OP_I
	*    DAI1_PB01_O - SRC6_DAT_TDM_OP_I
	*/
	SRU_DIRECT(REG_DAI1_DAT3,0x00000000);
	SRU_DIRECT(REG_DAI1_EXTD_DAT3,0x00000000);

	/* Connections:
	*    DAI1_PB01_O - PDM1_DAT1_I
	*    DAI1_PB01_O - PDM1_DAT0_I
	*    DAI1_PB01_O - SPDIF1_TX_DAT_I
	*/
	SRU_DIRECT(REG_DAI1_DAT4,0x00000000);
	SRU_DIRECT(REG_DAI1_EXTD_DAT4,0x00000000);

	/* Connections:
	*    DAI1_PB01_O - SPDIF1_RX_I
	*/
	SRU_DIRECT(REG_DAI1_DAT5,0x00000000);
	SRU_DIRECT(REG_DAI1_EXTD_DAT5,0x00000000);

	/* Connections:
	*    DAI1_LOW - SPT7B_D1_I
	*    DAI1_LOW - SPT7A_D1_I
	*    DAI1_LOW - SPT7A_D0_I
	*    DAI1_LOW - SPT7B_D0_I
	*/
	SRU_DIRECT(REG_DAI1_DAT6,0x00fbefbe);
	SRU_DIRECT(REG_DAI1_EXTD_DAT6,0x00000000);

	/* Connections:
	*    DAI1_PB20_O - SPT6B_FS_I
	*    DAI1_PB08_O - SPT4B_FS_I
	*    DAI1_PB14_O - SPT5A_FS_I
	*    DAI1_PB20_O - SPT6A_FS_I
	*    DAI1_PB14_O - SPT5B_FS_I
	*    DAI1_PB04_O - SPT4A_FS_I
	*/
	SRU_DIRECT(REG_DAI1_FS0,0x2736b4e3);
	SRU_DIRECT(REG_DAI1_EXTD_FS0,0x00000000);

	/* Connections:
	*    DAI1_LOW - SRC4_FS_OP_I
	*    DAI1_LOW - SRC6_FS_IP_I
	*    DAI1_LOW - SRC5_FS_IP_I
	*    DAI1_LOW - SRC6_FS_OP_I
	*    DAI1_LOW - SRC4_FS_IP_I
	*    DAI1_LOW - SRC5_FS_OP_I
	*/
	SRU_DIRECT(REG_DAI1_FS1,0x3def7bde);
	SRU_DIRECT(REG_DAI1_EXTD_FS1,0x00000000);

	/* Connections:
	*    DAI1_LOW - SPDIF1_TX_FS_I
	*    DAI1_LOW - SRC7_FS_OP_I
	*    DAI1_LOW - SRC7_FS_IP_I
	*/
	SRU_DIRECT(REG_DAI1_FS2,0x3def7bde);
	SRU_DIRECT(REG_DAI1_EXTD_FS2,0x00000000);

	/* Connections:
	*    DAI1_LOW - PDM1_LRCLK_I
	*    DAI1_LOW - SPT7B_FS_I
	*    DAI1_LOW - TM0_ACI15_I
	*    DAI1_LOW - SPT7A_FS_I
	*/
	SRU_DIRECT(REG_DAI1_FS4,0x000f7bde);
	SRU_DIRECT(REG_DAI1_EXTD_FS4,0x00000000);

	/* Connections:
	*    DAI1_LOW - DAI1_INT_8_I
	*    DAI1_LOW - DAI1_INT_9_I
	*    DAI1_LOW - DAI1_MISCA2_I
	*    DAI1_LOW - DAI1_INV_MISCA5_I
	*    DAI1_LOW - DAI1_MISCA1_I
	*    DAI1_LOW - DAI1_MISCA3_I
	*    DAI1_LOW - DAI1_INV_MISCA4_I
	*    DAI1_LOW - DAI1_MISCA0_I
	*    DAI1_LOW - DAI1_INT_6_I
	*    DAI1_LOW - DAI1_INT_7_I
	*/
	SRU_DIRECT(REG_DAI1_MISC0,0x3def7bde);
	SRU_DIRECT(REG_DAI1_EXTD_MISC0,0x00000000);

	/* Connections:
	*    DAI1_LOW - DAI1_INT_0_I
	*    DAI1_LOW - DAI1_INT_1_I
	*    DAI1_LOW - DAI1_INT_2_I
	*    DAI1_LOW - DAI1_INT_3_I
	*    DAI1_LOW - DAI1_INT_4_I
	*    DAI1_LOW - DAI1_INT_5_I
	*/
	SRU_DIRECT(REG_DAI1_MISC1,0x3def7bde);
	SRU_DIRECT(REG_DAI1_EXTD_MISC1,0x00000000);

	/* Connections:
	*    DAI1_LOW - PCG0_HWC_TRIG_I
	*    DAI1_LOW - PCG0_HWH_TRIG_I
	*    DAI1_LOW - PCG0_HWD_TRIG_I
	*    DAI1_LOW - PCG0_HWG_TRIG_I
	*/
	SRU_DIRECT(REG_DAI1_MISC2,0x000f7bde);
	SRU_DIRECT(REG_DAI1_EXTD_MISC2,0x00000000);

	/* Connections:
	*    SPT4B_D0_PBEN_O - DAI1_PBEN05_I
	*    SPT4A_D0_PBEN_O - DAI1_PBEN01_I
	*    SPT4A_D1_PBEN_O - DAI1_PBEN02_I
	*    SPT4A_CLK_PBEN_O - DAI1_PBEN03_I
	*    SPT4A_FS_PBEN_O - DAI1_PBEN04_I
	*/
	SRU_DIRECT(REG_DAI1_PBEN0,0x0e2482ca);
	SRU_DIRECT(REG_DAI1_EXTD_PBEN0,0x00000000);

	/* Connections:
	*    SPT5A_D1_PBEN_O - DAI1_PBEN10_I
	*    SPT5A_D0_PBEN_O - DAI1_PBEN09_I
	*    SPT4B_D1_PBEN_O - DAI1_PBEN06_I
	*    SPT4B_FS_PBEN_O - DAI1_PBEN08_I
	*    SPT4B_CLK_PBEN_O - DAI1_PBEN07_I
	*/
	SRU_DIRECT(REG_DAI1_PBEN1,0x1348d30f);
	SRU_DIRECT(REG_DAI1_EXTD_PBEN1,0x00000000);

	/* Connections:
	*    SPT5B_D0_PBEN_O - DAI1_PBEN11_I
	*    SPT5B_CLK_PBEN_O - DAI1_PBEN13_I
	*    SPT5B_D1_PBEN_O - DAI1_PBEN12_I
	*    SPT6A_D0_PBEN_O - DAI1_PBEN15_I
	*    SPT5B_FS_PBEN_O - DAI1_PBEN14_I
	*/
	SRU_DIRECT(REG_DAI1_PBEN2,0x1a5545d6);
	SRU_DIRECT(REG_DAI1_EXTD_PBEN2,0x00000000);

	/* Connections:
	*    SPT6B_FS_PBEN_O - DAI1_PBEN20_I
	*    SPT6B_D0_PBEN_O - DAI1_PBEN17_I
	*    SPT6A_D1_PBEN_O - DAI1_PBEN16_I
	*    SPT6B_CLK_PBEN_O - DAI1_PBEN19_I
	*    SPT6B_D1_PBEN_O - DAI1_PBEN18_I
	*/
	SRU_DIRECT(REG_DAI1_PBEN3,0x1d71f79b);
	SRU_DIRECT(REG_DAI1_EXTD_PBEN3,0x00000000);

	/* Connections:
	*    SPT4A_FS_O - DAI1_PB04_I
	*    SPT4A_D1_O - DAI1_PB02_I
	*    SPT4A_CLK_O - DAI1_PB03_I
	*    SPT4A_D0_O - DAI1_PB01_I
	*/
	SRU_DIRECT(REG_DAI1_PIN0,0x04c80a94);
	SRU_DIRECT(REG_DAI1_EXTD_PIN0,0x00000000);

	/* Connections:
	*    SPT4B_FS_O - DAI1_PB08_I
	*    SPT4B_D1_O - DAI1_PB06_I
	*    SPT4B_CLK_O - DAI1_PB07_I
	*    SPT4B_D0_O - DAI1_PB05_I
	*/
	SRU_DIRECT(REG_DAI1_PIN1,0x04e84b96);
	SRU_DIRECT(REG_DAI1_EXTD_PIN1,0x00000000);

	/* Connections:
	*    SPT5A_D0_O - DAI1_PB09_I
	*    SPT5B_D1_O - DAI1_PB12_I
	*    SPT5B_D0_O - DAI1_PB11_I
	*    SPT5A_D1_O - DAI1_PB10_I
	*/
	SRU_DIRECT(REG_DAI1_PIN2,0x03668c98);
	SRU_DIRECT(REG_DAI1_EXTD_PIN2,0x00000000);

	/* Connections:
	*    SPT6A_D1_O - DAI1_PB16_I
	*    SPT6A_D0_O - DAI1_PB15_I
	*    SPT5B_FS_O - DAI1_PB14_I
	*    SPT5B_CLK_O - DAI1_PB13_I
	*/
	SRU_DIRECT(REG_DAI1_PIN3,0x03a714a3);
	SRU_DIRECT(REG_DAI1_EXTD_PIN3,0x00000000);

	/* Connections:
	*    SPT6B_CLK_O - DAI1_PB19_I
	*    SPT6B_D1_O - DAI1_PB18_I
	*    SPT6B_D0_O - DAI1_PB17_I
	*    DAI1_LOW - DAI1_INV_PB20_I
	*    SPT6B_FS_O - DAI1_PB20_I
	*    DAI1_LOW - DAI1_INV_PB19_I
	*/
	SRU_DIRECT(REG_DAI1_PIN4,0x05694f9e);
	SRU_DIRECT(REG_DAI1_EXTD_PIN4,0x00000000);

	/* set DAI1_PIN05 to input */
	//SRU(1, LOW, DAI1_PBEN05_I);
	/* route DAI1_PIN05 to SPT4_ACLK */
	//SRU(1, DAI1_PB05_O, SPT4_ACLK_I);
	//SRU(1, DAI1_PB05_O, SPT4_BCLK_I);
	/*set DAI1_PIN04 to input */
	//SRU(1, LOW, DAI1_PBEN04_I);
	/* route DAI1_PIN04 to SPT4_AFS */
	//SRU(1, DAI1_PB04_O, SPT4_AFS_I);
	//SRU(1, DAI1_PB04_O, SPT4_BFS_I);
	/* set DAI1_PIN01 to output */
	//SRU(1, HIGH, DAI1_PBEN01_I);
	/* route SPT4_AD0 to DAI1_PIN01 */
	//SRU(1, SPT4_AD0_O, DAI1_PB01_I);
	/* set DAI1_PIN02 to output */
	//SRU(1, HIGH, DAI1_PBEN02_I);
	/* route SPT4_AD1 to DAI1_PIN02 */
	//SRU(1, SPT4_AD1_O, DAI1_PB02_I);

	/* set DAI1_PIN12 to input */
	//SRU(1, LOW, DAI1_PBEN12_I);
	/* route DAI1_PIN12 to SPT4_BCLK */
	//SRU(1, DAI1_PB12_O, SPT4_BCLK_I);
	/*set DAI1_PIN20 to input */
	//SRU(1, LOW, DAI1_PBEN20_I);
	/* route DAI1_PIN20 to SPT4_BFS */
	//SRU(1, DAI1_PB20_O, SPT4_BFS_I);
	/* set DAI1_PIN06 to input */
	//SRU(1, LOW, DAI1_PBEN06_I);
	/* route DAI1_PIN06 to SPT4_BD0 */
	//SRU(1, DAI1_PB06_O, SPT4_BD0_I);
}
EXPORT_SYMBOL(sru_init);
#endif

void pads_init(void)
{
	writel(0xffffffff, __io_address(REG_PADS0_DAI0_IE));
	writel(0xffffffff, __io_address(REG_PADS0_DAI1_IE));
}
EXPORT_SYMBOL(pads_init);

#ifdef CONFIG_MACH_SC59X_DT
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

/**
 * Inactive all boot-pins from softconfig as listed in the board dts file
 */
static int __init sc59x_softconfig_init(void)
{
	int ret = 0;
	struct device_node *np;

	if (!of_machine_is_compatible(sc59x_dt_board_compat[0]))
		return -ENODEV;

	np = of_find_node_by_name(NULL, "softconfig_default");
	if (!np)
		return -ENODEV;
	printk("%s %p\n", __func__, np);
	//ret = softconfig_of_set_group_active_pins_output(
	//					NULL, np, "boot-pins", false);
	of_node_put(np);

	return ret;
}
subsys_initcall_sync(sc59x_softconfig_init);

DT_MACHINE_START(SC59X_DT, "SC59x-EZKIT (Device Tree Support)")
	.map_io		= sc59x_map_io,
	.init_early	= sc59x_init_early,
	.init_time	= sc59x_timer_init,
	.init_machine	= sc59x_init,
	.dt_compat	= sc59x_dt_board_compat,
	.restart        = sc59x_ezkit_restart,
MACHINE_END

#else
MACHINE_START(SC59X, "SC59x-EZKIT")
	.atag_offset	= 0x100,
	.map_io		= sc59x_map_io,
	.init_early	= sc59x_init_early,
	.init_irq	= sc59x_init_irq,
	.init_time	= sc59x_timer_init,
	.init_machine	= sc59x_init,
	.restart        = sc59x_restart,
MACHINE_END
#endif
