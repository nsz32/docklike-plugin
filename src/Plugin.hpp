// ** opensource.org/licenses/GPL-3.0

#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include <string>

#include <gtkmm.h>
#include <libwnck/libwnck.h>

extern "C"
{
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4ui/libxfce4ui.h>
}

#include "Taskbar.hpp"

namespace nmt {

	class Plugin: public Gtk::Box
	{
		public:
			Plugin(XfcePanelPlugin* xfPlugin);

		private:
			void init();

			Taskbar* mTaskbar;
	};

}

#endif