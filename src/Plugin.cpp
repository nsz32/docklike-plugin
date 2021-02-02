/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include "Plugin.hpp"

#include "Helpers.hpp"

/* the website url */
#define PLUGIN_WEBSITE "https://github.com/nsz32/docklike-plugin"

namespace Plugin
{
	XfcePanelPlugin* mXfPlugin;
	GdkDevice* mPointer;
	GdkDisplay* display;
	GdkSeat* seat;

	void init(XfcePanelPlugin* xfPlugin)
	{
		xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, NULL);

		mXfPlugin = xfPlugin;

		display = gdk_display_get_default();
		seat = gdk_display_get_default_seat(display);
		mPointer = gdk_seat_get_pointer(seat);

		Settings::init();
		AppInfos::init();
		Dock::init();
		Wnck::init();
		Theme::init();
		Hotkeys::init();

		//--------------------------------------------------

		gtk_container_add(GTK_CONTAINER(mXfPlugin), GTK_WIDGET(Dock::mBox));

		xfce_panel_plugin_menu_show_configure(mXfPlugin);
		xfce_panel_plugin_menu_show_about(mXfPlugin);

		//--------------------------------------------------

		g_signal_connect(G_OBJECT(GTK_WIDGET(mXfPlugin)), "size-changed",
			G_CALLBACK(+[](XfcePanelPlugin* plugin, gint size) {
				Dock::onPanelResize(size);
				return true;
			}),
			NULL);

		g_signal_connect(G_OBJECT(GTK_WIDGET(mXfPlugin)), "orientation-changed",
			G_CALLBACK(+[](XfcePanelPlugin* plugin, GtkOrientation orientation) {
				Dock::onPanelOrientationChange(orientation);
			}),
			NULL);

		g_signal_connect(G_OBJECT(mXfPlugin), "configure-plugin",
			G_CALLBACK(+[](XfcePanelPlugin* plugin) {
				SettingsDialog::popup();
			}),
			NULL);

		g_signal_connect(G_OBJECT(mXfPlugin), "about",
			G_CALLBACK(+[](XfcePanelPlugin* plugin) {
				Plugin::aboutDialog();
			}),
			NULL);
	}

	void getPointerPosition(gint* x, gint* y)
	{
		gdk_device_get_position(mPointer, NULL, x, y);
	}

	void aboutDialog()
	{
		const gchar* authors[] = {
			"Nicolas Szabo <nszabo@vivaldi.net> © 2019-2020",
			"David Keogh <davidtkeogh@gmail.com> © 2020-2021"
			"\n",
			NULL};

		const gchar* translators =
			"Yamada Hayao <hayao@fascode.net> : ja\n"
			"Dmitry K <dkabishchev@ya.ru> : ru\n"
			"Fábio Meneghetti <fabiom@riseup.net> : pt\n"
			"Mirko Brombin <send@mirko.pm> : it\n"
			"Adem Kürşat Uzun <ademkursatuzun@gmail.com> : tr\n"
			"Santiago Soler <santiago.r.soler@gmail.com> : es\n"
			"fredii: de\n"
			"Lucas Hadjilucas <puzzle@outlook.com> : el\n"
			"Jan Kazemier : nl\n";

		gtk_show_about_dialog(NULL,
			"program-name", "Docklike Taskbar",
			"logo-icon-name", "preferences-system-windows",
			"comments", _("A modern, minimalist taskbar for XFCE."),
			"version", PACKAGE_VERSION,
			"copyright", "Copyright © 2019-2021",
			"license", "This program is free software : you can redistribute it and / or modify it under\n"
					   "the terms of the GNU General Public License as published by the Free Software\n"
					   "Foundation, either version 3 of the License, or (at your option) any later version.\n\n"

					   "This program is distributed in the hope that it will be useful, but WITHOUT ANY\n"
					   "WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n"
					   "FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\n"

					   "You should have received a copy of the GNU General Public License\n"
					   "along with this program.  If not, see <https://www.gnu.org/licenses/>.",
			"website", PLUGIN_WEBSITE,
			"website-label", "xfce4-docklike-plugin",
			"authors", authors,
			"translator-credits", translators,
			//"documenters", documenters,
			NULL);
	}

} // namespace Plugin

//----------------------------------------------------------------------------------------------------------------------

extern "C" void construct(XfcePanelPlugin* xfPlugin)
{
	// xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");
	Plugin::init(xfPlugin);
}
