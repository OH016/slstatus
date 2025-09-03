/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <unistd.h>

#include "../slstatus.h"
#include "../util.h"

/*
 * https://www.kernel.org/doc/html/latest/power/power_supply_class.html
 */

#define POWER_SUPPLY_CAPACITY "/sys/class/power_supply/%s/capacity"

const char *
battery_perc(const char *bat)
{
	int cap_perc;
	char path[PATH_MAX];

	if (esnprintf(path, sizeof(path), POWER_SUPPLY_CAPACITY, bat) < 0)
		return NULL;
	if (pscanf(path, "%d", &cap_perc) != 1)
		return NULL;

	return bprintf("%d", cap_perc);
}
