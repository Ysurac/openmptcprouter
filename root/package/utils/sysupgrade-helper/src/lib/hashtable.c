/*
 * This implementation is based on code from uClibc-0.9.30.3 but was
 * modified and extended for use within U-Boot.
 *
 * Copyright (C) 2010 Wolfgang Denk <wd@denx.de>
 *
 * Original license header:
 *
 * Copyright (C) 1993, 1995, 1996, 1997, 2002 Free Software Foundation, Inc.
 * This file is part of the GNU C Library.
 * Contributed by Ulrich Drepper <drepper@gnu.ai.mit.edu>, 1993.
 *
 * The GNU C Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The GNU C Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

#include <errno.h>
#include <malloc.h>

#ifdef USE_HOSTCC		/* HOST build */
# include <string.h>
# include <assert.h>
# include <ctype.h>

# ifndef debug
#  ifdef DEBUG
#   define debug(fmt,args...)	printf(fmt ,##args)
#  else
#   define debug(fmt,args...)
#  endif
# endif
#else				/* U-Boot build */
# include <common.h>
# include <linux/string.h>
# include <linux/ctype.h>
#endif

#ifndef	CONFIG_ENV_MIN_ENTRIES	/* minimum number of entries */
#define	CONFIG_ENV_MIN_ENTRIES 64
#endif
#ifndef	CONFIG_ENV_MAX_ENTRIES	/* maximum number of entries */
#define	CONFIG_ENV_MAX_ENTRIES 512
#endif

#include "search.h"

/*
 * [Aho,Sethi,Ullman] Compilers: Principles, Techniques and Tools, 1986
 * [Knuth]	      The Art of Computer Programming, part 3 (6.4)
 */

/*
 * The reentrant version has no static variables to maintain the state.
 * Instead the interface of all functions is extended to take an argument
 * which describes the current status.
 */
typedef struct _ENTRY {
	int used;
	ENTRY entry;
} _ENTRY;


/*
 * hcreate()
 */

/*
 * For the used double hash method the table size has to be a prime. To
 * correct the user given table size we need a prime test.  This trivial
 * algorithm is adequate because
 * a)  the code is (most probably) called a few times per program run and
 * b)  the number is small because the table must fit in the core
 * */
static int isprime(unsigned int number)
{
	/* no even number will be passed */
	unsigned int div = 3;

	while (div * div < number && number % div != 0)
		div += 2;

	return number % div != 0;
}

/*
 * Before using the hash table we must allocate memory for it.
 * Test for an existing table are done. We allocate one element
 * more as the found prime number says. This is done for more effective
 * indexing as explained in the comment for the hsearch function.
 * The contents of the table is zeroed, especially the field used
 * becomes zero.
 */

int hcreate_r(size_t nel, struct hsearch_data *htab)
{
	/* Test for correct arguments.  */
	if (htab == NULL) {
		__set_errno(EINVAL);
		return 0;
	}

	/* There is still another table active. Return with error. */
	if (htab->table != NULL)
		return 0;

	/* Change nel to the first prime number not smaller as nel. */
	nel |= 1;		/* make odd */
	while (!isprime(nel))
		nel += 2;

	htab->size = nel;
	htab->filled = 0;

	/* allocate memory and zero out */
	htab->table = (_ENTRY *) calloc(htab->size + 1, sizeof(_ENTRY));
	if (htab->table == NULL)
		return 0;

	/* everything went alright */
	return 1;
}


/*
 * hdestroy()
 */

/*
 * After using the hash table it has to be destroyed. The used memory can
 * be freed and the local static variable can be marked as not used.
 */

void hdestroy_r(struct hsearch_data *htab)
{
	int i;

	/* Test for correct arguments.  */
	if (htab == NULL) {
		__set_errno(EINVAL);
		return;
	}

	/* free used memory */
	for (i = 1; i <= htab->size; ++i) {
		if (htab->table[i].used > 0) {
			ENTRY *ep = &htab->table[i].entry;

			free((void *)ep->key);
			free(ep->data);
		}
	}
	free(htab->table);

	/* the sign for an existing table is an value != NULL in htable */
	htab->table = NULL;
}

/*
 * hsearch()
 */

