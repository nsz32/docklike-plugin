#include "Utility.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace nmt::Utils
{

	namespace String {
		void split(const std::string& str, std::list<std::string>& list, char delim = ' ')
		{
			std::stringstream ss(str);
			std::string token;
			while (std::getline(ss, token, delim)) {
				list.push_back(token);
			}
		}

		std::string toLower(std::string str)
		{
			std::for_each(str.begin(), str.end(), [](char &c){ c = std::tolower(static_cast<unsigned char>(c)); });
			return str;
		}
	}

	namespace Wnck {
		WnckScreen* getScreen()
		{
			WnckScreen* wnckScreen = wnck_screen_get_default();
			wnck_screen_force_update (wnckScreen);
			return wnckScreen;
		}

		GList* getWindowsList()
		{
			return wnck_screen_get_windows(wnck_screen_get_default());
		}

		WnckWindow* getActiveWindow()
		{
			return wnck_screen_get_active_window(wnck_screen_get_default());
		}

		std::string getGroupName(WnckWindow* wnckWindow)
		{
			//Wnck method
			const char* buf = wnck_window_get_class_instance_name(wnckWindow);
			if(buf != NULL && buf[0] != '\0')
			{
				return buf;
			}

			//proc/{pid}/cmdline method

			char buffer[512];
			std::string path = "/proc/" + std::to_string(wnck_window_get_pid(wnckWindow)) + "/cmdline";
			int fd = open(path.c_str(), O_RDONLY);
			if(fd >= 0)
			{
				int nbr = read(fd, buffer, 512);
				close(fd);

				char* exe = basename(buffer);


				if(strcmp(exe, "python")!=0) //ADDIT graphical interpreters
					return exe;

				char* it = buffer;
				while(*it++);

				if(it < buffer+nbr)
					return basename(it);
			}
			
			//fallback : return window's name
			return wnck_window_get_name(wnckWindow);
		}

		std::string getName(WnckWindow* wnckWindow)
		{
			return wnck_window_get_name(wnckWindow);
		}

		gulong getXID(WnckWindow* wnckWindow)
		{
			return wnck_window_get_xid(wnckWindow);
		}

		gushort getState(WnckWindow* wnckWindow)
		{
			return wnck_window_get_state(wnckWindow);
		}


		void activate(WnckWindow* wnckWindow, guint32 timestamp)
		{
			wnck_window_activate(wnckWindow, timestamp);
		}

		void minimize(WnckWindow* wnckWindow)
		{
			wnck_window_minimize(wnckWindow);
		}
	}

}
