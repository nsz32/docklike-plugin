/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include "AppInfos.hpp"

void AppInfo::launch()
{
	GDesktopAppInfo* info = g_desktop_app_info_new_from_filename(this->path.c_str());

	g_app_info_launch((GAppInfo*)info, NULL, NULL, NULL);
}

void AppInfo::launch_action(const gchar* action)
{
	GDesktopAppInfo* info = g_desktop_app_info_new_from_filename(this->path.c_str());

	g_desktop_app_info_launch_action(info, action, NULL);
}

namespace AppInfos
{
	std::list<std::string> mXdgDataDirs;
	Store::Map<const std::string, AppInfo*> mAppInfoWMClasses;
	Store::Map<const std::string, AppInfo*> mAppInfoIds;
	Store::Map<const std::string, AppInfo*> mAppInfoNames;

	pthread_mutex_t AppInfosLock;

	std::string parseId(const std::string id)
	{
		return id.substr(0, id.size() - 8);
	}

	void findXDGDirectories()
	{
		char* var = getenv("XDG_DATA_DIRS");
		if (var != NULL && var[0] != '\0')
		{
			Help::String::split(var, mXdgDataDirs, ':');
		}

		mXdgDataDirs.push_back("/usr/local/share");
		mXdgDataDirs.push_back("/usr/share");

		for (std::string& dirs : mXdgDataDirs)
		{
			dirs += "/applications/";
		}

		mXdgDataDirs.sort();
		mXdgDataDirs.unique();

		mXdgDataDirs.push_back(std::string(getenv("HOME")) + "/.local/share/applications/");
	}

	void loadDesktopEntry(const std::string& xdgDir, std::string filename)
	{
		if (filename.size() < 9 || filename.substr(filename.size() - 8, std::string::npos) != ".desktop")
			return;

		std::string id = filename.substr(0, filename.size() - 8);
		std::string path = xdgDir + id + ".desktop";

		GDesktopAppInfo* gAppInfo = g_desktop_app_info_new_from_filename(path.c_str());

		if (gAppInfo == NULL)
			return;

		pthread_mutex_lock(&AppInfosLock);

		std::string icon;
		char* icon_ = g_desktop_app_info_get_string(gAppInfo, "Icon");
		if (icon_ == NULL)
		{
			pthread_mutex_unlock(&AppInfosLock);
			return;
		}

		icon = Help::String::trim(icon_);

		std::string name;
		char* name_ = g_desktop_app_info_get_string(gAppInfo, "Name");
		if (name_ != NULL)
			name = name_;

		const gchar* const* actions = g_desktop_app_info_list_actions(gAppInfo);
		AppInfo* info = new AppInfo({path, icon, name, actions});

		id = Help::String::toLowercase(id);
		mAppInfoIds.set(id, info);
		mAppInfoIds.set(path, info); // for saved pinned groups

		if (!name.empty())
		{
			name = Help::String::toLowercase(Help::String::trim(name));
			if (name.find(' ') == std::string::npos)
			{
				if (name != id)
					mAppInfoNames.set(name, info);
			}
		}

		std::string exec;
		char* exec_ = g_desktop_app_info_get_string(gAppInfo, "Exec");
		if (exec_ != NULL && exec_[0] != '\0')
		{
			std::string execLine = Help::String::toLowercase(Help::String::pathBasename(Help::String::trim(exec_)));

			exec = Help::String::getWord(execLine, 0);
			if (exec != "env" && exec != "exo-open")
			{
				if (exec != id && exec != name)
					mAppInfoNames.set(Help::String::toLowercase(exec), info);
			}
		}

		std::string wmclass;
		char* wmclass_ = g_desktop_app_info_get_string(gAppInfo, "StartupWMClass");
		if (wmclass_ != NULL && wmclass_[0] != '\0')
		{
			wmclass = Help::String::toLowercase(Help::String::trim(wmclass_));

			if (wmclass != id && wmclass != name && wmclass != exec)
				mAppInfoWMClasses.set(wmclass, info);
		}

		pthread_mutex_unlock(&AppInfosLock);
	}