/*
 * This is the search function. It uses double hashing with open addressing.
 * The argument item.key has to be a pointer to an zero terminated, most
 * probably strings of chars. The function for generating a number of the
 * strings is simple but fast. It can be replaced by a more complex function
 * like ajw (see [Aho,Sethi,Ullman]) if the needs are shown.
 *
 * We use an trick to speed up the lookup. The table is created by hcreate
 * with one more element available. This enables us to use the index zero
 * special. This index will never be used because we store the first hash
 * index in the field used where zero means not used. Every other value
 * means used. The used field can be used as a first fast comparison for
 * equality of the stored and the parameter value. This helps to prevent
 * unnecessary expensive calls of strcmp.
 *
 * This implementation differs from the standard library version of
 * this function in a number of ways:
 *
 * - While the standard version does not make any assumptions about
 *   the type of the stored data objects at all, this implementation
 *   works with NUL terminated strings only.
 * - Instead of storing just pointers to the original objects, we
 *   create local copies so the caller does not need to care about the
 *   data any more.
 * - The standard implementation does not provide a way to update an
 *   existing entry.  This version will create a new entry or update an
 *   existing one when both "action == ENTER" and "item.data != NULL".
 * - Instead of returning 1 on success, we return the index into the
 *   internal hash table, which is also guaranteed to be positive.
 *   This allows us direct access to the found hash table slot for
 *   example for functions like hdelete().
 */

/*
 * hstrstr_r - return index to entry whose key and/or data contains match
 */
int hstrstr_r(const char *match, int last_idx, ENTRY ** retval,
	      struct hsearch_data *htab)
{
	unsigned int idx;

	for (idx = last_idx + 1; idx < htab->size; ++idx) {
		if (htab->table[idx].used <= 0)
			continue;
		if (strstr(htab->table[idx].entry.key, match) ||
		    strstr(htab->table[idx].entry.data, match)) {
			*retval = &htab->table[idx].entry;
			return idx;
		}
	}

	__set_errno(ESRCH);
	*retval = NULL;
	return 0;
}

int hmatch_r(const char *match, int last_idx, ENTRY ** retval,
	     struct hsearch_data *htab)
{
	unsigned int idx;
	size_t key_len = strlen(match);

	for (idx = last_idx + 1; idx < htab->size; ++idx) {
		if (htab->table[idx].used <= 0)
			continue;
		if (!strncmp(match, htab->table[idx].entry.key, key_len)) {
			*retval = &htab->table[idx].entry;
			return idx;
		}
	}

	__set_errno(ESRCH);
	*retval = NULL;
	return 0;
}

int hsearch_r(ENTRY item, ACTION action, ENTRY ** retval,
	      struct hsearch_data *htab)
{
	unsigned int hval;
	unsigned int count;
	unsigned int len = strlen(item.key);
	unsigned int idx;
	unsigned int first_deleted = 0;

	/* Compute an value for the given string. Perhaps use a better method. */
	hval = len;
	count = len;
	while (count-- > 0) {
		hval <<= 4;
		hval += item.key[count];
	}

	/*
	 * First hash function:
	 * simply take the modul but prevent zero.
	 */
	hval %= htab->size;
	if (hval == 0)
		++hval;

	/* The first index tried. */
	idx = hval;

	if (htab->table[idx].used) {
		/*
		 * Further action might be required according to the
		 * action value.
		 */
		unsigned hval2;

		if (htab->table[idx].used == -1
		    && !first_deleted)
			first_deleted = idx;

		if (htab->table[idx].used == hval
		    && strcmp(item.key, htab->table[idx].entry.key) == 0) {
			/* Overwrite existing value? */
			if ((action == ENTER) && (item.data != NULL)) {
				free(htab->table[idx].entry.data);
				htab->table[idx].entry.data =
					strdup(item.data);
				if (!htab->table[idx].entry.data) {
					__set_errno(ENOMEM);
					*retval = NULL;
					return 0;
				}
			}
			/* return found entry */
			*retval = &htab->table[idx].entry;
			return idx;
		}

		/*
		 * Second hash function:
		 * as suggested in [Knuth]
		 */
		hval2 = 1 + hval % (htab->size - 2);

		do {
			/*
			 * Because SIZE is prime this guarantees to
			 * step through all available indices.
			 */
			if (idx <= hval2)
				idx = htab->size + idx - hval2;
			else
				idx -= hval2;

			/*
			 * If we visited all entries leave the loop
			 * unsuccessfully.
			 */
			if (idx == hval)
				break;

			/* If entry is found use it. */
			if ((htab->table[idx].used == hval)
			    && strcmp(item.key, htab->table[idx].entry.key) == 0) {
				/* Overwrite existing value? */
				if ((action == ENTER) && (item.data != NULL)) {
					free(htab->table[idx].entry.data);
					htab->table[idx].entry.data =
						strdup(item.data);
					if (!htab->table[idx].entry.data) {
						__set_errno(ENOMEM);
						*retval = NULL;
						return 0;
					}
				}
				/* return found entry */
				*retval = &htab->table[idx].entry;
				return idx;
			}
		}
		while (htab->table[idx].used);
	}

