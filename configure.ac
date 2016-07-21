#
# Copyright (c) 2012,2014 Scyphus Solutions Co. Ltd.  All rights reserved.
# Authors:
#      Hirochika Asai  <asai@scyphus.co.jp>
#

AC_PREREQ(2.59)
AC_INIT([iftop], [0.1.1], [bug@scyphus.co.jp])
AC_CONFIG_SRCDIR([iftop.c])
AC_CONFIG_HEADER([config.h])

# Get hostname and other information.
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE
AC_SUBST(SUBDIRS)

# arguments
AC_ARG_ENABLE(debug,
  [  --enable-debug    Turn on debugging [default no]],
  [case "${enableval}" in
    yes) debug=yes; AC_MSG_RESULT(Checking for debug... yes); AC_DEFINE(DEBUG, 1, debugging option) ;;
    no)  debug=no;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
  esac],[debug=no])
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)

## ncurses
AC_ARG_WITH([ncurses],
  AC_HELP_STRING(
    [--with-ncurses@<:@=PATH@:>@],
    [PATH is path to 'ncurses-config']),
    [WITH_CURSES=$withval], [WITH_CURSES=yes])
if test x"$WITH_CURSES" = x"yes"; then
  AC_PATH_PROGS([CURSES_CONFIG], [ncurses-config ncurses5-config], [no])
  if test "$CURSES_CONFIG" = "no"; then
    AC_MSG_ERROR([ncurses-config/ncurses5-config not found $CURSES_CONFIG])
  fi
  CURSES_INCLUDE="`$CURSES_CONFIG --includedir`"
  CURSES_LIBS="`$CURSES_CONFIG --libs`"
  AC_SUBST(CURSES_INCLUDE)
  AC_SUBST(CURSES_LIBS)
  AC_DEFINE(WITH_CURSES, 1, with ncurses)
elif test x"$WITH_CURSES" = x"no"; then
  AC_DEFINE(WITH_CURSES, 0, without ncurses)
else
  AC_PATH_PROG([CURSES_CONFIG], [$WITH_CURSES], [no])
  if test "$CURSES_CONFIG" = "no"; then
    AC_MSG_ERROR([$WITH_CURSES not found])
  fi
fi
# Enable ncurses
#CURSES_LIBS="-lcurses"
#AC_SUBST(CURSES_LIBS)
#AC_DEFINE(WITH_CURSES, 1, with ncurses)

# Checks for hosts
case $host_os in
     freebsd*)
        AC_DEFINE(TARGET_FREEBSD, 1, running on FreeBSD)
        arch=freebsd
        ;;
     netbsd*)
        AC_DEFINE(TARGET_NETBSD, 1, running on NetBSD)
        arch=netbsd
        ;;
     darwin*)
        AC_DEFINE(TARGET_DARWIN, 1, running on Mac OS X)
        arch=darwin
        ;;
     linux*)
        AC_DEFINE(TARGET_LINUX, 1, running on Linux)
        arch=linux
        ;;
     *) ;;
esac

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
#AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
#AC_CHECK_FUNCS([])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
