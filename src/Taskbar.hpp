// ** opensource.org/licenses/GPL-3.0

#ifndef TASKBAR_HPP
#define TASKBAR_HPP

#include <iostream>

#include <string>

#include <gtkmm.h>
#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include "Plugin.hpp"
#include "Store.tpp"
#include "GroupWindow.hpp"
#include "Group.hpp"
#include "Wnck.hpp"
#include "Helpers.hpp"

class Group;

namespace Taskbar
	{
			void init();
			
			std::vector<std::string> getPinnedList();
			void moveButton(Group* moving, Group* dest);

			extern GtkWidget* mBoxWidget;
			extern WnckScreen* mWnckScreen;
			extern Store::KeyStore<std::string, Group*> mGroups;
	}

#endif