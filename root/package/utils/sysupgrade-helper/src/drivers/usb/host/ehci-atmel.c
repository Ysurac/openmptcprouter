/*
 * (C) Copyright 2012
 * Atmel Semiconductor <www.atmel.com>
 * Written-by: Bo Shen <voice.shen@atmel.com>
 *
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <common.h>
#include <watchdog.h>
#include <usb.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/at91_pmc.h>
#include <asm/arch/clk.h>

#include "ehci.h"
#include "ehci-core.h"

/* Enable UTMI PLL time out 500us
 * 10 times as datasheet specified
 */
#define EN_UPLL_TIMEOUT	500UL

int ehci_hcd_init(void)
{
	at91_pmc_t *pmc = (at91_pmc_t *)ATMEL_BASE_PMC;
	ulong start_time, tmp_time;

	start_time = get_timer(0);
	/* Enable UTMI PLL */
	writel(AT91_PMC_UPLLEN | AT91_PMC_BIASEN, &pmc->uckr);
	while ((readl(&pmc->sr) & AT91_PMC_LOCKU) != AT91_PMC_LOCKU) {
		WATCHDOG_RESET();
		tmp_time = get_timer(0);
		if ((tmp_time - start_time) > EN_UPLL_TIMEOUT) {
			printf("ERROR: failed to enable UPLL\n");
			return -1;
		}
	}

	/* Enable USB Host clock */
	writel(1 << ATMEL_ID_UHPHS, &pmc->pcer);

	hccr = (struct ehci_hccr *)ATMEL_BASE_EHCI;
	hcor = (struct ehci_hcor *)((uint32_t)hccr +
			HC_LENGTH(ehci_readl(&hccr->cr_capbase)));

	return 0;
}

int ehci_hcd_stop(void)
{
	at91_pmc_t *pmc = (at91_pmc_t *)ATMEL_BASE_PMC;
	ulong start_time, tmp_time;

	/* Disable USB Host Clock */
	writel(1 << ATMEL_ID_UHPHS, &pmc->pcdr);

	start_time = get_timer(0);
	/* Disable UTMI PLL */
	writel(readl(&pmc->uckr) & ~AT91_PMC_UPLLEN, &pmc->uckr);
	while ((readl(&pmc->sr) & AT91_PMC_LOCKU) == AT91_PMC_LOCKU) {
		WATCHDOG_RESET();
		tmp_time = get_timer(0);
		if ((tmp_time - start_time) > EN_UPLL_TIMEOUT) {
			printf("ERROR: failed to stop UPLL\n");
			return -1;
		}
	}

	return 0;
}
