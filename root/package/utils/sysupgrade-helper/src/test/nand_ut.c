/*
 * Copyright (c) 2012 The Linux Foundation. All rights reserved.
 */

#include <common.h>
#include <nand.h>
#include <malloc.h>
#include <asm/errno.h>

#include "munit.h"

#define PAGE_OFFS(nand, n)    (((nand)->writesize) * (n))
#define PAGE0_OFFS(nand)      PAGE_OFFS(nand, 0)
#define PAGE1_OFFS(nand)      PAGE_OFFS(nand, 1)
#define PAGE_LAST_OFFS(nand)  ((nand)->size - (nand)->writesize)

#define PAGE_LEN(nand, count)    (((nand)->writesize) * (count))
#define OOB_LEN(nand, count)     (((nand)->oobavail) * (count))
#define OOB_RAW_LEN(nand, count) (((nand)->oobsize) * (count))

#define BLOCK_OFFS(nand, n)   (((nand)->erasesize) * (n))
#define BLOCK0_OFFS(nand)     BLOCK_OFFS(nand, 0)
#define BLOCK1_OFFS(nand)     BLOCK_OFFS(nand, 1)
#define BLOCK_LAST_OFFS(nand) ((nand)->size - (nand)->erasesize)

#define BLOCK_LEN(nand, count) (((nand)->erasesize) * (count))

static uint8_t *testbuf;

static unsigned long rand(void)
{
	static uint32_t lfsr = 1;

	/* taps: 32 31 29 1; feedback polynomial: x^32 + x^31 + x^29 + x + 1 */
	lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xD0000001u);
	return lfsr;
}

static int read_page_raw(nand_info_t *nand, loff_t offs, uint8_t *buf)
{
	struct mtd_oob_ops ops;

	ops.mode = MTD_OOB_RAW;
	ops.len = PAGE_LEN(nand, 1);
	ops.retlen = 0;
	ops.ooblen = OOB_RAW_LEN(nand, 1);
	ops.oobretlen = 0;
	ops.ooboffs = 0;
	ops.datbuf = buf;
	ops.oobbuf = buf + PAGE_LEN(nand, 1);

	return nand->read_oob(nand, offs, &ops);
}

static int write_page_raw(nand_info_t *nand, loff_t offs, uint8_t *buf)
{
	struct mtd_oob_ops ops;

	ops.mode = MTD_OOB_RAW;
	ops.len = PAGE_LEN(nand, 1);
	ops.retlen = 0;
	ops.ooblen = OOB_RAW_LEN(nand, 1);
	ops.oobretlen = 0;
	ops.ooboffs = 0;
	ops.datbuf = buf;
	ops.oobbuf = buf + PAGE_LEN(nand, 1);

	return nand->write_oob(nand, offs, &ops);
}

static int read_pages_oob(nand_info_t *nand, loff_t offs, uint8_t *datbuf,
			  size_t len, uint8_t *oobbuf, size_t ooblen)
{
	struct mtd_oob_ops ops;

	ops.mode = MTD_OOB_AUTO;
	ops.len = len;
	ops.retlen = 0;
	ops.ooblen = ooblen;
	ops.oobretlen = 0;
	ops.ooboffs = 0;
	ops.datbuf = datbuf;
	ops.oobbuf = oobbuf;

	return nand->read_oob(nand, offs, &ops);
}

static int write_pages_oob(nand_info_t *nand, loff_t offs, uint8_t *datbuf,
			   size_t len, uint8_t *oobbuf, size_t ooblen)
{
	struct mtd_oob_ops ops;

	ops.mode = MTD_OOB_AUTO;
	ops.len = len;
	ops.retlen = 0;
	ops.ooblen = ooblen;
	ops.oobretlen = 0;
	ops.ooboffs = 0;
	ops.datbuf = datbuf;
	ops.oobbuf = oobbuf;

	return nand->write_oob(nand, offs, &ops);
}

static int check_page_erased(nand_info_t *nand, loff_t offs)
{
	unsigned int i;
	int ret;

	ret = read_page_raw(nand, offs, testbuf);
	if (ret < 0)
		return ret;

	for (i = 0; i < (nand->writesize + nand->oobsize); i++) {
		if (testbuf[i] != 0xFF)
			return 0;
	}

	return 1;
}

/*
 * Erase block 0, and check if page 0 of the block is set to 0xFFs.
 */
