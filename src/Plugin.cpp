// ** opensource.org/licenses/GPL-3.0

#include "Plugin.hpp"

#include "Helpers.hpp"

namespace Plugin
{
	XfcePanelPlugin* mXfPlugin;
	Config* mConfig;
	GdkDevice* mPointer;

	void init(XfcePanelPlugin* xfPlugin)
	{
		mXfPlugin = xfPlugin;

		mConfig = new Config(xfce_panel_plugin_save_location(mXfPlugin, true));

		GdkDisplay* display = gdk_display_get_default();
		GdkDeviceManager* deviceManager = gdk_display_get_device_manager(display);
		mPointer = gdk_device_manager_get_client_pointer(deviceManager);

		AppInfos::init();

		Dock::init();
		Wnck::init();

		Theme::init(GTK_WIDGET(Dock::mBox));

		//--------------------------------------------------

		gtk_container_add(GTK_CONTAINER(mXfPlugin), GTK_WIDGET(Dock::mBox));

		xfce_panel_plugin_menu_show_configure(mXfPlugin);

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
				GtkWidget* dialog = Settings::popup();
				//Theme::applyDefault(dialog);
			}),
			NULL);
	}

	void getPointerPosition(gint* x, gint* y)
	{
		gdk_device_get_position(mPointer, NULL, x, y);
	}

} // namespace Plugin

//----------------------------------------------------------------------------------------------------------------------

extern "C" void construct(XfcePanelPlugin* xfPlugin)
{
	// xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");
	Plugin::init(xfPlugin);
}
