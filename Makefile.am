#
# Copyright (c) 2012,2014 Scyphus Solutions Co. Ltd.  All rights reserved.
# Authors:
#      Hirochika Asai  <asai@scyphus.co.jp>
#

#INCLUDES = -I$(top_srcdir)/include
INCLUDES = -I$(CURSES_INCLUDES)

bin_PROGRAMS = iftop
iftop_SOURCES = iftop.c ifmon.c ifmon.h stime.c stime.h
#iftop_LDFLAGS = $(CURSES_LIBS)
iftop_LDADD = $(CURSES_LIBS)

CLEANFILES = *~

