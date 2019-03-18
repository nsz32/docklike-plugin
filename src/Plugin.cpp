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
		Taskbar::init();

		//--------------------------------------------------

		gtk_container_add(GTK_CONTAINER(xfPlugin), GTK_WIDGET(Taskbar::mBoxWidget));

		Glib::ustring data = "button.opened { box-shadow: inset 0px -1px shade(@theme_selected_bg_color, 0.75); }"
		"button.active { box-shadow: inset 0px -1px shade(@theme_selected_bg_color, 1.15); }"
		"button.drop { border-left: 5px solid @theme_selected_bg_color; }";

		Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();

		if(!cssProvider->load_from_data(data)) {
			std::cerr << "Failed to load css !!\n";
		}

		Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();

		Gtk::StyleContext::add_provider_for_screen(screen, cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

		//TODO orientation, settings, ...

		//--------------------------------------------------
	}
}


//----------------------------------------------------------------------------------------------------------------------

extern "C" void construct(XfcePanelPlugin* xfPlugin)
{
	Gtk::Main::init_gtkmm_internals();

	//xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

	Plugin::init(xfPlugin);
}
