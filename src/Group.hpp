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
		Group(AppInfo* appInfo, bool pinned);

		void updateStyle();
		void electNewTopWindow();

		int hasVisibleWindows();

		void onWindowActivate(GroupWindow* groupWindow);
		void onWindowUnactivate();

		void onButtonPress(GdkEventButton* event);
		void onButtonRelease(GdkEventButton* event);
		void onScroll(GdkEventScroll* scroll_event);
		bool onMouseEnter();
		

		void onDragBegin(GdkDragContext* context);

		AppInfo* mAppInfo;
		Store::List<GroupWindow*> mWindows;
		uint mTopWindowIndex;


	private:
		void setTopWindow(GroupWindow* groupWindow);

		bool mActive;
		bool mDropHover;

};

#endif