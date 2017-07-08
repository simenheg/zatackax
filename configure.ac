#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Zatacka X], [0.2.0], [simenheg@gmail.com], [zatackax],
  [https://github.com/simenheg/zatackax])
AC_CONFIG_SRCDIR([src/zatackax.c])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_MAKE_SET

# Checks for libraries.
AC_HAVE_LIBRARY([SDL], [],
  [AC_MSG_ERROR([cannot find library SDL])])
AC_HAVE_LIBRARY([SDL_image], [],
  [AC_MSG_ERROR([cannot find library SDL_image])])
AC_HAVE_LIBRARY([SDL_mixer], [],
  [AC_MSG_ERROR([cannot find library SDL_mixer])])
AC_HAVE_LIBRARY([SDL_ttf], [],
  [AC_MSG_ERROR([cannot find library SDL_ttf])])
AC_HAVE_LIBRARY([m])

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
