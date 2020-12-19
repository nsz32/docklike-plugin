/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include "Helpers.hpp"

namespace Help
{
	namespace String
	{
		void split(const std::string& str, std::list<std::string>& list,
			char delim = ' ')
		{
			std::stringstream ss(str);
			std::string token;
			while (std::getline(ss, token, delim))
			{
				list.push_back(token);
			}
		}

		std::string toLowercase(std::string str)
		{
			std::for_each(str.begin(), str.end(), [](char& c) {
				c = std::tolower(static_cast<unsigned char>(c));
			});
			return str;
		}

		std::string numericOnly(std::string str)
		{
			str.erase(
				std::remove_if(str.begin(), str.end(), [](char chr) { return chr < 48 || chr > 57; }),
				str.end());

			return str;
		}

		std::string getWord(std::string str, int index, char separator)
		{
			if (index == std::string::npos)
			{
				std::string::iterator it = --str.end();
				while (it != str.begin() && *it == separator)
					--it;

				std::string::iterator end = it + 1;

				while (it != str.begin() && *it != separator)
					--it;

				if (*it == separator)
					++it;

				return std::string(it, end);
			}

			std::string::iterator it = str.begin();
			while (it != str.end() && *it == separator)
				++it;

			while (index > 0)
			{
				--index;
				while (it != str.end() && *it != separator)
					++it;
				while (it != str.end() && *it == separator)
					++it;
			}
			if (it == str.end())
				return "";

			std::string::iterator start = it;

			while (it != str.end() && *it != separator)
				++it;

			return std::string(start, it);
		}

		std::string pathBasename(const std::string str)
		{
			char buffer[1024];
			strcpy(buffer, str.c_str());
			return basename(buffer);
		}

		std::string trim(const std::string str)
		{
			std::string::const_iterator s = str.begin();
			std::string::const_iterator e = str.end();

			while (s != e && (*s == ' ' || *s == '\t' || *s == '"'))
				++s;
			if (e != s)
				--e;
			while (e != s && (*e == ' ' || *e == '\t' || *e == '"'))
				--e;

			return std::string(s, e + 1);
		}
	} // namespace String

	namespace Gtk
	{
		std::list<std::string> bufferToStdStringList(gchar** stringList)
		{
			std::list<std::string> ret;

			if (stringList != NULL)
				for (int i = 0; stringList[i] != NULL; ++i)
					ret.push_back(stringList[i]);

			return ret;
		}

		std::vector<char*> stdToBufferStringList(std::list<std::string>& stringList)
		{
			std::vector<char*> buf;

			for (std::string& s : stringList)
				buf.push_back(&s[0]);

			return buf;
		}

		int getChildPosition(GtkContainer* container, GtkWidget* child)
		{
			GValue gv = G_VALUE_INIT;
			g_value_init(&gv, G_TYPE_INT);

			gtk_container_child_get_property(container, child, "position", &gv);

			return g_value_get_int(&gv);
		}

		void cssClassAdd(GtkWidget* widget, char* className)
		{
			gtk_style_context_add_class(gtk_widget_get_style_context(widget), className);
		}

		void cssClassRemove(GtkWidget* widget, char* className)
		{
			gtk_style_context_remove_class(gtk_widget_get_style_context(widget), className);
		}

		Timeout::Timeout()
		{
			mDuration = mTimeoutId = NULL;
			// mFunction = NULL;
		}

		void Timeout::setup(uint ms, std::function<bool()> function)
		{
			mDuration = ms;
			mFunction = function;
		}

		void Timeout::start()
		{
			stop();
			mTimeoutId = g_timeout_add(mDuration, G_SOURCE_FUNC(+[](Timeout* me) {
				bool cont = me->mFunction();

				if (!cont)
					me->mTimeoutId = NULL;
				return cont;
			}),
				this);
		}

		void Timeout::stop()
		{
			if (mTimeoutId != NULL)
			{
				g_source_remove(mTimeoutId);
				mTimeoutId = NULL;
			}
		}
	} // namespace Gtk

} // namespace Help
