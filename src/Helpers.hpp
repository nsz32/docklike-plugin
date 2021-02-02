/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <gtk/gtk.h>

#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

namespace Help
{
	namespace String
	{
		void split(const std::string& str, std::list<std::string>& list, char delim);
		std::string toLowercase(std::string str);
		std::string numericOnly(std::string str);
		std::string getWord(std::string str, int index, char separator = ' ');
		std::string pathBasename(const std::string str);
		std::string trim(const std::string str);
	} // namespace String

	namespace Gtk
	{
		std::list<std::string> bufferToStdStringList(gchar** stringList);
		std::vector<char*> stdToBufferStringList(std::list<std::string>& stringList);

		int getChildPosition(GtkContainer* container, GtkWidget* child);

		void cssClassAdd(GtkWidget* widget, const char* className);
		void cssClassRemove(GtkWidget* widget, const char* className);

		class Timeout
		{
		  public:
			Timeout();

			void setup(uint ms, std::function<bool()> function);

			void start();
			void stop();

			uint mDuration;
			std::function<bool()> mFunction;

			uint mTimeoutId;
		};
	} // namespace Gtk
} // namespace Help

#endif