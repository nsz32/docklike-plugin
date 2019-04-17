// ** opensource.org/licenses/GPL-3.0

#ifndef GROUPWINDOW_HPP
#define GROUPWINDOW_HPP

#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include "Helpers.hpp"
#include "Wnck.hpp"
#include "Group.hpp"
#include "DockButtonMenuItem.hpp"

#include <iostream>
	
	class Group;

	class GroupWindow
	{
		public:
			GroupWindow(WnckWindow* wnckWindow, Group* group);

			bool getState(WnckWindowState flagMask);
			gulong getXID();

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