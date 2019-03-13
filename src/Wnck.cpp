// ** opensource.org/licenses/GPL-3.0

#include "Wnck.hpp"

namespace Wnck
{
	namespace
	{
		std::map<std::string, std::string> mGroupNameRename //ADDIT GroupName aliases
			= {
				{"soffice", "libreoffice"},
				{"radium_linux.bin", "radium"},
			}; 

		void groupNameTransform(std::string& groupName, WnckWindow* wnckWindow)
		{
			//Rename from table
			std::map<std::string, std::string>::iterator itRenamed;
			if((itRenamed = mGroupNameRename.find(groupName)) != mGroupNameRename.end())
				groupName = itRenamed->second;

			//LibreOffice <- needs window name tracking
			/*BAD if(groupName == "libreoffice")
			{
				std::string winName = getName(wnckWindow);
				std::cout << "NAME:" << winName << std::endl;
				if(!winName.empty())
				{
					std::string name = Help::String::toLowercase(Help::String::getLastWord(winName));
					if(name == "calc" || name == "draw" || name == "impress" || name == "math")
						groupName = "libreoffice-" + name;
					else
						groupName = "libreoffice-writer";

					return;
				}
			}*/
		}

		std::string getGroupNameSys(WnckWindow* wnckWindow)
		{
			//Wnck method const char *
			const char* buf = wnck_window_get_class_group_name(wnckWindow);
			if(buf != NULL && buf[0] != '\0')
				return buf;
			buf = wnck_window_get_class_instance_name(wnckWindow);
			if(buf != NULL && buf[0] != '\0')
				return buf;

			//proc/{pid}/cmdline method
			char buffer[512];
			std::string path = "/proc/" + std::to_string(wnck_window_get_pid(wnckWindow)) + "/cmdline";
			int fd = open(path.c_str(), O_RDONLY);
			if(fd >= 0)
			{
				int nbr = read(fd, buffer, 512);
				close(fd);

				char* exe = basename(buffer);

				if(strcmp(exe, "python")!=0) //ADDIT graphical interpreters here
					return exe;

				char* it = buffer;
				while(*it++);

				if(it < buffer+nbr)
					return basename(it);
			}
			
			//fallback : return window's name
			return wnck_window_get_name(wnckWindow);
		}
	}


	//public:

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

	std::string getGroupName(WnckWindow* wnckWindow)
	{
		std::string groupName = Help::String::toLowercase(getGroupNameSys(wnckWindow));

		groupNameTransform(groupName, wnckWindow);

		return groupName;
	}

	GtkWidget* getActionMenu(WnckWindow* wnckWindow)
	{
		return wnck_action_menu_new(wnckWindow);
	}
}
