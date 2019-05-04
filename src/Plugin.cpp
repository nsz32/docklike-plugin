// ** opensource.org/licenses/GPL-3.0

#include "Plugin.hpp"
#include "Helpers.hpp"

namespace Plugin
{
	XfcePanelPlugin* mXfPlugin;
	Config* mConfig;
	Help::Gtk::Timeout mTimeout;

	void init(XfcePanelPlugin* xfPlugin)
	{
		mXfPlugin = xfPlugin;

		mConfig = new Config(xfce_panel_plugin_save_location(mXfPlugin, true));

		AppInfos::init();

		Theme::init(gtk_widget_get_parent(GTK_WIDGET(mXfPlugin)));

		Dock::init();
		Wnck::init();

		//--------------------------------------------------

		gtk_container_add(GTK_CONTAINER(xfPlugin), GTK_WIDGET(Dock::mBox));

		//TODO orientation, settings, ...

		//--------------------------------------------------

		g_signal_connect(G_OBJECT(GTK_WIDGET(mXfPlugin)), "size-changed",
		G_CALLBACK(+[](XfcePanelPlugin *plugin, gint size){
			Dock::onPanelResize(size);
			return true;
		}), NULL);

		g_signal_connect(G_OBJECT(GTK_WIDGET(mXfPlugin)), "orientation-changed",
		G_CALLBACK(+[](XfcePanelPlugin *plugin, GtkOrientation orientation){
			Dock::onPanelOrientationChange(orientation);
		}), NULL);

		mTimeout.setup(1000000, [](){
			
			GdkDisplay *display = gdk_display_get_default ();
			GdkDeviceManager *device_manager = gdk_display_get_device_manager (display);
			GdkDevice *device = gdk_device_manager_get_client_pointer (device_manager);

			// do whatever with Gdk.Device, i.e:
			int x, y;
			gdk_device_get_position (device, NULL, &x, &y);
			std::cout << "pos:" << x << " " << y << std::endl;

			return true;
		});

		mTimeout.start();
		
	}
}


//----------------------------------------------------------------------------------------------------------------------

extern "C" void construct(XfcePanelPlugin* xfPlugin)
{
	//xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");
	Plugin::init(xfPlugin);
}