static char *test_block0_erase(void)
{
	int ret;
	nand_info_t *nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK0_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("block0_erase: erase of single block failed", ret == 0);

	mu_assert("block0_erase: block not erased", check_page_erased(nand, 0));

	return NULL;
}

/*
 * Erase last block, and check if page 0 of the block is set to 0xFFs.
 */
static char *test_last_block_erase(void)
{
	int ret;
	nand_info_t *nand = &nand_info[0];
	loff_t offs = BLOCK_LAST_OFFS(nand);

	ret = nand_erase(nand, offs, BLOCK_LEN(nand, 1));
	mu_assert("last_block_erase: erase failed", ret == 0);

	mu_assert("last_block_erase: last block not erased",
		  check_page_erased(nand, offs));

	return NULL;
}

/*
 * Erase 5 blocks, and check if page 0 of the blocks is set to 0xFFs.
 */
static char *test_multi_block_erase(void)
{
	int i;
	int ret;
	int erased;
	int block_count;
	nand_info_t *nand = &nand_info[0];

	block_count = 5;
	ret = nand_erase(nand, BLOCK0_OFFS(nand), BLOCK_LEN(nand, block_count));
	mu_assert("multi_block_erase: erase failed", ret == 0);

	for (i = 0; i < block_count; i++) {
		erased = check_page_erased(nand, BLOCK_OFFS(nand, i));
		mu_assert("multi_block_erase: not erased", erased);
	}

	return NULL;
}

/*
 * Erase 5 blocks, and check if page 0 of the blocks is set to 0xFFs.
 */
static char *test_invalid_block_erase(void)
{
	int ret;
	int block_count = 2;
	nand_info_t *nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK_LAST_OFFS(nand),
			 BLOCK_LEN(nand, block_count));
	mu_assert("invalid_block: erase did not fail", ret == -EINVAL);

	return NULL;
}

/*
 * Erase from unaligned block address, and check if erase fails with
 * EINVAL.
 */
static char *test_unaligned_erase_start(void)
{
	int ret;
	nand_info_t *nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK0_OFFS(nand) + 1, BLOCK_LEN(nand, 1));
	mu_assert("unaligned_erase_start: expected EINVAL", ret == -EINVAL);

	return NULL;
}

/*
 * Erase an unaligned length, and check it erase fails with EINVAL.
 */
static char *test_unaligned_erase_len(void)
{
	int ret;
	nand_info_t *nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK0_OFFS(nand), BLOCK_LEN(nand, 1) + 1);
	mu_assert("unaligned_erase_len: expected EINVAL", ret == -EINVAL);

	return NULL;
}

/*
 * Helper test function to write to flash, read back, and check the
 * read back data. This aggregates common code, from similar tests.
 */
char *test_page_write(loff_t offs, size_t wlen, size_t rlen)
{
	int i;
	int ret;
	nand_info_t *nand;
	uint8_t *writebuf;
	uint8_t *readbuf;

	nand = &nand_info[0];
	writebuf = testbuf;
	readbuf = testbuf + wlen;

	for (i = 0; i < wlen; i++)
		writebuf[i] = rand() & 0xFF;

	ret = nand_write(nand, offs, &wlen, writebuf);
	mu_assert("page_write: write failed", ret == 0);

	ret = nand_read(nand, offs, &rlen, readbuf);
	mu_assert("page_write: read after write failed", ret == 0);

	for (i = 0; i < rlen; i++) {
		if (readbuf[i] != writebuf[i])
			mu_assert("page_write: read back data differs", 0);
	}

	return NULL;
}

/*
 * Write to page 0 and check if readback data matches.
 */
char *test_page0_write(void)
{
	int ret;
	nand_info_t *nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK0_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("page0_write: erase failed", ret == 0);

	return test_page_write(PAGE0_OFFS(nand), PAGE_LEN(nand, 1),
			       PAGE_LEN(nand, 1));
}

/*
 * Write to last page and check if readback data matches.
 */
char *test_last_page_write(void)
{
	int ret;
	loff_t offs;
	nand_info_t *nand = &nand_info[0];

	offs = BLOCK_LAST_OFFS(nand);
	ret = nand_erase(nand, offs, BLOCK_LEN(nand, 1));
	mu_assert("page0_write: erase failed", ret == 0);

	offs = PAGE_LAST_OFFS(nand);
	return test_page_write(offs, PAGE_LEN(nand, 1), PAGE_LEN(nand, 1));
}

