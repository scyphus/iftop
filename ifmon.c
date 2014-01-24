/*_
 * Copyright 2011 Scyphus Solutions Co. Ltd.  All rights reserved.
 *
 * Authors:
 *      Hirochika Asai  <asai@scyphus.co.jp>
 */

#include "config.h"
#include "ifmon.h"
#include "stime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#if TARGET_DARWIN || TARGET_FREEBSD

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netinet/in.h>
#include <netinet/in_pcb.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/tcp.h>
#include <netinet/tcp_var.h>
#include <errno.h>
#include <sys/time.h>
#include <net/if.h>
#include <net/if_mib.h>

#elif TARGET_LINUX

#else
#error "Unsupported operating system"
#endif


/*
 * Prototype declarations
 */
static int _get_stat(ifmon_t *, struct ifmon_stat *);
static int _update_stat(ifmon_t *, struct ifmon_stat *, struct ifmon_stat *);

ifmon_t *
ifmon_init(ifmon_t *ifmon, const char *ifname)
{
    int ret;

    if ( NULL == ifmon ) {
        ifmon = malloc(sizeof(ifmon_t));
        if ( NULL == ifmon ) {
            return NULL;
        }
        ifmon->_need_to_free = 1;
    } else {
        ifmon->_need_to_free = 0;
    }

    /* Get interface name */
    ifmon->ifname = strdup(ifname);
    if ( NULL == ifmon->ifname ) {
        if ( ifmon->_need_to_free ) {
            free(ifmon);
        }
        return NULL;
    }

    /* Initialize */
    bzero(&(ifmon->prevstat), sizeof(struct ifmon_stat));
    ret =  _get_stat(ifmon, &(ifmon->curstat));
    if ( 0 != ret ) {
        if ( ifmon->_need_to_free ) {
            free(ifmon);
        }
        return NULL;
    }

    return ifmon;
}

int
ifmon_update(ifmon_t *ifmon)
{
    return _update_stat(ifmon, &(ifmon->curstat), &(ifmon->prevstat));
}

static int
_update_stat(
    ifmon_t *ifmon, struct ifmon_stat *curstat, struct ifmon_stat *prevstat)
{
    struct ifmon_stat newstat;
    int ret;

    ret =  _get_stat(ifmon, &newstat);
    if ( 0 != ret ) {
        return -1;
    }
    (void)memcpy(prevstat, curstat, sizeof(struct ifmon_stat));
    (void)memcpy(curstat, &newstat, sizeof(struct ifmon_stat));

    return 0;
}

#if TARGET_DARWIN || TARGET_FREEBSD
static int
_search_if_index(const char *ifname)
{
    unsigned int numif = 0;
    size_t len = 0;
    static int mib1[5] = { CTL_NET, PF_LINK, NETLINK_GENERIC, IFMIB_SYSTEM,
                           IFMIB_IFCOUNT };
    static int mib2[6] = { CTL_NET, PF_LINK, NETLINK_GENERIC, IFMIB_IFDATA,
                           0, IFDATA_GENERAL };
    struct ifmibdata mibdata;
    int idx;
    int ret;

    len = sizeof(numif);
    ret = sysctl(mib1, 5, &numif, &len, NULL, 0);
    if ( ret < 0 ) {
        return -1;
    }

    len = sizeof(struct ifmibdata);

    for ( idx = 1; idx <= numif; idx++ ) {
        mib2[4] = idx;
        ret = sysctl(mib2, 6, &mibdata, &len, NULL, 0);
        if ( ret < 0 ) {
            if ( ENOENT == errno ) {
                /* End of interface */
                break;
            } else {
                /* Error */
                continue;
            }
        } else {
            /* No error */
            if ( 0 == strcmp(ifname, mibdata.ifmd_name) ) {
                return idx;
            }
        }
    }

    return 0;
}
#endif

static int
_get_stat(ifmon_t *ifmon, struct ifmon_stat *stat)
{
#if TARGET_DARWIN || TARGET_FREEBSD
    int idx;
    int ret;
    size_t len = 0;
    static int mib[6] = { CTL_NET, PF_LINK, NETLINK_GENERIC, IFMIB_IFDATA,
                          0, IFDATA_GENERAL };
    struct ifmibdata mibdata;

    idx = _search_if_index(ifmon->ifname);
    if ( idx < 0 ) {
        /* Error */
        return -1;
    } else if ( 0 == idx ) {
        /* Not found */
        return -1;
    }

    len = sizeof(struct ifmibdata);
    mib[4] = idx;

    stat->time = microtime();
    ret = sysctl(mib, 6, &mibdata, &len, NULL, 0);
    if ( ret < 0 ) {
        return 0;
    }
    stat->total_pkts.tx = mibdata.ifmd_data.ifi_opackets;
    stat->total_pkts.rx = mibdata.ifmd_data.ifi_ipackets;
    stat->total_bytes.tx = mibdata.ifmd_data.ifi_obytes;
    stat->total_bytes.rx = mibdata.ifmd_data.ifi_ibytes;

#elif TARGET_LINUX

    FILE *fp;
    int ret;
    char buf[4096];
    char ifname[256];
    unsigned long long rbyes;
    unsigned long long rpackets;
    unsigned long long rerrs;
    unsigned long long rdrop;
    unsigned long long rfifo;
    unsigned long long rframe;
    unsigned long long rcompressed;
    unsigned long long rmulticast;
    unsigned long long tbyes;
    unsigned long long tpackets;
    unsigned long long terrs;
    unsigned long long tdrop;
    unsigned long long tfifo;
    unsigned long long tcolls;
    unsigned long long tcarrier;
    unsigned long long tcompressed;

    /* Open file /proc/net/dev that has interface statistics */
    fp = fopen(PATH_PROC_NET_DEV, "r");
    if ( NULL == fp ) {
        return -1;
    }
    /* Eat two lines */
    (void)fgets(buf, sizeof(buf), fp);
    (void)fgets(buf, sizeof(buf), fp);

    /* Set time */
    stat->time = microtime();
    stat->total_pkts.tx = 0;
    stat->total_pkts.rx = 0;
    stat->total_bytes.tx = 0;
    stat->total_bytes.rx = 0;

    while ( !feof(fp) ) {
        if ( NULL != fgets(buf, sizeof(buf), fp) ) {
            /* Parse */
            ret = sscanf(
                buf, " %255[^: ]:%llu %llu %llu %llu %llu %llu %llu %llu "
                "%llu %llu %llu %llu %llu %llu %llu %llu",
                ifname, &rbyes, &rpackets, &rerrs, &rdrop, &rfifo, &rframe,
                &rcompressed, &rmulticast, &tbyes, &tpackets, &terrs,
                &tdrop, &tfifo, &tcolls, &tcarrier, &tcompressed);
            if ( ret <= 0 ) {
                continue;
            }
            if ( 0 == strcmp(ifmon->ifname, ifname) ) {
                stat->total_pkts.tx = tpackets;
                stat->total_pkts.rx = rpackets;
                stat->total_bytes.tx = tbyes;
                stat->total_bytes.rx = rbyes;
                break;
            }
        } else {
            /* Error */
            break;
        }
    }

    (void)fclose(fp);

#else
#error "Unsupported operating system"
#endif

    return 0;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
