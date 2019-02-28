// ** opensource.org/licenses/GPL-3.0

#ifndef TASKBAR_HPP
#define TASKBAR_HPP

#include <iostream>

#include <string>

#include <gtkmm.h>
#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include "Store.tpp"
#include "Window.hpp" //go to cpp / rm class decl \/
#include "Group.hpp"
#include "Utility.hpp"


namespace nmt {

	class Window;
	class Group;

	class Taskbar: public Gtk::Box
	{
		public:
			Taskbar();
			~Taskbar();
			
		private:
			void init();

			void onWnckWindowOpened(WnckWindow* wnckWindow);
			void onWnckWindowClosed(WnckWindow* wnckWindow);
			void onWnckWindowActivate(WnckWindow* wnckWindow);

			WnckScreen* mWnckScreen;

			KeyStore<std::string, Group*> mGroups;
	};
}

#endif