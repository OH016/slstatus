/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>

#include "arg.h"
#include "slstatus.h"
#include "util.h"

struct arg {
	const char *(*func)(const char *);
	const char *fmt;
	const char *args;
};

char buf[32];
static Display *dpy;

#include "config.h"

static void
difftimespec(struct timespec *res, struct timespec *a, struct timespec *b)
{
	res->tv_sec = a->tv_sec - b->tv_sec - (a->tv_nsec < b->tv_nsec);
	res->tv_nsec = a->tv_nsec - b->tv_nsec +
	               (a->tv_nsec < b->tv_nsec) * 1E9;
}

int
main(void)
{
	struct timespec start, current, diff, intspec, wait;
	size_t i, len;
	int ret;
	char status[MAXLEN];
	const char *res;

	if (!(dpy = XOpenDisplay(NULL)))
		die("XOpenDisplay: Failed to open display");

	while(1) {
		if (clock_gettime(CLOCK_MONOTONIC, &start) < 0)
			die("clock_gettime:");

		status[0] = '\0';
		for (i = len = 0; i < LEN(args); i++) {
			res = args[i].func(args[i].args);
			if ((ret = esnprintf(status + len, sizeof(status) - len,
			                     args[i].fmt, res)) < 0)
				break;

			len += ret;
		}

		if (XStoreName(dpy, DefaultRootWindow(dpy), status) < 0)
			die("XStoreName: Allocation failed");
		XFlush(dpy);

		if (clock_gettime(CLOCK_MONOTONIC, &current) < 0)
			die("clock_gettime:");
		difftimespec(&diff, &current, &start);

		intspec.tv_sec = interval / 1000;
		intspec.tv_nsec = (interval % 1000) * 1E6;
		difftimespec(&wait, &intspec, &diff);

		if (wait.tv_sec >= 0 &&
			nanosleep(&wait, NULL) < 0 &&
			errno != EINTR)
				die("nanosleep:");
	}


	return 0;
}
