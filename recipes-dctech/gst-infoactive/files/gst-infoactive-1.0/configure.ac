#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#AC_PREREQ(2.60)
AC_INIT(gst-infoactive, 0.1, carlos.garcia@nxp.com)
AC_CONFIG_SRCDIR([gst-src/gst_infoactive.c])
#AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE(gst-src/gst_infoactive, main)

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.
AM_PROG_LIBTOOL

# Set shared libraries
AC_DISABLE_STATIC
AC_ENABLE_SHARED

# Checks for header files.
AC_CHECK_HEADERS([gst-src/commons.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
#AC_CONFIG_LINKS([libgreio.a:linux-imx6yocto-armle-opengles_2.0-obj/lib/libgreio.a])

GST_REQUIRED=1.0
GST_MAJORMINOR=1.0
PKG_CHECK_MODULES(GST, gstreamer-$GST_MAJORMINOR >= $GST_REQUIRED, HAVE_GST=yes,HAVE_GST=no)
#PKG_CHECK_MODULES([GSTREAMER],[gstreamer-1.0],[have_gstreamer-1.0=yes],[have_gstreamer-1.0=no])
#PKG_CHECK_MODULES([GLIB],[glib-2.0],[have_glib-2.0=yes],[have_glib-2.0=no])

SB_LIB_DIR="linux-imx6yocto-armle-opengles_2.0-obj/lib"
AC_SUBST(SB_LIB_DIR)
HOSTDIR=$srcdir
AC_SUBST(HOSTDIR)
#AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile gst-src/Makefile infoactive-hmi/Makefile fbset-src/Makefile])
AC_OUTPUT