/*
 * Write to 3 pages and check if readback data matches.
 */
char *test_multi_page_write(void)
{
	int ret;
	nand_info_t *nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK0_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("multi_page_write: erase failed", ret == 0);

	return test_page_write(PAGE0_OFFS(nand), PAGE_LEN(nand, 3),
			       PAGE_LEN(nand, 3));
}

/*
 * Write 3 pages and read 1.5 pages and check if readback data
 * matches.
 */
char *test_partial_page_read(void)
{
	int ret;
	nand_info_t *nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK0_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("partial_page_read: erase failed", ret == 0);

	return test_page_write(PAGE0_OFFS(nand), PAGE_LEN(nand, 3),
			       PAGE_LEN(nand, 3) / 2);
}

/*
 * Write 1 page out of chip boundary, and check if write fails with
 * EINVAL.
 */
char *test_invalid_page_write(void)
{
	int ret;
	u_long i;
	size_t len;
	loff_t offs;
	nand_info_t *nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK_LAST_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("invalid_page_write: erase failed", ret == 0);

	len = PAGE_LEN(nand, 2);
	offs = PAGE_LAST_OFFS(nand);

	for (i = 0; i < len; i++)
		testbuf[i] = rand() & 0xFF;

	ret = nand_write(nand, offs, &len, testbuf);
	mu_assert("page_write: write did not fail", ret == -EINVAL);

	return NULL;
}

/*
 * Erase a block, even if is marked bad.
 */
static int erase_bad_block(nand_info_t *nand, loff_t offs)
{
	struct erase_info instr;

	instr.mtd = nand;
	instr.addr = offs;
	instr.len = BLOCK_LEN(nand, 1);
	instr.callback = 0;
	instr.scrub = 1;
	return nand->erase(nand, &instr);
}

/*
 * Mark a block bad, and check if it is marked bad.
 */
static char *test_mark_bad(void)
{
	int ret;
	uint32_t badblocks;
	nand_info_t *nand = &nand_info[0];

	badblocks = nand->ecc_stats.badblocks;

	ret = nand->block_markbad(nand, BLOCK0_OFFS(nand));
	mu_assert("mark_bad: markbad failed", ret == 0);

	ret = nand->block_isbad(nand, BLOCK0_OFFS(nand));
	mu_assert("mark_bad: isbad failed", ret != -1);

	mu_assert("mark_bad: not marked block", ret == 1);

	mu_assert("mark_bad: stats not incremented",
		  nand->ecc_stats.badblocks == badblocks + 1);

	ret = erase_bad_block(nand, BLOCK0_OFFS(nand));
	mu_assert("mark_bad: erase failed", ret == 0);

	return NULL;
}

/*
 * Mark a unaligned address bad, and check if it fails with EINVAL.
 */
static char *test_unaligned_mark_bad(void)
{
	int ret;
	nand_info_t *nand = &nand_info[0];

	ret = nand->block_markbad(nand, BLOCK0_OFFS(nand) + 1);
	mu_assert("unaligned_mark_bad: expected EINVAL", ret == -EINVAL);

	return NULL;
}

/*
 * Mark a block out of chip boundary, and check if it fails with
 * EINVAL.
 */
static char *test_invalid_mark_bad(void)
{
	int ret;
	loff_t offs;
	nand_info_t *nand = &nand_info[0];

	offs = BLOCK_LAST_OFFS(nand) + BLOCK_LEN(nand, 2);
	ret = nand->block_markbad(nand, offs);
	mu_assert("invalid_mark_bad: expected EINVAL", ret == -EINVAL);

	return NULL;
}

/*
 * Check an unaligned address is bad, and check if it fails with
 * EINVAL.
 */
static char *test_unaligned_isbad(void)
{
	int ret;
	nand_info_t *nand = &nand_info[0];

	ret = nand->block_isbad(nand, BLOCK0_OFFS(nand) + 1);
	mu_assert("unaligned_isbad: expected EINVAL", ret == -EINVAL);

	return NULL;
}

/*
 * Check a block out of chip boundary is bad, and check if it fails
 * with EINVAL.
 */
