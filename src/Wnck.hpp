// ** opensource.org/licenses/GPL-3.0

#ifndef WNCK_HPP
#define WNCK_HPP

#include <map>
#include <fcntl.h>

#include <libwnck/libwnck.h>

#include "GroupWindow.hpp"
#include "Store.tpp"
#include "Helpers.hpp"

class GroupWindow;

namespace Wnck
{
		void init();

		gulong getActiveWindowXID();

		std::string getName(GroupWindow* groupWindow);
		gushort getState(GroupWindow* groupWindow);
		std::string getGroupName(GroupWindow* groupWindow);
		GtkWidget* getActionMenu(GroupWindow* groupWindow);

		void activate(GroupWindow* groupWindow, guint32 timestamp);
		void minimize(GroupWindow* groupWindow);

		extern WnckScreen* mWnckScreen;
		extern Store::KeyStore<gulong, GroupWindow*> mGroupWindows;
}


#endif