	/* An empty bucket has been found. */
	if (action == ENTER) {
		/*
		 * If table is full and another entry should be
		 * entered return with error.
		 */
		if (htab->filled == htab->size) {
			__set_errno(ENOMEM);
			*retval = NULL;
			return 0;
		}

		/*
		 * Create new entry;
		 * create copies of item.key and item.data
		 */
		if (first_deleted)
			idx = first_deleted;

		htab->table[idx].used = hval;
		htab->table[idx].entry.key = strdup(item.key);
		htab->table[idx].entry.data = strdup(item.data);
		if (!htab->table[idx].entry.key ||
		    !htab->table[idx].entry.data) {
			__set_errno(ENOMEM);
			*retval = NULL;
			return 0;
		}

		++htab->filled;

		/* return new entry */
		*retval = &htab->table[idx].entry;
		return 1;
	}

	__set_errno(ESRCH);
	*retval = NULL;
	return 0;
}


/*
 * hdelete()
 */

/*
 * The standard implementation of hsearch(3) does not provide any way
 * to delete any entries from the hash table.  We extend the code to
 * do that.
 */

int hdelete_r(const char *key, struct hsearch_data *htab)
{
	ENTRY e, *ep;
	int idx;

	debug("hdelete: DELETE key \"%s\"\n", key);

	e.key = (char *)key;

	if ((idx = hsearch_r(e, FIND, &ep, htab)) == 0) {
		__set_errno(ESRCH);
		return 0;	/* not found */
	}

	/* free used ENTRY */
	debug("hdelete: DELETING key \"%s\"\n", key);

	free((void *)ep->key);
	free(ep->data);
	htab->table[idx].used = -1;

	--htab->filled;

	return 1;
}

/*
 * hexport()
 */

/*
 * Export the data stored in the hash table in linearized form.
 *
 * Entries are exported as "name=value" strings, separated by an
 * arbitrary (non-NUL, of course) separator character. This allows to
 * use this function both when formatting the U-Boot environment for
 * external storage (using '\0' as separator), but also when using it
 * for the "printenv" command to print all variables, simply by using
 * as '\n" as separator. This can also be used for new features like
 * exporting the environment data as text file, including the option
 * for later re-import.
 *
 * The entries in the result list will be sorted by ascending key
 * values.
 *
 * If the separator character is different from NUL, then any
 * separator characters and backslash characters in the values will
 * be escaped by a preceeding backslash in output. This is needed for
 * example to enable multi-line values, especially when the output
 * shall later be parsed (for example, for re-import).
 *
 * There are several options how the result buffer is handled:
 *
 * *resp  size
 * -----------
 *  NULL    0	A string of sufficient length will be allocated.
 *  NULL   >0	A string of the size given will be
 *		allocated. An error will be returned if the size is
 *		not sufficient.  Any unused bytes in the string will
 *		be '\0'-padded.
 * !NULL    0	The user-supplied buffer will be used. No length
 *		checking will be performed, i. e. it is assumed that
 *		the buffer size will always be big enough. DANGEROUS.
 * !NULL   >0	The user-supplied buffer will be used. An error will
 *		be returned if the size is not sufficient.  Any unused
 *		bytes in the string will be '\0'-padded.
 */

static int cmpkey(const void *p1, const void *p2)
{
	ENTRY *e1 = *(ENTRY **) p1;
	ENTRY *e2 = *(ENTRY **) p2;

	return (strcmp(e1->key, e2->key));
}

