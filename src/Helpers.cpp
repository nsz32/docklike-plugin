// ** opensource.org/licenses/GPL-3.0

#include "Helpers.hpp"

namespace Help
{
	namespace String
	{
		void split(const std::string& str, std::list<std::string>& list, char delim = ' ')
		{
			std::stringstream ss(str);
			std::string token;
			while (std::getline(ss, token, delim)) {
				list.push_back(token);
			}
		}

		std::string toLowercase(std::string str)
		{
			std::for_each(str.begin(), str.end(), [](char &c){ c = std::tolower(static_cast<unsigned char>(c)); });
			return str;
		}

		std::string getWord(std::string str, int index, char separator)
		{
			if(index == std::string::npos)
			{
				std::string::iterator it = --str.end();
				while(it != str.begin() && *it == separator) --it;

				std::string::iterator end = it+1;

				while(it != str.begin() && *it != separator) --it;

				if(*it == separator) ++it;

				return std::string(it, end);
			}
			
			std::string::iterator it = str.begin();
			while(it != str.end() && *it == separator) ++it;

			while(index > 0) {
				--index;
				while(it != str.end() && *it != separator) ++it;
				while(it != str.end() && *it == separator) ++it;
			}
			if(it == str.end()) return "";

			std::string::iterator start = it;

			while(it != str.end() && *it != separator) ++it;

			return std::string(start, it);
		}

		std::string pathBasename(const std::string str)
		{
			char buffer[512]; strcpy(buffer, str.c_str());
			return basename(buffer);
		}

		std::string trim(const std::string str)
		{
			std::string::const_iterator s = str.begin();
			std::string::const_iterator e = str.end();

			while(s != e && (*s == ' ' || *s == '\t' || *s == '"')) ++s;
			if(e != s) --e;
			while(e != s && (*e == ' ' || *e == '\t' || *e == '"')) --e;

			return std::string(s, e+1);
		}
	}

	namespace Gtk
	{
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
			mTimeoutId = NULL;
		}

		void Timeout::start(uint time, GSourceFunc function, void* data)
		{
			mFunction = function;
			mData = data;

			if(mTimeoutId != NULL) stop();

			mTimeoutId = g_timeout_add(time, G_SOURCE_FUNC(+[](Timeout* me){
				me->mFunction(me->mData);
				me->mTimeoutId = NULL;
				return false;
			}), this);
		}

		void Timeout::stop()
		{
			if(mTimeoutId != NULL)
			{
				g_source_remove(mTimeoutId);
				mTimeoutId = NULL;
			}
		}
	}


}
