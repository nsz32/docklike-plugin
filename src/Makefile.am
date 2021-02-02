AM_CXXFLAGS = -DWNCK_I_KNOW_THIS_IS_UNSTABLE -DPACKAGE_LOCALE_DIR=\"$(localedir)\" $(GTK_CFLAGS) $(CAIRO_CFLAGS) $(WNCK_CFLAGS) $(SENSORS_CFLAGS) --std=c++11

plugindir = $(libdir)/xfce4/panel/plugins
plugin_LTLIBRARIES = libdocklike.la

libdocklike_la_SOURCES =							\
		Hotkeys.cpp Hotkeys.hpp						\
		Theme.cpp Theme.hpp							\
		AppInfos.cpp AppInfos.hpp					\
		Settings.cpp Settings.hpp					\
		Group.cpp Group.hpp							\
		GroupMenu.cpp GroupMenu.hpp					\
		GroupMenuItem.cpp GroupMenuItem.hpp			\
		GroupWindow.cpp GroupWindow.hpp				\
		Helpers.cpp Helpers.hpp						\
		Plugin.cpp Plugin.hpp						\
		Dock.cpp Dock.hpp							\
		Wnck.cpp Wnck.hpp							\
		SettingsDialog.cpp SettingsDialog.hpp		\
		State.tpp 									\
		Store.tpp									\
		register.c									\
		_gresources.xml _dialogs.xml				\
		_gresources.c

_gresources.c: _gresources.xml _dialogs.xml
	glib-compile-resources --generate-source --target _gresources.c _gresources.xml

libdocklike_la_CPPFLAGS = @LIBXFCE4PANEL_CFLAGS@ @LIBXFCE4UI_CFLAGS@ $(X11_CFLAGS)

libdocklike_la_LDFLAGS = -avoid-version -module -no-undefined -export-symbols-regex '^xfce_panel_module_(preinit|init|construct)'

libdocklike_la_LIBADD = @LIBXFCE4PANEL_LIBS@ @LIBXFCE4UI_LIBS@ $(X11_LIBS) $(GTK_LIBS) $(CAIRO_LIBS) $(WNCK_LIBS)

pluginsdir = $(datadir)/xfce4/panel/plugins
plugins_in_files = docklike.desktop.in
plugins_DATA = $(plugins_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = $(plugins_in_files)

CLEANFILES = $(NULL)
DISTCLEANFILES = docklike.desktop