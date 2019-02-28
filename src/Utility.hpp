#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <list>

#include <libwnck/libwnck.h>
#include <libgen.h>

#include <iostream>
#include <algorithm>
#include <sstream>

namespace nmt::Utils
{
	namespace String {
		void split(const std::string& str, std::list<std::string>& list, char delim);
		std::string toLower(std::string str);

	}

	namespace Wnck {
		WnckScreen* getScreen();
		GList* getWindowsList();
		WnckWindow* getActiveWindow();
		std::string getGroupName(WnckWindow* wnckWindow);
		std::string getName(WnckWindow* wnckWindow);
		gulong getXID(WnckWindow* wnckWindow);
		gushort getState(WnckWindow* wnckWindow);

		void activate(WnckWindow* wnckWindow, guint32 timestamp);
		void minimize(WnckWindow* wnckWindow);
	}
}

#endif