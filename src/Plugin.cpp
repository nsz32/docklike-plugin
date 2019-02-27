// ** opensource.org/licenses/GPL-3.0

#include <iostream> 

#include "Group.hpp"
#include "XDG.hpp"

#include "Plugin.hpp"

#include <gtkmm/stylecontext.h>


nmt::Plugin::Plugin(XfcePanelPlugin* xfPlugin)
{
	init();

	mTaskbar = new nmt::Taskbar();

	gtk_container_add(GTK_CONTAINER(xfPlugin), GTK_WIDGET(mTaskbar->gobj()));

	show();
}

void nmt::Plugin::init()
{
	XDG::init();

	Glib::ustring data = "button.active {background-color:rgba(40,70,100,0.7); border-bottom:1px solid lightgrey;}" "button {background:none;}";
	Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();
	if(!cssProvider->load_from_data(data)) {
		std::cerr << "Failed to load css !!\n";
	}
	Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();
	Gtk::StyleContext::add_provider_for_screen(screen, cssProvider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	//TODO orientation, settings, ...
}



//----------------------------------------------------------------------------------------------------------------------



extern "C" void construct(XfcePanelPlugin* xfPlugin)
{
	Gtk::Main::init_gtkmm_internals();

	xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

	new nmt::Plugin(xfPlugin);
}