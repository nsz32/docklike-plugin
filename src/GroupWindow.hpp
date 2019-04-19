// ** opensource.org/licenses/GPL-3.0

#ifndef GROUPWINDOW_HPP
#define GROUPWINDOW_HPP

#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include "Helpers.hpp"
#include "AppInfos.hpp"
#include "Wnck.hpp"
#include "Group.hpp"
#include "DockButtonMenuItem.hpp"

#include <iostream>
	
	class Group;

	class GroupWindow
	{
		public:
			GroupWindow(WnckWindow* wnckWindow);
			~GroupWindow();

			void getInGroup(Group* group);
			void leaveGroup(Group* group);

			void onActivate();
			void onUnactivate();

			bool getState(WnckWindowState flagMask);

			void activate(guint32 timestamp);
			void minimize();
			void showMenu();

			Group* mGroup;

			WnckWindow* mWnckWindow;
			DockButtonMenuItem mDockButtonMenuItem;

			void updateState(unsigned short state, unsigned short changeMask = USHRT_MAX);
			unsigned short mState;
	};

#endif