	void* threadedXDGDirectoryWatcher(void* dirPath)
	{
		std::cout << "watch XDG dir: " << *(std::string*)dirPath << std::endl;

		int fd = inotify_init();
		int wd = inotify_add_watch(fd, ((std::string*)dirPath)->c_str(), IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE);

		int i = 0, len = 0;
		char buf[1024];
		struct inotify_event* event;

		while (true)
		{
			i = 0;
			len = read(fd, buf, 1024);

			while (i < len)
			{
				event = (struct inotify_event*)&buf[i];
				std::cout << "new file detected:" << event->name << std::endl;

				loadDesktopEntry(*(std::string*)dirPath, event->name);

				i += sizeof(struct inotify_event) + event->len;
			}
		}
	}

	void watchXDGDirectory(std::string xdgDir)
	{
		pthread_t thread_store;
		std::string* arg = new std::string(xdgDir);

		pthread_create(&thread_store, NULL, threadedXDGDirectoryWatcher, arg);
	}

	void loadXDGDirectories()
	{
		for (std::string xdgDir : mXdgDataDirs)
		{
			std::cout << "XDGDirectories:" << xdgDir << std::endl;

			DIR* directory = opendir(xdgDir.c_str());
			if (directory == NULL)
				continue;

			struct dirent* entry;
			std::list<std::string> matches;
			while ((entry = readdir(directory)) != NULL)
			{
				std::string filename = entry->d_name;

				loadDesktopEntry(xdgDir, filename);
			}

			watchXDGDirectory(xdgDir);
		}
	}

	void init()
	{
		pthread_mutex_init(&AppInfosLock, NULL);

		findXDGDirectories();
		loadXDGDirectories();
	}

	// ADDIT GroupName aliases
	std::map<std::string, std::string> mGroupNameRename = {
		{"soffice", "libreoffice-startcenter"},
		{"libreoffice", "libreoffice-startcenter"},
		{"radium_linux.bin", "radium"},
	};

	void groupNameTransform(std::string& groupName)
	{
		// Rename from table
		std::map<std::string, std::string>::iterator itRenamed;
		if ((itRenamed = mGroupNameRename.find(groupName)) != mGroupNameRename.end())
			groupName = itRenamed->second;
	}

	AppInfo* search(std::string id)
	{
		groupNameTransform(id);

		AppInfo* ai = mAppInfoWMClasses.get(id);
		if (ai != NULL)
			return ai;

		ai = mAppInfoIds.get(id);
		if (ai != NULL)
			return ai;

		uint pos = id.find(' ');
		if (pos != std::string::npos)
		{
			id = id.substr(0, pos);

			ai = mAppInfoIds.get(id);
			if (ai != NULL)
				return ai;
		}

		ai = mAppInfoNames.get(id);
		if (ai != NULL)
			return ai;

		pos = id.find(' ');
		if (pos != std::string::npos)
		{
			id = id.substr(0, pos);

			ai = mAppInfoNames.get(id);
			if (ai != NULL)
				return ai;
		}

		gchar*** gioPath = g_desktop_app_info_search(id.c_str());

		if (gioPath != NULL && gioPath[0] != NULL && gioPath[0][0] != NULL && gioPath[0][0][0] != '\0')
		{
			std::string gioId = gioPath[0][0];
			gioId = Help::String::toLowercase(gioId.substr(0, gioId.size() - 8));

			ai = mAppInfoIds.get(gioId);

			for (int i = 0; gioPath[i] != NULL; ++i)
				g_strfreev(gioPath[i]);
			g_free(gioPath);

			if (ai != NULL)
				return ai;
		}

		return new AppInfo({"", "", id});
	}

} // namespace AppInfos