// ** opensource.org/licenses/GPL-3.0

#ifndef TASKBAR_HPP
#define TASKBAR_HPP

extern "C"
{
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4ui/libxfce4ui.h>
}

#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include <iostream>
#include <string>

#include "GroupWindow.hpp"
#include "Helpers.hpp"
#include "Plugin.hpp"
#include "Store.tpp"
#include "Wnck.hpp"

class Group;

namespace Dock
{
	void init();

	Group* prepareGroup(AppInfo* appInfo);

	void moveButton(Group* moving, Group* dest);
	void savePinned();

	void onPanelResize(int size);
	void onPanelOrientationChange(GtkOrientation orientation);

	extern GtkWidget* mBox;
	extern Store::KeyStore<AppInfo*, Group*> mGroups;

	extern int mPanelSize;
	extern int mIconSize;
} // namespace Dock

#endif