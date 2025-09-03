/* See LICENSE file for copyright and license details. */

/* interval between updates (in ms) */
const unsigned int interval = 1000;

/* text to show if no value can be retrieved */
static const char unknown_str[] = "";

/* maximum output string length */
#define MAXLEN 2048

/*
 * function            description                     argument (example)
 * battery_perc        battery percentage              battery name (BAT0)
 * cpu_perc            cpu usage in percent            NULL
 * datetime            date and time                   format string (%F %T)
 * netspeed_rx         receive network speed           interface name (wlan0)
 * netspeed_tx         transfer network speed          interface name (wlan0)
 * ram_perc            memory usage in percent         NULL
 * temp                temperature in degree celsius   sensor file
 * vol_perc            OSS/ALSA volume in percent      mixer file (/dev/mixer)
 * wifi_essid          WiFi ESSID                      interface name (wlan0)
 */
static const struct arg args[] = {
	/* function format          argument */
	{ wifi_essid,	"󰖩 %s ",	"wlan0" },
	{ netspeed_rx,	"RX %-5s ",	"wlan0" },
	{ netspeed_tx,	"TX %-5s ",	"wlan0" },
	{ vol_perc, 	"󰕾 %s%% ",	"Master" },
	{ cpu_perc, 	" %s%% ",	NULL },
	{ temp,		"󰔏 %s°C ",	"/sys/class/thermal/thermal_zone1/temp" },
	{ ram_perc, 	"󰍜 %s%% ",	NULL },
	{ battery_perc, "󱐋 %s%% ",	"BAT0" },
	{ datetime, 	"󰥔 %s",		"%R" },
};
