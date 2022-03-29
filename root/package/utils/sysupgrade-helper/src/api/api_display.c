/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <api_public.h>
#include <lcd.h>
#include <video_font.h> /* Get font width and height */

/* lcd.h needs BMP_LOGO_HEIGHT to calculate CONSOLE_ROWS */
#if defined(CONFIG_LCD_LOGO) && !defined(CONFIG_LCD_INFO_BELOW_LOGO)
#include <bmp_logo.h>
#endif

/* TODO(clchiou): add support of video device */

int display_get_info(int type, struct display_info *di)
{
	if (!di)
		return API_EINVAL;

	switch (type) {
	default:
		debug("%s: unsupport display device type: %d\n",
				__FILE__, type);
		return API_ENODEV;
#ifdef CONFIG_LCD
	case DISPLAY_TYPE_LCD:
		di->pixel_width  = panel_info.vl_col;
		di->pixel_height = panel_info.vl_row;
		di->screen_rows = CONSOLE_ROWS;
		di->screen_cols = CONSOLE_COLS;
		break;
#endif
	}

	di->type = type;
	return 0;
}

int display_draw_bitmap(ulong bitmap, int x, int y)
{
	if (!bitmap)
		return API_EINVAL;
#ifdef CONFIG_LCD
	return lcd_display_bitmap(bitmap, x, y);
#else
	return API_ENODEV;
#endif
}

void display_clear(void)
{
#ifdef CONFIG_LCD
	lcd_clear();
#endif
}
