# Based off original GNOME 2 applet build files copyright:
#
# 2003-2007 Ole Laursen <olau@hardworking.dk>
#
# XFCE4 modifications copyright:
#
# 2013 OmegaPhil (OmegaPhil@startmail.com)


@SET_MAKE@

SUBDIRS = src

# Installing the plugin's desktop file in the correct place
#desktopdir = $(datadir)/xfce4/panel/plugins
#desktop_DATA = $(PACKAGE_NAME).desktop

#
# Desktop file
#
desktopdir = /usr/share/xfce4/panel/plugins
desktop_in_files = docklike.desktop
desktop_DATA = $(desktop_in_files)

EXTRA_DIST = $(desktop_in_files)

# Clean generated files
#CLEANFILES = $(desktop_DATA)