static char *test_invalid_isbad(void)
{
	int ret;
	loff_t offs;
	nand_info_t *nand = &nand_info[0];

	offs = BLOCK_LAST_OFFS(nand) + BLOCK_LEN(nand, 2);
	ret = nand->block_isbad(nand, offs);
	mu_assert("invalid_isbad: expected EINVAL", ret == -EINVAL);

	return NULL;
}

/*
 * Erase a bad block, and check if it fails with EIO.
 */
char *test_erase_bad(void)
{
	int ret;
	struct erase_info instr;
	nand_info_t *nand = &nand_info[0];

	ret = nand->block_markbad(nand, BLOCK0_OFFS(nand));
	mu_assert("erase_bad: markbad failed", ret == 0);

	instr.mtd = nand;
	instr.addr = BLOCK0_OFFS(nand);
	instr.len = BLOCK_LEN(nand, 1);
	instr.callback = 0;
	instr.scrub = 0;

	ret = nand->erase(nand, &instr);
	mu_assert("erase_bad: bad block erase did not fail", ret == -EIO);

	ret = erase_bad_block(nand, BLOCK0_OFFS(nand));
	mu_assert("erase_bad: erase failed", ret == 0);

	return NULL;
}

/*
 * Read a page raw, introduce bit errors and write the page raw.
 */
