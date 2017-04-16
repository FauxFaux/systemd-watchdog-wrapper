AC_INIT([systemd-watchdog-wrapper], [1.0], [chris@chris-lamb.co.uk])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])

PKG_CHECK_MODULES([libsystemd], [libsystemd >= 232])

AC_OUTPUT
