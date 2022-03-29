/*
 * mode_string implementation for busybox
 *
 * Copyright (C) 2003  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

/* Aug 13, 2003
 * Fix a bug reported by junkio@cox.net involving the mode_chars index.
 */


#include <common.h>
#include <linux/stat.h>

#if ( S_ISUID != 04000 ) || ( S_ISGID != 02000 ) || ( S_ISVTX != 01000 ) \
 || ( S_IRUSR != 00400 ) || ( S_IWUSR != 00200 ) || ( S_IXUSR != 00100 ) \
 || ( S_IRGRP != 00040 ) || ( S_IWGRP != 00020 ) || ( S_IXGRP != 00010 ) \
 || ( S_IROTH != 00004 ) || ( S_IWOTH != 00002 ) || ( S_IXOTH != 00001 )
#error permission bitflag value assumption(s) violated!
#endif

#if ( S_IFSOCK!= 0140000 ) || ( S_IFLNK != 0120000 ) \
 || ( S_IFREG != 0100000 ) || ( S_IFBLK != 0060000 ) \
 || ( S_IFDIR != 0040000 ) || ( S_IFCHR != 0020000 ) \
 || ( S_IFIFO != 0010000 )
#warning mode type bitflag value assumption(s) violated! falling back to larger version

#if (S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX) == 07777
#undef mode_t
#define mode_t unsigned short
#endif

static const mode_t mode_flags[] = {
	S_IRUSR, S_IWUSR, S_IXUSR, S_ISUID,
	S_IRGRP, S_IWGRP, S_IXGRP, S_ISGID,
	S_IROTH, S_IWOTH, S_IXOTH, S_ISVTX
};

/* The static const char arrays below are duplicated for the two cases
 * because moving them ahead of the mode_flags declaration cause a text
 * size increase with the gcc version I'm using. */

/* The previous version used "0pcCd?bB-?l?s???".  However, the '0', 'C',
 * and 'B' types don't appear to be available on linux.  So I removed them. */
static const char type_chars[16] = "?pc?d?b?-?l?s???";
/*                                  0123456789abcdef */
static const char mode_chars[7] = "rwxSTst";

const char *bb_mode_string(int mode)
{
	static char buf[12];
	char *p = buf;

	int i, j, k;

	*p = type_chars[ (mode >> 12) & 0xf ];
	i = 0;
	do {
		j = k = 0;
		do {
			*++p = '-';
			if (mode & mode_flags[i+j]) {
				*p = mode_chars[j];
				k = j;
			}
		} while (++j < 3);
		if (mode & mode_flags[i+j]) {
			*p = mode_chars[3 + (k & 2) + ((i&8) >> 3)];
		}
		i += 4;
	} while (i < 12);

	/* Note: We don't bother with nul termination because bss initialization
	 * should have taken care of that for us.  If the user scribbled in buf
	 * memory, they deserve whatever happens.  But we'll at least assert. */
	if (buf[10] != 0) return NULL;

	return buf;
}

#else

/* The previous version used "0pcCd?bB-?l?s???".  However, the '0', 'C',
 * and 'B' types don't appear to be available on linux.  So I removed them. */
static const char type_chars[16] = "?pc?d?b?-?l?s???";
/*                                  0123456789abcdef */
static const char mode_chars[7] = "rwxSTst";

const char *bb_mode_string(int mode)
{
	static char buf[12];
	char *p = buf;

	int i, j, k, m;

	*p = type_chars[ (mode >> 12) & 0xf ];
	i = 0;
	m = 0400;
	do {
		j = k = 0;
		do {
			*++p = '-';
			if (mode & m) {
				*p = mode_chars[j];
				k = j;
			}
			m >>= 1;
		} while (++j < 3);
		++i;
		if (mode & (010000 >> i)) {
			*p = mode_chars[3 + (k & 2) + (i == 3)];
		}
	} while (i < 3);

	/* Note: We don't bother with nul termination because bss initialization
	 * should have taken care of that for us.  If the user scribbled in buf
	 * memory, they deserve whatever happens.  But we'll at least assert. */
	if (buf[10] != 0) return NULL;

	return buf;
}

#endif
