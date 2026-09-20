AC_PREREQ([2.69])
AC_INIT([vulkan_graphix], [1.0], [mehoggan@gmail.com])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

PKG_PROG_PKG_CONFIG
if test -z "$PKG_CONFIG"; then
  AC_MSG_WARN([Cannot find pkg-config, make sure it is installed in your PATH])
fi

PKG_CHECK_MODULES([VULKAN], [vulkan])
PKG_CHECK_MODULES([X11], [x11])
PKG_CHECK_MODULES([GLM], [glm])

# vulkan_earth: a legacy OpenGL/GLUT game vendored under vulkan_earth/,
# ported off MSVC/Win32 onto X11 via GLUT (see vulkan_earth/src/Makefile.am).
PKG_CHECK_MODULES([VULKAN_EARTH_GL], [gl glu glut])
PKG_CHECK_MODULES([VULKAN_EARTH_GLEW], [glew])
PKG_CHECK_MODULES([VULKAN_EARTH_SDL], [sdl SDL_mixer])

PKG_CHECK_MODULES([GTEST], [gtest gtest_main],
  [have_gtest=yes], [have_gtest=no])
AM_CONDITIONAL([HAVE_GTEST], [test "x$have_gtest" = "xyes"])
if test "x$have_gtest" = "xno"; then
  AC_MSG_WARN([gtest/gtest_main not found via pkg-config, tests/ will not build])
fi

# Code coverage (gcov instrumentation for lib/ and tests/; see README.md)
AC_ARG_ENABLE([coverage],
  [AS_HELP_STRING([--enable-coverage],
    [instrument lib/ and tests/ with gcov for code coverage reports])],
  [enable_coverage=$enableval], [enable_coverage=no])
AM_CONDITIONAL([COVERAGE_ENABLED], [test "x$enable_coverage" = "xyes"])

COVERAGE_CXXFLAGS=
COVERAGE_LDFLAGS=
if test "x$enable_coverage" = "xyes"; then
  # Force CXXFLAGS itself (not just a value we AC_SUBST elsewhere) here,
  # before AC_PROG_CXX below would otherwise default it to "-g -O2": a
  # per-target Makefile.am `+=` can only ever *append* to CPPFLAGS, and
  # every Automake compile recipe places the global $(CXXFLAGS) last on
  # the command line (by design, so users can always override package
  # flags) - so an appended "-O0" there previously lost to the trailing
  # "-O2" every time, silently building "coverage" binaries at -O2. That
  # optimizes away small inline functions (e.g. accessor one-liners)
  # entirely, so gcov can never mark their line as executed even though
  # they ran - under-reporting coverage for exactly the code most likely
  # to be fully exercised. -O0 keeps line-level coverage accurate.
  CXXFLAGS="-g -O0"
  COVERAGE_CXXFLAGS="--coverage"
  COVERAGE_LDFLAGS="--coverage"

  AC_CHECK_PROG([GCOVR], [gcovr], [gcovr], [])
  AC_CHECK_PROG([LCOV], [lcov], [lcov], [])
  AC_CHECK_PROG([GENHTML], [genhtml], [genhtml], [])
  if test -z "$GCOVR" && test -z "$LCOV"; then
    AC_MSG_WARN([--enable-coverage was given but neither gcovr nor lcov was found on PATH - install one to turn the raw .gcda/.gcno files into a report (pip install --user gcovr, or apt install lcov)])
  fi
fi
AC_SUBST([COVERAGE_CXXFLAGS])
AC_SUBST([COVERAGE_LDFLAGS])

# Conditionals
AC_CANONICAL_HOST()

# Detect the target system
build_linux=no
build_windows=no
build_mac=no
case "${host_os}" in
	linux*)
		build_linux=yes
		AC_CHECK_HEADERS([limits.h])
		AC_CHECK_LIB(m, sincos,
			[LIBS="-lm $LIBS" AC_DEFINE(HAVE_LIBM, 1, [Define if you have m.])],
			[AC_MSG_ERROR(
        [lib m is required - see https://www.gnu.org/software/libc/])])
    AC_PROG_CXX([g++])
    AC_MSG_RESULT(["Going to use $CXX as compiler in Linux environment."])
		;;
	darwin*)
		build_mac=yes
    AC_PROG_CXX([clang++])
    AC_MSG_RESULT(["Going to use $CXX as compiler in OSx environtment."])
		;;
	*)
		AC_MSG_ERROR(["OS $host_os is not supported"])
		;;
esac
AM_CONDITIONAL([LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([OSX], [test "$build_mac" = "yes"])

LT_INIT()

AX_CXX_COMPILE_STDCXX([20], [noext], [mandatory])
AC_CONFIG_SRCDIR([bin/tutorial01_main.cpp])

AC_CONFIG_FILES([
  Makefile
  lib/Makefile
  bin/Makefile
  tests/Makefile
  vulkan_earth/src/Makefile
])

AC_OUTPUT
