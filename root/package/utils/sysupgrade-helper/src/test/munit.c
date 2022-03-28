/*
 * Copyright (c) 2012 The Linux Foundation. All rights reserved.
 */

#include <common.h>
#include "munit.h"

void mu_init(struct mu_runner *runner, int verbose)
{
	runner->failed = 0;
	runner->run = 0;
	runner->verbose = verbose;
}

void _mu_run_test(struct mu_runner *runner, char *test_name,
		  char *(*test)(void))
{
	char *message;

	if (runner->verbose)
		printf("Running %s ... ", test_name);

	message = test();
	runner->run++;

	if (!message) {
		if (runner->verbose)
			printf("done.\n");
		else
			putc('.');
	} else {
		if (runner->verbose) {
			putc('\n');
			puts(message);
			putc('\n');
		} else {
			putc('E');
		}
		runner->failed++;
	}
}
