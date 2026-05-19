#ifndef TOON_STATS_H
#define TOON_STATS_H

#include <stddef.h>

/* hcb_rrd HTTP fetch + JSON parse for the stats screen.
   The endpoint returns: {"DD-MM-YYYY HH:MM:SS": value, ...}
   We parse into parallel arrays of timestamps + floats (NaN = null/missing). */

#define STATS_MAX_SAMPLES 512

typedef struct {
    int    n;
    double samples[STATS_MAX_SAMPLES];   /* value per slot (NaN for null) */
    char   labels[STATS_MAX_SAMPLES][20]; /* "DD-MM HH:MM" short label */
    double min, max;
} stats_series_t;

/* Fetch raw history JSON for a given loggerName + rra. Returns 0 on success.
   Parsed values land in `out` (max STATS_MAX_SAMPLES samples).
   `max_samples` caps the response to the most-recent N entries — needed
   because the RRD archive lives much further back than any single chart
   window. Pass STATS_MAX_SAMPLES for "as many as fit". */
int stats_fetch(const char * logger_name, const char * rra, int max_samples,
                stats_series_t * out);

/* Convenience pre-defined fetches. */
int stats_elec_flow_5min(stats_series_t * out);
int stats_gas_flow_5min(stats_series_t * out);
int stats_water_flow_5min(stats_series_t * out);

#endif
