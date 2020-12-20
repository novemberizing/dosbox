AC_INIT([novemberizing-zip], [0.0.1], [novemberizing@gmail.com])

dnl -----------------------------------------------------
dnl CHECK FOR EMSCRIPTEN
dnl -----------------------------------------------------
AS_CASE([$CC], [*emcc], [enable_emscripten="yes"], [enable_emscripten="no"])
AS_IF([test "x$enable_emscripten" = "xyes"], AC_MSG_NOTICE([detect use of emscripten]))
AS_IF([test "x$enable_emscripten" = "xyes"], [ac_cv_exeext=.js])
AM_CONDITIONAL([EMSCRIPTEN], [test "x$enable_emscripten" = "xyes"])

AC_PROG_CC
AM_PROG_AR

AC_CONFIG_MACRO_DIRS([m4])

dnl Setup for automake
AM_INIT_AUTOMAKE

LT_INIT([disable-shared])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
])
AC_OUTPUT