ssize_t hexport_r(struct hsearch_data *htab, const char sep,
		 char **resp, size_t size,
		 int argc, char * const argv[])
{
	ENTRY *list[htab->size];
	char *res, *p;
	size_t totlen;
	int i, n;

	/* Test for correct arguments.  */
	if ((resp == NULL) || (htab == NULL)) {
		__set_errno(EINVAL);
		return (-1);
	}

	debug("EXPORT  table = %p, htab.size = %d, htab.filled = %d, "
		"size = %zu\n", htab, htab->size, htab->filled, size);
	/*
	 * Pass 1:
	 * search used entries,
	 * save addresses and compute total length
	 */
	for (i = 1, n = 0, totlen = 0; i <= htab->size; ++i) {

		if (htab->table[i].used > 0) {
			ENTRY *ep = &htab->table[i].entry;
			int arg, found = 0;

			for (arg = 0; arg < argc; ++arg) {
				if (strcmp(argv[arg], ep->key) == 0) {
					found = 1;
					break;
				}
			}
			if ((argc > 0) && (found == 0))
				continue;

			list[n++] = ep;

			totlen += strlen(ep->key) + 2;

			if (sep == '\0') {
				totlen += strlen(ep->data);
			} else {	/* check if escapes are needed */
				char *s = ep->data;

				while (*s) {
					++totlen;
					/* add room for needed escape chars */
					if ((*s == sep) || (*s == '\\'))
						++totlen;
					++s;
				}
			}
			totlen += 2;	/* for '=' and 'sep' char */
		}
	}

#ifdef DEBUG
	/* Pass 1a: print unsorted list */
	printf("Unsorted: n=%d\n", n);
	for (i = 0; i < n; ++i) {
		printf("\t%3d: %p ==> %-10s => %s\n",
		       i, list[i], list[i]->key, list[i]->data);
	}
#endif

	/* Sort list by keys */
	qsort(list, n, sizeof(ENTRY *), cmpkey);

	/* Check if the user supplied buffer size is sufficient */
	if (size) {
		if (size < totlen + 1) {	/* provided buffer too small */
			printf("Env export buffer too small: %zu, "
				"but need %zu\n", size, totlen + 1);
			__set_errno(ENOMEM);
			return (-1);
		}
	} else {
		size = totlen + 1;
	}

	/* Check if the user provided a buffer */
	if (*resp) {
		/* yes; clear it */
		res = *resp;
		memset(res, '\0', size);
	} else {
		/* no, allocate and clear one */
		*resp = res = calloc(1, size);
		if (res == NULL) {
			__set_errno(ENOMEM);
			return (-1);
		}
	}
	/*
	 * Pass 2:
	 * export sorted list of result data
	 */
	for (i = 0, p = res; i < n; ++i) {
		const char *s;

		s = list[i]->key;
		while (*s)
			*p++ = *s++;
		*p++ = '=';

		s = list[i]->data;

		while (*s) {
			if ((*s == sep) || (*s == '\\'))
				*p++ = '\\';	/* escape */
			*p++ = *s++;
		}
		*p++ = sep;
	}
	*p = '\0';		/* terminate result */

	return size;
}


/*
 * himport()
 */

/*
 * Import linearized data into hash table.
 *
 * This is the inverse function to hexport(): it takes a linear list
 * of "name=value" pairs and creates hash table entries from it.
 *
 * Entries without "value", i. e. consisting of only "name" or
 * "name=", will cause this entry to be deleted from the hash table.
 *
 * The "flag" argument can be used to control the behaviour: when the
 * H_NOCLEAR bit is set, then an existing hash table will kept, i. e.
 * new data will be added to an existing hash table; otherwise, old
 * data will be discarded and a new hash table will be created.
 *
 * The separator character for the "name=value" pairs can be selected,
 * so we both support importing from externally stored environment
 * data (separated by NUL characters) and from plain text files
 * (entries separated by newline characters).
 *
 * To allow for nicely formatted text input, leading white space
 * (sequences of SPACE and TAB chars) is ignored, and entries starting
 * (after removal of any leading white space) with a '#' character are
 * considered comments and ignored.
 *
 * [NOTE: this means that a variable name cannot start with a '#'
 * character.]
 *
 * When using a non-NUL separator character, backslash is used as
 * escape character in the value part, allowing for example for
 * multi-line values.
 *
 * In theory, arbitrary separator characters can be used, but only
 * '\0' and '\n' have really been tested.
 */

