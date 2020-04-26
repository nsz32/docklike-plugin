// ** opensource.org/licenses/GPL-3.0

#ifndef WNCK_HPP
#define WNCK_HPP

#include <fcntl.h>
#include <libwnck/libwnck.h>

#include <map>

#include "Group.hpp"
#include "GroupWindow.hpp"
#include "Helpers.hpp"
#include "Store.tpp"

class GroupWindow;

namespace Wnck
{
	void init();

	gulong getActiveWindowXID();

	std::string getName(GroupWindow* groupWindow);
	std::string getGroupName(GroupWindow* groupWindow);
	gushort getState(GroupWindow* groupWindow);
	GdkPixbuf* getMiniIcon(GroupWindow* groupWindow);

	GtkWidget* buildActionMenu(GroupWindow* groupWindow, Group* group);

	void close(GroupWindow* groupWindow, guint32 timestamp);
	void activate(GroupWindow* groupWindow, guint32 timestamp);
	void minimize(GroupWindow* groupWindow);

	void setActiveWindow();

	extern WnckScreen* mWnckScreen;
	extern Store::KeyStore<gulong, GroupWindow*> mGroupWindows;
} // namespace Wnck

#endif //WNCK_HPP