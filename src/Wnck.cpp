/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include "Wnck.hpp"

#define RETURN_IF(b) \
	if (b)           \
		return;

namespace Wnck
{
	WnckScreen* mWnckScreen;
	Store::KeyStore<gulong, GroupWindow*> mGroupWindows;

	namespace // private:
	{
		// ADDIT GroupName aliases
		std::map<std::string, std::string> mGroupNameRename = {
			{"soffice", "libreoffice"},
			{"radium_linux.bin", "radium"},
		};

		void groupNameTransform(std::string& groupName, WnckWindow* wnckWindow)
		{
			// Rename from table
			std::map<std::string, std::string>::iterator itRenamed;
			if ((itRenamed = mGroupNameRename.find(groupName)) != mGroupNameRename.end())
				groupName = itRenamed->second;

			// LibreOffice <- needs window name tracking
			/*BAD if(groupName == "libreoffice")
			{
					std::string winName = getName(wnckWindow);
					std::cout << "NAME:" << winName << std::endl;
					if(!winName.empty())
					{
							std::string name =
			Help::String::toLowercase(Help::String::getLastWord(winName)); if(name ==
			"calc" || name == "draw" || name == "impress" || name == "math") groupName
			= "libreoffice-" + name; else groupName = "libreoffice-writer";

							return;
					}
			}*/
		}

		std::string getGroupNameSys(WnckWindow* wnckWindow)
		{
			// Wnck method const char *
			const char* buf = wnck_window_get_class_group_name(wnckWindow);
			if (buf != NULL && buf[0] != '\0')
				return buf;
			buf = wnck_window_get_class_instance_name(wnckWindow);
			if (buf != NULL && buf[0] != '\0')
				return buf;

			// proc/{pid}/cmdline method
			char buffer[512];
			std::string path = "/proc/" + std::to_string(wnck_window_get_pid(wnckWindow)) + "/cmdline";
			int fd = open(path.c_str(), O_RDONLY);
			if (fd >= 0)
			{
				int nbr = read(fd, buffer, 512);
				::close(fd);

				char* exe = basename(buffer);

				if (strcmp(exe, "python") != 0) // ADDIT graphical interpreters here
					return exe;

				char* it = buffer;
				while (*it++)
					;

				if (it < buffer + nbr)
					return basename(it);
			}

			// fallback : return window's name
			return wnck_window_get_name(wnckWindow);
		}
	} // namespace

	// public:

	void init()
	{
		mWnckScreen = wnck_screen_get_default();
		wnck_screen_force_update(mWnckScreen);

		// signal connection
		g_signal_connect(G_OBJECT(mWnckScreen), "window-opened",
			G_CALLBACK(+[](WnckScreen* screen, WnckWindow* wnckWindow) {
				mGroupWindows.pushSecond(wnck_window_get_xid(wnckWindow), new GroupWindow(wnckWindow));
			}),
			NULL);

		g_signal_connect(G_OBJECT(mWnckScreen), "window-closed",
			G_CALLBACK(+[](WnckScreen* screen, WnckWindow* wnckWindow) {
				GroupWindow* groupWindow = mGroupWindows.pop(wnck_window_get_xid(wnckWindow));
				delete groupWindow;
			}),
			NULL);

		g_signal_connect(G_OBJECT(mWnckScreen), "active-window-changed",
			G_CALLBACK(+[](WnckScreen* screen, WnckWindow* previousActiveWindow) {
				setActiveWindow();
			}),
			NULL);

		// already opened windows
		for (GList* window_l = wnck_screen_get_windows(mWnckScreen);
			 window_l != NULL;
			 window_l = window_l->next)
		{
			WnckWindow* wnckWindow = WNCK_WINDOW(window_l->data);
			mGroupWindows.push(wnck_window_get_xid(wnckWindow),
				new GroupWindow(wnckWindow));
		}
		setActiveWindow();
	}

