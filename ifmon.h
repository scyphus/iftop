/*_
 * Copyright 2011 Scyphus Solutions Co. Ltd.  All rights reserved.
 *
 * Authors:
 *      Hirochika Asai  <asai@scyphus.co.jp>
 */

#ifndef _IFMON_H
#define _IFMON_H

#include "config.h"
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#define PATH_PROC_NET_DEV "/proc/net/dev"

struct ifmon_stat {
    double time;
    struct {
        uint64_t tx;
        uint64_t rx;
    } total_pkts;
    struct {
        uint64_t tx;
        uint64_t rx;
    } total_bytes;
};

/*
 * Interface monitor instance
 */
typedef struct _ifmon {
    char *ifname;
    struct ifmon_stat prevstat;
    struct ifmon_stat curstat;
    /* Free call required? */
    int _need_to_free;
} ifmon_t;

#ifdef __cplusplus
extern "C" {
#endif

    ifmon_t * ifmon_init(ifmon_t *, const char *);
    int ifmon_update(ifmon_t *);

#ifdef __cplusplus
}
#endif

#endif /* _IFMON_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
