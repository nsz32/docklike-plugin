// ** opensource.org/licenses/GPL-3.0

#ifndef GROUPWINDOW_HPP
#define GROUPWINDOW_HPP

#include <gtkmm.h>
#include <libwnck/libwnck.h>

#include "Helpers.hpp"
#include "Wnck.hpp"
#include "Group.hpp"

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

			WnckWindow* mWnckWindow;
			
		private:
			void init();
			void updateState(unsigned short state, unsigned short changeMask = USHRT_MAX);

			Group* mGroup;

			unsigned short mState;
	};

#endif