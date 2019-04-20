// ** opensource.org/licenses/GPL-3.0

#include "Wnck.hpp"

#define RETURN_IF(b) if(b)return;

namespace Wnck
{
	WnckScreen* mWnckScreen;
	Store::KeyStore<gulong, GroupWindow*> mGroupWindows;

	namespace //private:
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
				::close(fd);

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

	void init()
	{
		mWnckScreen = wnck_screen_get_default();
		wnck_screen_force_update(mWnckScreen);

		//signal connection
		g_signal_connect(G_OBJECT(mWnckScreen), "window-opened",
		G_CALLBACK(+[](WnckScreen* screen, WnckWindow* wnckWindow)
		{
			mGroupWindows.push(wnck_window_get_xid(wnckWindow), new GroupWindow(wnckWindow));
		}), NULL);

		g_signal_connect(G_OBJECT(mWnckScreen), "window-closed",
		G_CALLBACK(+[](WnckScreen* screen, WnckWindow* wnckWindow)
		{
			std::cout << "DELETE:" << 1 << std::endl;
			GroupWindow* groupWindow = mGroupWindows.pop(wnck_window_get_xid(wnckWindow));
			delete groupWindow;
		}), NULL);

		g_signal_connect(G_OBJECT(mWnckScreen), "active-window-changed",
		G_CALLBACK(+[](WnckScreen* screen, WnckWindow* previousActiveWindow)
		{ 
			gulong activeXID = getActiveWindowXID();
			if(activeXID != NULL)
			{
				mGroupWindows.first()->onUnactivate();
				mGroupWindows.moveToStart(activeXID)->onActivate();
			}
		}), NULL);

		//already oppened windows
		for (GList* window_l = wnck_screen_get_windows(wnck_screen_get_default()); window_l != NULL; window_l = window_l->next)
		{
			WnckWindow* wnckWindow = WNCK_WINDOW(window_l->data);
			mGroupWindows.push(wnck_window_get_xid(wnckWindow), new GroupWindow(wnckWindow));
		}
	}

	gulong getActiveWindowXID()
	{
		WnckWindow* activeWindow = wnck_screen_get_active_window(wnck_screen_get_default());
		if(!WNCK_IS_WINDOW(activeWindow)) return NULL;

		return wnck_window_get_xid(activeWindow);
	}

	std::string getName(GroupWindow* groupWindow)
	{
		return wnck_window_get_name(groupWindow->mWnckWindow);
	}

	gushort getState(GroupWindow* groupWindow)
	{
		return wnck_window_get_state(groupWindow->mWnckWindow);
	}

	GdkPixbuf* getMiniIcon(GroupWindow* groupWindow)
	{
		return wnck_window_get_mini_icon(groupWindow->mWnckWindow);
	}

	void activate(GroupWindow* groupWindow, guint32 timestamp)
	{
		wnck_window_activate(groupWindow->mWnckWindow, timestamp);
	}

	void close(GroupWindow* groupWindow, guint32 timestamp)
	{
		wnck_window_close(groupWindow->mWnckWindow, 0);
	}

	void minimize(GroupWindow* groupWindow)
	{
		wnck_window_minimize(groupWindow->mWnckWindow);
	}

	std::string getGroupName(GroupWindow* groupWindow)
	{
		std::string groupName = Help::String::toLowercase(getGroupNameSys(groupWindow->mWnckWindow));
		groupNameTransform(groupName, groupWindow->mWnckWindow);

		return groupName;
	}

	GtkWidget* getActionMenu(GroupWindow* groupWindow)
	{
		return wnck_action_menu_new(groupWindow->mWnckWindow);
	}
}
