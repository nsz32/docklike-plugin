// ** opensource.org/licenses/GPL-3.0

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <gtkmm.h>
#include <libwnck/libwnck.h>

#include "Group.hpp"

namespace nmt {

	class Group;

	class Window
	{
		public:
			Window(WnckWindow* wnckWindow, Group* group);

			bool getState(WnckWindowState flagMask);
			gulong getXID();

			void activate(guint32 timestamp);
			void minimize();

			WnckWindow* mWnckWindow;
			
		private:
			void init();
			void updateState(unsigned short state, unsigned short changeMask = USHRT_MAX);

			Group* mGroup;

			unsigned short mState;
	};

}

#endif