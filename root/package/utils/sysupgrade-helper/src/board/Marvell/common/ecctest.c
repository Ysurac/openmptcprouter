/*
 * (C) Copyright 2001
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifdef ECC_TEST
static inline void ecc_off (void)
{
	*(volatile int *) (INTERNAL_REG_BASE_ADDR + 0x4b4) &= ~0x00200000;
}

static inline void ecc_on (void)
{
	*(volatile int *) (INTERNAL_REG_BASE_ADDR + 0x4b4) |= 0x00200000;
}

static int putshex (const char *buf, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		printf ("%02x", buf[i]);
	}
	return 0;
}

static int char_memcpy (void *d, const void *s, int len)
{
	int i;
	char *cd = d;
	const char *cs = s;

	for (i = 0; i < len; i++) {
		*(cd++) = *(cs++);
	}
	return 0;
}

static int memory_test (char *buf)
{
	const char src[][16] = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
		{0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
		 0x02, 0x02, 0x02, 0x02, 0x02, 0x02},
		{0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
		 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},
		{0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
		 0x08, 0x08, 0x08, 0x08, 0x08, 0x08},
		{0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
		 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},
		{0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
		 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},
		{0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
		 0x40, 0x40, 0x40, 0x40, 0x40, 0x40},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
		 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
		 0x55, 0x55, 0x55, 0x55, 0x55, 0x55},
		{0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
		{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
	};
	const int foo[] = { 0 };
	int i, j, a;

	printf ("\ntest @ %d %p\n", foo[0], buf);
	for (i = 0; i < 12; i++) {
		for (a = 0; a < 8; a++) {
			const char *s = src[i] + a;
			int align = (unsigned) (s) & 0x7;

			/* ecc_off(); */
			memcpy (buf, s, 8);
			/* ecc_on(); */
			putshex (s, 8);
			if (memcmp (buf, s, 8)) {
				putc ('\n');
				putshex (buf, 8);
				printf (" [FAIL] (%p) align=%d\n", s, align);
				for (j = 0; j < 8; j++) {
					s[j] == buf[j] ? puts ("  ") :
						printf ("%02x",
							(s[j]) ^ (buf[j]));
				}
				putc ('\n');
			} else {
				printf (" [PASS] (%p) align=%d\n", s, align);
			}
			/* ecc_off(); */
			char_memcpy (buf, s, 8);
			/* ecc_on(); */
			putshex (s, 8);
			if (memcmp (buf, s, 8)) {
				putc ('\n');
				putshex (buf, 8);
				printf (" [FAIL] (%p) align=%d\n", s, align);
				for (j = 0; j < 8; j++) {
					s[j] == buf[j] ? puts ("  ") :
						printf ("%02x",
							(s[j]) ^ (buf[j]));
				}
				putc ('\n');
			} else {
				printf (" [PASS] (%p) align=%d\n", s, align);
			}
		}
	}

	return 0;
}
#endif
