// ** opensource.org/licenses/GPL-3.0

#ifndef GROUP_HPP
#define GROUP_HPP

#include <iostream>
#include <vector>

#include <gtk/gtk.h>
#include <cairo/cairo.h>
#include <libwnck/libwnck.h>

#include "Store.tpp"

#include "DockButton.hpp"
#include "Helpers.hpp"
#include "AppInfos.hpp"
#include "Dock.hpp"
#include "GroupWindow.hpp"


class GroupWindow;

class Group: public DockButton
{
	public:
		Group(std::string groupName, AppInfo* appInfo, bool pinned);

		void addWindow(GroupWindow* window);
		void removeWindow(gulong XID);

		void updateStyle();

		int hasVisibleWindows();

		void onWindowActivate(gulong XID);
		void onWindowUnactivate();

		bool onButtonPress(GdkEventButton* event);
		bool onButtonRelease(GdkEventButton* event);
		bool onScroll(GdkEventScroll* scroll_event);
		bool onMouseEnter();
		

		void onDragBegin(GdkDragContext* context);

		std::string mGroupName;
		bool mPinned;
		AppInfo* mAppInfo;


	private:

		bool mActive;
		bool mDropHover;
		Store::KeyStore<gulong, GroupWindow*> mWindows;

};

#endif