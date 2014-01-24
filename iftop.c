/*_
 * Copyright 2011 Scyphus Solutions Co. Ltd.  All rights reserved.
 *
 * Authors:
 *      Hirochika Asai  <asai@scyphus.co.jp>
 */

/* $Id$ */

#define TRAFFIC_MGT 1

#include "config.h"
#include "ifmon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#if WITH_CURSES
#include <curses.h>
#endif

static void
_usage(const char *prog)
{
    fprintf(stderr, "Usage: %s ifname\n", prog);
    exit(EXIT_FAILURE);
}

int
main(int argc, const char *const argv[], const char *const envp[])
{
    ifmon_t *ifmons;
    char **ifnames;
    int nifs;
    int ret;
    int flags;
    int ch;
    double delta;
    double dtxb;
    double drxb;
    double dtxp;
    double drxp;
    int i;

    if ( 2 > argc ) {
        _usage(argv[0]);
    }
    /* Get interfaces */
    nifs = argc - 1;
    ifnames = malloc(sizeof(char *) * nifs);
    if ( NULL == ifnames ) {
        perror("malloc()");
        return EXIT_FAILURE;
    }
    ifmons = malloc(sizeof(ifmon_t) * nifs);
    if ( NULL == ifmons ) {
        perror("malloc()");
        return EXIT_FAILURE;
    }
    for ( i = 0; i < nifs; i++ ) {
        ifnames[i] = strdup(argv[i+1]);
        if ( NULL == ifnames[i] ) {
            perror("strdup()");
            return EXIT_FAILURE;
        }
        if ( NULL == ifmon_init(&ifmons[i], ifnames[i]) ) {
            /* Error */
            fprintf(stderr, "Error: ifname=%s\n", ifnames[i]);
            return EXIT_FAILURE;
        }
    }

#if WITH_CURSES
    /* curses */
    if ( NULL == initscr() ) {
        fprintf(stderr, "initscr(): Cannot initialize screen\n");
        return EXIT_FAILURE;
    }
    cbreak();
    noecho();
    scrollok(stdscr, FALSE);
    move(0, 0);
#endif

    /* Set stdin as non-blocking */
    flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if ( -1 == flags ) {
        flags = 0;
    }
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    for ( ;; ) {
#if WITH_CURSES
        ch = getch();
        if ( 'q' == ch ) {
            break;
        }
#endif

        for ( i = 0; i < nifs; i++ ) {
            ret = ifmon_update(&ifmons[i]);
            if ( 0 != ret ) {
                /* Error */
                break;
            }
        }

#if WITH_CURSES
        clear();
#endif

#if WITH_CURSES
        time_t timer;
        time(&timer);

        /*
          struct tm *tm;
          tm = localtime(&timer);
         */

        /*printw("Time: %.6f\n", );*/
        printw("Interface statistics\n%s\n", ctime(&timer));
        printw("                       pkts/s                    kbps\n");
        printw("                   TX          RX            TX          RX\n");
#endif
        for ( i = 0; i < nifs; i++ ) {
            delta = ifmons[i].curstat.time - ifmons[i].prevstat.time;
            dtxp = ifmons[i].curstat.total_pkts.tx
                - ifmons[i].prevstat.total_pkts.tx;
            drxp = ifmons[i].curstat.total_pkts.rx
                - ifmons[i].prevstat.total_pkts.rx;
            dtxb = ifmons[i].curstat.total_bytes.tx
                - ifmons[i].prevstat.total_bytes.tx;
            drxb = ifmons[i].curstat.total_bytes.rx
                - ifmons[i].prevstat.total_bytes.rx;
#if WITH_CURSES
            printw("%-8.8s: %11.02lf %11.02lf   %11.02lf %11.02lf\n",
                   ifnames[i], dtxp/delta, drxp/delta,
                   8*dtxb/delta/1000, 8*drxb/delta/1000);
#else
            printf("%-8.8s: %11.02lf %11.02lf   %11.02lf %11.02lf\n",
                   ifnames[i], dtxp/delta, drxp/delta,
                   8*dtxb/delta/1000, 8*drxb/delta/1000);
#endif
        }

#if 0
        printw("Time: %.6f (%.6f)\n",
               ifmon.curstat.time, ifmon.curstat.time - ifmon.prevstat.time);
        printw("\tTX: %15llu pkts\tRX: %15llu pkts\n",
               ifmon.curstat.total_pkts.tx - ifmon.prevstat.total_pkts.tx,
               ifmon.curstat.total_pkts.rx - ifmon.prevstat.total_pkts.rx);
        printw("\tTX: %15llu bytes\tRX: %15llu bytes\n",
               ifmon.curstat.total_bytes.tx - ifmon.prevstat.total_bytes.tx,
               ifmon.curstat.total_bytes.rx - ifmon.prevstat.total_bytes.rx);
#endif

#if WITH_CURSES
        refresh();
#endif
        sleep(1);
    }

#if WITH_CURSES
    /* End curses */
    endwin();
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
