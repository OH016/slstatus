/* See LICENSE file for copyright and license details. */

/* battery */
const char *battery_perc(const char *);

/* cpu */
const char *cpu_perc(const char *unused);

/* datetime */
const char *datetime(const char *fmt);

/* netspeeds */
const char *netspeed_rx(const char *interface);
const char *netspeed_tx(const char *interface);

/* ram */
const char *ram_perc(const char *unused);

/* temperature */
const char *temp(const char *);

/* volume */
const char *vol_perc(const char *card);

/* wifi */
const char *wifi_essid(const char *interface);
