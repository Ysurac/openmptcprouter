/*
 * Copyright (c) 2012 The Linux Foundation. All rights reserved.
 */
#ifndef MINUNIT_H
#define MINUNIT_H

struct mu_runner {
	int run;
	int failed;
	int verbose;
};

void mu_init(struct mu_runner *runner, int verbose);
void _mu_run_test(struct mu_runner *runner, char *test_name,
		  char *(*test)(void));

#define progress() putc('.')

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(runner, test) _mu_run_test(runner, #test, test)

#endif
