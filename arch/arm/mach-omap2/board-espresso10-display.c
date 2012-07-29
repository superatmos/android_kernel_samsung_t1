/* arch/arm/mach-omap2/board-espresso10-display.c
 *
 * Copyright (C) 2011 Samsung Electronics Co, Ltd.
 *
 * Based on mach-omap2/board-tuna-display.c
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/omapfb.h>
#include <linux/regulator/consumer.h>
#include <linux/platform_data/panel-ltn101al03.h>

#include <plat/vram.h>
#include <plat/omap_hwmod.h>

#include <video/omapdss.h>
#include <video/omap-panel-generic-dpi.h>

#include "board-espresso10.h"
#include "control.h"
#include "mux.h"
#include "omap_muxtbl.h"

#define ESPRESSO10_FB_RAM_SIZE		SZ_16M	/* ~1280*720*4 * 2 */

static struct ltn101al03_panel_data espresso10_panel_data;

static void espresso10_lcd_set_power(bool enable)
{
	pr_debug("(%s): espresso10_lcd_set_power, enable=%d\n",
		 __func__, enable);

	gpio_set_value(espresso10_panel_data.lcd_en_gpio, enable);
}

static void espresso10_lcd_set_gptimer_idle(void)
{
	struct omap_hwmod *timer10_hwmod;
	pr_debug("espresso10_lcd_set_gptimer_idle\n");

	timer10_hwmod = omap_hwmod_lookup("timer10");
	if (likely(timer10_hwmod))
		omap_hwmod_idle(timer10_hwmod);
}

static struct omap_dss_device espresso10_lcd_device = {
	.name			= "lcd",
	.driver_name		= "ltn101al03_panel",
	.type			= OMAP_DISPLAY_TYPE_DPI,
	.phy.dpi.data_lines	= 24,
	.data			= &espresso10_panel_data,
	.channel		= OMAP_DSS_CHANNEL_LCD2,
#ifdef CONFIG_FB_OMAP_BOOTLOADER_INIT
	.skip_init		= true,
#else
	.skip_init		= false,
#endif
	.panel = {
		.width_in_um	= 216960,
		.height_in_um	= 135600,
	},
};

static void __init espresso10_hdmi_mux_init(void)
{
	u32 r;
	/* strong pullup on DDC lines using unpublished register */
	r = OMAP4_HDMI_DDC_SCL_PULLUPRESX_MASK
			| OMAP4_HDMI_DDC_SDA_PULLUPRESX_MASK;
	omap4_ctrl_pad_writel(r, OMAP4_CTRL_MODULE_PAD_CORE_CONTROL_I2C_1);
}

static struct omap_dss_device espresso10_hdmi_device = {
	.name = "hdmi",
	.driver_name = "hdmi_panel",
	.type = OMAP_DISPLAY_TYPE_HDMI,
	.clocks = {
		.dispc  = {
			.dispc_fclk_src = OMAP_DSS_CLK_SRC_FCK,
		},
		.hdmi = {
			.regn = 15,
			.regm2 = 1,
			.max_pixclk_khz = 75000,
		},
	},
	.channel = OMAP_DSS_CHANNEL_DIGIT,
};

static struct omap_dss_device *espresso10_dss_devices[] = {
	&espresso10_lcd_device,
	&espresso10_hdmi_device,
};

static struct omap_dss_board_info espresso10_dss_data = {
	.devices	= espresso10_dss_devices,
	.default_device	= &espresso10_lcd_device,
};

static struct omapfb_platform_data espresso10_fb_pdata = {
	.mem_desc = {
		.region_cnt = 1,
		.region = {
			[0] = {
				.size = ESPRESSO10_FB_RAM_SIZE,
			},
		},
	},
};

void __init omap4_espresso10_display_early_init(void)
{
	struct omap_hwmod *timer10_hwmod;
	struct omap_hwmod *gpio3_hwmod;
	struct omap_hwmod *gpio5_hwmod;

	/* correct timer10 hwmod flag settings for espresso board. */
	timer10_hwmod = omap_hwmod_lookup("timer10");
	if (likely(timer10_hwmod))
		timer10_hwmod->flags =
			(HWMOD_INIT_NO_IDLE | HWMOD_INIT_NO_RESET);

	/* correct gpio3 hwmod flag settings for espresso board. */
	gpio3_hwmod = omap_hwmod_lookup("gpio3");
	if (likely(gpio3_hwmod))
		gpio3_hwmod->flags = HWMOD_INIT_NO_RESET;

	/* correct gpio5 hwmod flag settings for espresso board. */
	gpio5_hwmod = omap_hwmod_lookup("gpio5");
	if (likely(gpio5_hwmod))
		gpio5_hwmod->flags = HWMOD_INIT_NO_RESET;
}

void __init omap4_espresso10_display_init(void)
{
	struct ltn101al03_panel_data *panel;
	int ret;
	u8 board_type;

	espresso10_panel_data.lvds_nshdn_gpio =
	    omap_muxtbl_get_gpio_by_name("LVDS_nSHDN");
	espresso10_panel_data.lcd_en_gpio =
	    omap_muxtbl_get_gpio_by_name("LCD_EN");
	espresso10_panel_data.led_backlight_reset_gpio =
	    omap_muxtbl_get_gpio_by_name("LED_BACKLIGHT_RESET");
	espresso10_panel_data.backlight_gptimer_num = 10;
	espresso10_panel_data.set_power = espresso10_lcd_set_power;
	espresso10_panel_data.set_gptimer_idle =
		espresso10_lcd_set_gptimer_idle;

	ret = gpio_request(espresso10_panel_data.lcd_en_gpio, "lcd_en");
	if (ret < 0)
		pr_err("(%s): gpio_request %d failed!\n", __func__,
		       espresso10_panel_data.lcd_en_gpio);

	gpio_direction_output(espresso10_panel_data.lcd_en_gpio, 1);

	panel = &espresso10_panel_data;

	espresso10_lcd_device.data = panel;

	board_type = omap4_espresso10_get_board_type();
	if (board_type == SEC_MACHINE_ESPRESSO10_USA_BBY) {
		/* Two DSS devices: LCD & HDMI */
		espresso10_dss_data.num_devices = 2;
		espresso10_hdmi_device.hpd_gpio =
			omap_muxtbl_get_gpio_by_name("HDMI_HPD");
		espresso10_hdmi_mux_init();
	} else
		/* LCD only */
		espresso10_dss_data.num_devices = 1;

	omap_vram_set_sdram_vram(ESPRESSO10_FB_RAM_SIZE, 0);
	omapfb_set_platform_data(&espresso10_fb_pdata);
	omap_display_init(&espresso10_dss_data);
}

