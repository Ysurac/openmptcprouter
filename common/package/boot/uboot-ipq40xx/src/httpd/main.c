/*
 * Copyright (c) 2001-2003, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: main.c,v 1.10.2.1 2003/10/04 22:54:17 adam Exp $
 *
 */


#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "httpd.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

/*-----------------------------------------------------------------------------------*/
int
main(void)
{
	u8_t i, arptimer;
	tapdev_init();
	uip_init();
	httpd_init();
	arptimer = 0;
	while(1) {
		uip_len = tapdev_read();
		if(uip_len == 0) {
			for(i = 0; i < UIP_CONNS; i++) {
				uip_periodic(i);
				if(uip_len > 0) {
					uip_arp_out();
					tapdev_send();
				}
			}

			if(++arptimer == 20) {
				uip_arp_timer();
				arptimer = 0;
			}
		} else {
			if(BUF->type == htons(UIP_ETHTYPE_IP)) {
				uip_arp_ipin();
				uip_input();
				if(uip_len > 0) {
					uip_arp_out();
					tapdev_send();
				}
			} else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
				uip_arp_arpin();
				if(uip_len > 0) {
					tapdev_send();
				}
			}
		}
	}
	return 0;
}
