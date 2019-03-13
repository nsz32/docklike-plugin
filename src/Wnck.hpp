// ** opensource.org/licenses/GPL-3.0

#ifndef WNCK_HPP
#define WNCK_HPP

#include <map>
#include <fcntl.h>

#include <libwnck/libwnck.h>

#include "Helpers.hpp"

namespace Wnck
{

		WnckScreen* getScreen();

		GList* getWindowsList();

		WnckWindow* getActiveWindow();

		std::string getName(WnckWindow* wnckWindow);

		gulong getXID(WnckWindow* wnckWindow);

		gushort getState(WnckWindow* wnckWindow);

		void activate(WnckWindow* wnckWindow, guint32 timestamp);

		void minimize(WnckWindow* wnckWindow);

		std::string getGroupName(WnckWindow* wnckWindow);

		GtkWidget* getActionMenu(WnckWindow* wnckWindow);

}


#endif