	gulong getActiveWindowXID()
	{
		WnckWindow* activeWindow = wnck_screen_get_active_window(mWnckScreen);
		if (!WNCK_IS_WINDOW(activeWindow))
			return NULL;

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
		WnckWorkspace* workspace = wnck_window_get_workspace(groupWindow->mWnckWindow);
		if (workspace != NULL)
			wnck_workspace_activate(workspace, timestamp);
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

	void setActiveWindow()
	{
		gulong activeXID = getActiveWindowXID();
		if (activeXID != NULL)
		{
			mGroupWindows.first()->onUnactivate();
			mGroupWindows.moveToStart(activeXID)->onActivate();
		}
	}

	std::string getGroupName(GroupWindow* groupWindow)
	{
		std::string groupName = Help::String::toLowercase(getGroupNameSys(groupWindow->mWnckWindow));
		groupNameTransform(groupName, groupWindow->mWnckWindow);

		return groupName;
	}

	GtkWidget* buildActionMenu(GroupWindow* groupWindow, Group* group)
	{
		GtkWidget* menu = (groupWindow != NULL) ? wnck_action_menu_new(groupWindow->mWnckWindow) : gtk_menu_new();

		AppInfo* appInfo = (groupWindow != NULL) ? groupWindow->mGroup->mAppInfo : group->mAppInfo;

		if (!appInfo->path.empty())
		{
			GtkWidget* launchAnother = gtk_menu_item_new_with_label((groupWindow != NULL) ? "Launch another" : "Launch");

			gtk_widget_show(launchAnother);

			gtk_menu_attach(GTK_MENU(menu), GTK_WIDGET(launchAnother), 0, 2, 0, 1);

			g_signal_connect(G_OBJECT(launchAnother), "activate",
				G_CALLBACK(+[](GtkMenuItem* menuitem, AppInfo* appInfo) {
					AppInfos::launch(appInfo);
				}),
				appInfo);

			if (group != NULL)
			{
				GtkWidget* separator = gtk_separator_menu_item_new();
				GtkWidget* pinToggle = gtk_menu_item_new_with_label(group->mPinned ? "Unpin" : "Pin");

				gtk_widget_show(separator);
				gtk_widget_show(pinToggle);

				gtk_menu_attach(GTK_MENU(menu), GTK_WIDGET(separator), 2, 3, 0, 1);
				gtk_menu_attach(GTK_MENU(menu), GTK_WIDGET(pinToggle), 2, 3, 0, 1);

				g_signal_connect(G_OBJECT(pinToggle), "activate",
					G_CALLBACK(+[](GtkMenuItem* menuitem, Group* group) {
						group->mPinned = !group->mPinned;
						if (!group->mPinned)
							group->updateStyle();
						Dock::savePinned();
					}),
					group);
			}

			if (group != NULL && group->mWindowsCount > 1)
			{
				GtkWidget* separator = gtk_separator_menu_item_new();
				gtk_widget_show(separator);

				GtkWidget* closeAll = gtk_menu_item_new_with_label("Close All");
				gtk_widget_show(closeAll);

				gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);
				gtk_menu_shell_append(GTK_MENU_SHELL(menu), closeAll);

				g_signal_connect(G_OBJECT(closeAll), "activate",
					G_CALLBACK(+[](GtkMenuItem* menuitem, Group* group) {
						group->mWindows.forEach([](GroupWindow* w) -> void {
							Wnck::close(w, 0);
						});
					}),
					group);
			}

			return menu;
		}
	}

	void switchToLastWindow(guint32 timestamp)
	{
		auto it = mGroupWindows.mList.begin();

		while (it != mGroupWindows.mList.end() && it->second->getState(WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST))
			++it; //skip dialogs
		if (it != mGroupWindows.mList.end())
			++it; //skip current window

		while (it != mGroupWindows.mList.end())
		{
			if (!it->second->getState(WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST))
			{
				it->second->activate(timestamp);
				return;
			}
			++it;
		}
	}
} // namespace Wnck
