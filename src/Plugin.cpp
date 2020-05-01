/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include "Plugin.hpp"

#include "Helpers.hpp"

namespace Plugin
{
	XfcePanelPlugin* mXfPlugin;
	GdkDevice* mPointer;

	void init(XfcePanelPlugin* xfPlugin)
	{
		mXfPlugin = xfPlugin;

		GdkDisplay* display = gdk_display_get_default();
		GdkDeviceManager* deviceManager = gdk_display_get_device_manager(display);
		mPointer = gdk_device_manager_get_client_pointer(deviceManager);

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
			"© 2019-2020 Nicolas Szabo (original author)",
			"<nszabo@vivaldi.net>  |  github.com/nsz32",
			//"",
			//_("Contributors:"),
			//"(c) 2020 ",
			NULL};

		gtk_show_about_dialog(NULL,
			"program-name", _("Docklike Taskbar"),
			"logo-icon-name", "preferences-system-windows",
			"comments", _("A modern, minimalist taskbar for XFCE."),
			//"version", PACKAGE_VERSION,
			"copyright", "Copyright © 2019-2020",
			"license", "This program is free software : you can redistribute it and / or modify it under\n"
					   "the terms of the GNU General Public License as published by the Free Software\n"
					   "Foundation, either version 3 of the License, or (at your option) any later version.\n\n"

					   "This program is distributed in the hope that it will be useful, but WITHOUT ANY\n"
					   "WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n"
					   "FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\n"

					   "You should have received a copy of the GNU General Public License\n"
					   "along with this program.  If not, see <https://www.gnu.org/licenses/>.",
			"website", "https://github.com/nsz32/docklike-plugin/",
			"website-label", "docklike-plugin",
			"authors", authors,
			//"documenters", documenters,
			//"translator-credits", translator-credits,
			NULL);
	}

} // namespace Plugin

//----------------------------------------------------------------------------------------------------------------------

extern "C" void construct(XfcePanelPlugin* xfPlugin)
{
	// xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");
	Plugin::init(xfPlugin);
}
