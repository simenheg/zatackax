#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([Zatacka X],[1.1.0],[simenheg@runbox.com],[zatackax],[https://github.com/simenheg/zatackax])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src/zatackax.c])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET

# Checks for libraries.
AC_CHECK_LIB([SDL2], [main], [],
  [AC_MSG_ERROR([cannot find library SDL2])])
AC_CHECK_LIB([SDL2_image], [main], [],
  [AC_MSG_ERROR([cannot find library SDL_image])])
AC_CHECK_LIB([SDL2_mixer], [main], [],
  [AC_MSG_ERROR([cannot find library SDL_mixer])])
AC_CHECK_LIB([SDL2_ttf], [main], [],
  [AC_MSG_ERROR([cannot find library SDL_ttf])])
AC_CHECK_LIB([m], [main])

# Checks for header files.
AC_CHECK_HEADERS([malloc.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit memset])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