int himport_r(struct hsearch_data *htab,
	      const char *env, size_t size, const char sep, int flag)
{
	char *data, *sp, *dp, *name, *value;

	/* Test for correct arguments.  */
	if (htab == NULL) {
		__set_errno(EINVAL);
		return 0;
	}

	/* we allocate new space to make sure we can write to the array */
	if ((data = malloc(size)) == NULL) {
		debug("himport_r: can't malloc %zu bytes\n", size);
		__set_errno(ENOMEM);
		return 0;
	}
	memcpy(data, env, size);
	dp = data;

	if ((flag & H_NOCLEAR) == 0) {
		/* Destroy old hash table if one exists */
		debug("Destroy Hash Table: %p table = %p\n", htab,
		       htab->table);
		if (htab->table)
			hdestroy_r(htab);
	}

	/*
	 * Create new hash table (if needed).  The computation of the hash
	 * table size is based on heuristics: in a sample of some 70+
	 * existing systems we found an average size of 39+ bytes per entry
	 * in the environment (for the whole key=value pair). Assuming a
	 * size of 8 per entry (= safety factor of ~5) should provide enough
	 * safety margin for any existing environment definitions and still
	 * allow for more than enough dynamic additions. Note that the
	 * "size" argument is supposed to give the maximum enviroment size
	 * (CONFIG_ENV_SIZE).  This heuristics will result in
	 * unreasonably large numbers (and thus memory footprint) for
	 * big flash environments (>8,000 entries for 64 KB
	 * envrionment size), so we clip it to a reasonable value.
	 * On the other hand we need to add some more entries for free
	 * space when importing very small buffers. Both boundaries can
	 * be overwritten in the board config file if needed.
	 */

	if (!htab->table) {
		int nent = CONFIG_ENV_MIN_ENTRIES + size / 8;

		if (nent > CONFIG_ENV_MAX_ENTRIES)
			nent = CONFIG_ENV_MAX_ENTRIES;

		debug("Create Hash Table: N=%d\n", nent);

		if (hcreate_r(nent, htab) == 0) {
			free(data);
			return 0;
		}
	}

	/* Parse environment; allow for '\0' and 'sep' as separators */
	do {
		ENTRY e, *rv;

		/* skip leading white space */
		while (isblank(*dp))
			++dp;

		/* skip comment lines */
		if (*dp == '#') {
			while (*dp && (*dp != sep))
				++dp;
			++dp;
			continue;
		}

		/* parse name */
		for (name = dp; *dp != '=' && *dp && *dp != sep; ++dp)
			;

		/* deal with "name" and "name=" entries (delete var) */
		if (*dp == '\0' || *(dp + 1) == '\0' ||
		    *dp == sep || *(dp + 1) == sep) {
			if (*dp == '=')
				*dp++ = '\0';
			*dp++ = '\0';	/* terminate name */

			debug("DELETE CANDIDATE: \"%s\"\n", name);

			if (hdelete_r(name, htab) == 0)
				debug("DELETE ERROR ##############################\n");

			continue;
		}
		*dp++ = '\0';	/* terminate name */

		/* parse value; deal with escapes */
		for (value = sp = dp; *dp && (*dp != sep); ++dp) {
			if ((*dp == '\\') && *(dp + 1))
				++dp;
			*sp++ = *dp;
		}
		*sp++ = '\0';	/* terminate value */
		++dp;

		/* enter into hash table */
		e.key = name;
		e.data = value;

		hsearch_r(e, ENTER, &rv, htab);
		if (rv == NULL) {
			printf("himport_r: can't insert \"%s=%s\" into hash table\n",
				name, value);
			return 0;
		}

		debug("INSERT: table %p, filled %d/%d rv %p ==> name=\"%s\" value=\"%s\"\n",
			htab, htab->filled, htab->size,
			rv, name, value);
	} while ((dp < data + size) && *dp);	/* size check needed for text */
						/* without '\0' termination */
	debug("INSERT: free(data = %p)\n", data);
	free(data);

	debug("INSERT: done\n");
	return 1;		/* everything OK */
}
