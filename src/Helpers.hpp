// ** opensource.org/licenses/GPL-3.0

#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <iostream>
#include <string>
#include <list>

#include <algorithm>
#include <functional>
#include <sstream>
#include <cstring>

#include <gtk/gtk.h>

namespace Help
{
	namespace String
	{
		void split(const std::string& str, std::list<std::string>& list, char delim);
		std::string toLowercase(std::string str);
		std::string getWord(std::string str, int index, char separator = ' ');
		std::string pathBasename(const std::string str);
		std::string trim(const std::string str);
	}

	namespace Gtk
	{
		int getChildPosition(GtkContainer* container, GtkWidget* child);

		void cssClassAdd(GtkWidget* widget, char* className);
		void cssClassRemove(GtkWidget* widget, char* className);

		class Timeout
		{
			public: 
				Timeout();

				void setup(uint ms, std::function<bool()> function);

				void start();
				void stop();

			private:
				uint mDuration;
				std::function<bool()> mFunction;

				uint mTimeoutId;
		};
	}
}

#endif