/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdint.h>

#include "../slstatus.h"
#include "../util.h"

const char *
ram_perc(const char *unused)
{
	uintmax_t total, free, buffers, cached, shmem, sreclaimable;
	int percent;
	FILE *fp;

	if (!(fp = fopen("/proc/meminfo", "r")))
		return NULL;

	if (lscanf(fp, "MemTotal:", "%ju kB", &total)  != 1 ||
		lscanf(fp, "MemFree:", "%ju kB", &free)    != 1 ||
		lscanf(fp, "Buffers:", "%ju kB", &buffers) != 1 ||
		lscanf(fp, "Cached:", "%ju kB", &cached)   != 1 ||
		lscanf(fp, "Shmem:", "%ju kB", &shmem)     != 1 ||
		lscanf(fp, "SReclaimable:", "%ju kB", &sreclaimable) != 1) {
		fclose(fp);
		return NULL;
	}
	fclose(fp);

	if (total == 0)
		return NULL;

	percent = 100 * (total - free - buffers - cached - sreclaimable + shmem) / total;
	return bprintf("%d", percent);
}
