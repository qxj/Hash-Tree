#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT(gsync, 0.1, junist@gmail.com)
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_PROG_RANLIB

AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
dnl AC_CHECK_LIB([libboost_thread], [main], [], [AC_MSG_ERROR([libboost_thread.so is missing.])])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strerror])

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 src/Makefile
                 test/Makefile])

AC_OUTPUT