char *test_flip_bits(int bit_flip_count)
{
	u_int i;
	int ret;
	size_t len;
	uint8_t *writebuf;
	uint8_t *readbuf;
	uint8_t error_byte;
	u_int error_bit;
	nand_info_t *nand = &nand_info[0];

	writebuf = testbuf;
	readbuf = writebuf + PAGE_LEN(nand, 1);

	ret = nand_erase(nand, BLOCK0_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("flip_bits: erase failed", ret == 0);

	len = PAGE_LEN(nand, 1);
	for (i = 0; i < len; i++)
		writebuf[i] = rand() % 0xFF;

	ret = nand_write(nand, PAGE0_OFFS(nand), &len, writebuf);
	mu_assert("flip_bits: write failed", ret == 0);

	ret = read_page_raw(nand, PAGE0_OFFS(nand), readbuf);
	mu_assert("flip_bits: read failed", ret == 0);

	for (i = 0; i < bit_flip_count; i++) {
		error_byte = rand() % (nand->writesize + nand->oobsize);
		error_bit = rand() % 8;

		/* Flip it */
		readbuf[error_byte] ^= (1 << error_bit);
	}

	ret = write_page_raw(nand, PAGE1_OFFS(nand), readbuf);
	mu_assert("flip_bits: write back failed", ret == 0);

	return NULL;
}

/*
 * Introduce 4 bit errors, and check if read corrects the bit errors
 * and returns EUCLEAN.
 */
char *test_correction(void)
{
	u_int i;
	int ret;
	char *err;
	loff_t offs;
	size_t len;
	uint8_t *page0;
	uint8_t *page1;
	uint32_t corrected;
	nand_info_t *nand = &nand_info[0];

	page0 = testbuf;
	page1 = testbuf + PAGE_LEN(nand, 1);
	corrected = nand->ecc_stats.corrected;

	err = test_flip_bits(4);
	mu_assert(err, err != NULL);

	offs = PAGE0_OFFS(nand);
	len = PAGE_LEN(nand, 1);
	ret = nand_read(nand, offs, &len, page0);
	mu_assert("correction: read failed", ret == 0);

	offs = PAGE1_OFFS(nand);
	len = PAGE_LEN(nand, 1);
	ret = nand_read(nand, offs, &len, page1);
	mu_assert("correction: expected EUCLEAN", ret == -EUCLEAN);

	for (i = 0; i < len; i++) {
		mu_assert("correction: data not corrected",
			  page0[i] == page1[i]);
	}

	mu_assert("correction: stats.corrected not incremented",
		  nand->ecc_stats.corrected > corrected);

	return NULL;
}

/*
 * Introduce 20 bit errors, and check if read fails with EBADMSG.
 */
char *test_ecc_fail(void)
{
	int ret;
	char *err;
	loff_t offs;
	size_t len;
	uint8_t *page0;
	uint8_t *page1;
	uint32_t failed;
	nand_info_t *nand = &nand_info[0];

	page0 = testbuf;
	page1 = testbuf + PAGE_LEN(nand, 1);
	failed = nand->ecc_stats.failed;

	err = test_flip_bits(20);
	mu_assert(err, err != NULL);

	offs = PAGE0_OFFS(nand);
	len = PAGE_LEN(nand, 1);
	ret = nand_read(nand, offs, &len, page0);
	mu_assert("ecc_fail: read failed", ret == 0);

	offs = PAGE1_OFFS(nand);
	len = PAGE_LEN(nand, 1);
	ret = nand_read(nand, offs, &len, page1);
	mu_assert("ecc_fail: expected EBADMSG", ret == -EBADMSG);

	mu_assert("ecc_fail: stats.fail not incremented",
		  nand->ecc_stats.failed > failed);

	return NULL;
}

/*
 * Helper test function to write inband and out-of-band data, read
 * back out-band data and check the read back data. This aggregates
 * common code, from similar tests.
 */
char *test_page_oob(loff_t offs, size_t len, size_t write_ooblen,
		    size_t read_ooblen)
{
	int i;
	int ret;
	nand_info_t *nand;
	uint8_t *write_datbuf;
	uint8_t *write_oobbuf;
	uint8_t *read_oobbuf;

	nand = &nand_info[0];

	write_datbuf = testbuf;
	write_oobbuf = write_datbuf + len;
	read_oobbuf = write_oobbuf + write_ooblen;

	for (i = 0; i < len; i++)
		write_datbuf[i] = rand() & 0xFF;

	for (i = 0; i < write_ooblen; i++)
		write_oobbuf[i] = rand() & 0xFF;

	ret = write_pages_oob(nand, offs, write_datbuf, len,
			      write_oobbuf, write_ooblen);
	mu_assert("page_oob: write failed", ret == 0);

	ret = read_pages_oob(nand, offs, NULL, 0, read_oobbuf, read_ooblen);
	mu_assert("page_oob: read after write failed", ret == 0);

	for (i = 0; i < read_ooblen; i++) {
		if (read_oobbuf[i] != write_oobbuf[i])
			mu_assert("page_oob: read back data differs", 0);
	}

	return NULL;
}

/*
 * Write inband and out-of-band to page 0, and check if readback
 * out-of-band data matches.
 */
char *test_page0_oob(void)
{
	int ret;
	nand_info_t *nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK0_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("page0_oob: erase failed", ret == 0);

	return test_page_oob(PAGE0_OFFS(nand), PAGE_LEN(nand, 1),
			     OOB_LEN(nand, 1), OOB_LEN(nand, 1));
}

/*
 * Write inband and out-of-band to last page, and check if readback
 * out-of-band data matches.
 */
char *test_last_page_oob(void)
{
	int ret;
	nand_info_t *nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK_LAST_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("last_page_oob: erase failed", ret == 0);

	return test_page_oob(PAGE_LAST_OFFS(nand), PAGE_LEN(nand, 1),
			     OOB_LEN(nand, 1), OOB_LEN(nand, 1));
}

/*
 * Write inband and out-of-band to 3 pages, and check if readback
 * out-of-band data matches.
 */
char *test_multi_page_oob(void)
{
	int ret;
	nand_info_t *nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK0_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("multi_page_oob: erase failed", ret == 0);

	return test_page_oob(PAGE0_OFFS(nand), PAGE_LEN(nand, 3),
			     OOB_LEN(nand, 1), OOB_LEN(nand, 1));
}

/*
 * Write inband and out-of-band to a page, and check if an reading
 * back and unalinged length of out-of-band data matches.
 */
char *test_read_partial_oob(void)
{
	int ret;
	nand_info_t *nand;

	nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK0_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("multi_page_oob: erase failed", ret == 0);

	return test_page_oob(PAGE0_OFFS(nand), PAGE_LEN(nand, 1),
			     OOB_LEN(nand, 1), 5);
}

/*
 * Write inband and unaligned length (5) of out-of-band to a page, and
 * check if a reading back and unalinged length (5) of out-of-band
 * data matches.
 */
char *test_write_partial_oob(void)
{
	int ret;
	nand_info_t *nand;

	nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK0_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("multi_page_oob: erase failed", ret == 0);

	return test_page_oob(PAGE0_OFFS(nand), PAGE_LEN(nand, 1), 5, 5);
}

/*
 * Write inband and unaligned length of out-of-band to 4 pages, and
 * check if a reading back and unalinged length (4 * oobavail / 3) of
 * out-of-band data matches.
 */
char *test_multi_page_partial_oob(void)
{
	int ret;
	size_t ooblen;
	nand_info_t *nand;

	nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK0_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("multi_page_oob: erase failed", ret == 0);

	ooblen = OOB_LEN(nand, 4) / 3;

	return test_page_oob(PAGE0_OFFS(nand), PAGE_LEN(nand, 4),
			     ooblen, ooblen);
}

/*
 * Write of inband and out-of-band to 1 page out of chip boundary,
 * fails with error EINVAL.
 */
char *test_invalid_page_oob(void)
{
	int ret;
	u_long i;
	size_t len;
	size_t ooblen;
	loff_t offs;
	uint8_t *write_datbuf;
	uint8_t *write_oobbuf;
	nand_info_t *nand = &nand_info[0];

	ret = nand_erase(nand, BLOCK_LAST_OFFS(nand), BLOCK_LEN(nand, 1));
	mu_assert("invalid_page_oob: erase failed", ret == 0);

	len = PAGE_LEN(nand, 2);
	ooblen = OOB_LEN(nand, 2);
	offs = PAGE_LAST_OFFS(nand);

	write_datbuf = testbuf;
	write_oobbuf = write_datbuf + len;

	for (i = 0; i < len; i++)
		write_datbuf[i] = rand() & 0xFF;

	for (i = 0; i < ooblen; i++)
		write_oobbuf[i] = rand() & 0xFF;

	ret = write_pages_oob(nand, offs, write_datbuf, len,
			      write_oobbuf, ooblen);
	mu_assert("invalid_page_oob: write did not fail",
		  ret == -EINVAL);

	return NULL;
}

static void all_tests(struct mu_runner *runner)
{
	/* Erase tests */
	mu_run_test(runner, test_block0_erase);
	mu_run_test(runner, test_last_block_erase);
	mu_run_test(runner, test_multi_block_erase);
	mu_run_test(runner, test_invalid_block_erase);
	mu_run_test(runner, test_unaligned_erase_start);
	mu_run_test(runner, test_unaligned_erase_len);

	/* Read/Write tests */
	mu_run_test(runner, test_page0_write);
	mu_run_test(runner, test_last_page_write);
	mu_run_test(runner, test_multi_page_write);
	mu_run_test(runner, test_invalid_page_write);
	mu_run_test(runner, test_partial_page_read);

	/* Bad block tests */
	mu_run_test(runner, test_mark_bad);
	mu_run_test(runner, test_erase_bad);
	mu_run_test(runner, test_unaligned_mark_bad);
	mu_run_test(runner, test_unaligned_isbad);
	mu_run_test(runner, test_invalid_mark_bad);
	mu_run_test(runner, test_invalid_isbad);

	/* ECC tests */
	mu_run_test(runner, test_correction);
	mu_run_test(runner, test_ecc_fail);

	/* OOB tests */
	mu_run_test(runner, test_page0_oob);
	mu_run_test(runner, test_last_page_oob);
	mu_run_test(runner, test_multi_page_oob);
	mu_run_test(runner, test_invalid_page_oob);
	mu_run_test(runner, test_read_partial_oob);
	mu_run_test(runner, test_write_partial_oob);
	mu_run_test(runner, test_multi_page_partial_oob);

	return;
}

int nand_ut(int verbose)
{
	struct mu_runner runner;
	nand_info_t *nand = &nand_info[0];

	testbuf = malloc((nand->writesize + nand->oobsize) * 16);
	if (testbuf == NULL) {
		printf("nand_ut: test buffer allocation failed\n");
		return CMD_RET_FAILURE;
	}

	mu_init(&runner, verbose);
	all_tests(&runner);

	putc('\n');
	printf("Tests run: %d  failed: %d\n",
	       runner.run, runner.failed);

	free(testbuf);

	if (runner.failed)
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}

static int do_nand_ut_cmd(cmd_tbl_t *cmdtp, int flag,
			  int argc, char * const argv[])
{
	int verbose = 0;

	if (argc == 2 && strcmp(argv[1], "verbose") == 0)
		verbose = 1;

	return nand_ut(verbose);
}

U_BOOT_CMD(nand_ut, 2, 1, do_nand_ut_cmd,
	   "Unit tests for NAND driver",
	   "nand_ut [verbose]");
