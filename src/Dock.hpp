// ** opensource.org/licenses/GPL-3.0

#ifndef TASKBAR_HPP
#define TASKBAR_HPP

extern "C"
{
	#include <libxfce4panel/libxfce4panel.h>
	#include <libxfce4ui/libxfce4ui.h>
}

#include <iostream>

#include <string>

#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include "Plugin.hpp"
#include "Store.tpp"
#include "GroupWindow.hpp"
#include "Group.hpp"
#include "Wnck.hpp"
#include "Helpers.hpp"

class DockButton;
class Group;

namespace Dock
{
	void init(XfcePanelPlugin* xfPlugin);
	
	std::vector<std::string> getPinnedList();
	void moveButton(DockButton* moving, DockButton* dest);
	void savePinned();

	void onPanelResize(int size);

	extern GtkWidget* mBox;
	extern WnckScreen* mWnckScreen;
	extern Store::KeyStore<std::string, Group*> mGroups;

	extern int mPanelSize;
	extern int mIconSize;
}

#endif