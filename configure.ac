AC_INIT([docklike-plugin], [0.0.1])
AM_INIT_AUTOMAKE([1.15.0 -Wall -Werror])

AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
#IT_PROG_INTLTOOL([0.40.0])
AM_PROG_AR

dnl Initialize libtool, prevent static libraries to remove pointless
dnl compilation
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

dnl Check for standard header files
AC_HEADER_STDC

dnl Configure the panel plugin (XFCE4 xdt-depends.m4)
XDT_CHECK_PACKAGE([LIBXFCE4PANEL], [libxfce4panel-2.0], [4.12.0])
XDT_CHECK_PACKAGE([LIBXFCE4UI], [libxfce4ui-2], [4.12.0])

dnl Checks for libraries
PKG_CHECK_MODULES([X11], [x11 >= 1.6])
PKG_CHECK_MODULES([WNCK], [libwnck-3.0 >= 3.30.0])
PKG_CHECK_MODULES([CAIRO], [cairo >= 1.16.0])
PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.24.0])

AC_SUBST([WNCK_CFLAGS])
AC_SUBST([CAIRO_LIBS])
AC_SUBST([WNCK_LIBS])

AC_SUBST([GTK_CFLAGS])
AC_SUBST([CAIRO_LIBS])
AC_SUBST([GTK_LIBS])

_cppflags="${CPPFLAGS}"
_ldflags="${LDFLAGS}"
 
CPPFLAGS=${_cppflags}
LDFLAGS=${_ldflags}

dnl Create makefiles
AC_CONFIG_FILES([Makefile 
        src/Makefile
])
AC_OUTPUT