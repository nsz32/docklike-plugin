// ** opensource.org/licenses/GPL-3.0

#include "Plugin.hpp"

namespace Plugin
{
	XfcePanelPlugin* mXfPlugin;
	Config* mConfig;

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
		
	}
}


//----------------------------------------------------------------------------------------------------------------------

extern "C" void construct(XfcePanelPlugin* xfPlugin)
{
	//xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");
	Plugin::init(xfPlugin);
}
