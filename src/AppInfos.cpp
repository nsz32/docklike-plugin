// ** opensource.org/licenses/GPL-3.0

#include "AppInfos.hpp"
#include <gio/gdesktopappinfo.h>

namespace AppInfos
{
	namespace
	{
		std::list<std::string> mXdgDataDirs;
		Store::Map<const std::string, AppInfo*> mAppInfoIds;
		Store::Map<const std::string, AppInfo*> mAppInfoNames;

		std::string parseId(const std::string id)
		{
			return id.substr(0, id.size() - 8);
		}
	}

	void findXDGDirectories()
	{
		//1. User's .local
		char* var = getenv("XDG_DATA_HOME");
		std::string xdgDataHome;
		if(var != NULL && var[0] != '\0')
			mXdgDataDirs.push_back(var);
		else
			mXdgDataDirs.push_back(std::string(getenv("HOME")) + "/.local/share");

		//2. System wide
		var = getenv("XDG_DATA_DIRS");
		if(var != NULL && var[0] != '\0')
		{
			Help::String::split(var, mXdgDataDirs, ':');
		}
		else
		{
			mXdgDataDirs.push_back("/usr/local/share");
			mXdgDataDirs.push_back("/usr/share");
		}

		for(std::string& dirs : mXdgDataDirs)
		{
			dirs += "/applications/";
		}
	}

	void loadDesktopEntry(const std::string& xdgDir, std::string id)
	{
		std::string path = xdgDir + id + ".desktop";

		GDesktopAppInfo* gAppInfo = g_desktop_app_info_new_from_filename(path.c_str());

		if(gAppInfo == NULL) return;

		std::string icon;
		char* icon_ = g_desktop_app_info_get_string(gAppInfo, "Icon");
		if(icon_ != NULL)
			icon = Help::String::trim(icon_);

		std::string name;
		char* name_ = g_desktop_app_info_get_string(gAppInfo, "Name");
		if(name_ != NULL)
			name = name_;

		AppInfo* info = new AppInfo({path, icon, name});

		id = Help::String::toLowercase(id);
		mAppInfoIds.set(id, info);
		mAppInfoIds.set(path, info);

		if(!name.empty()) 
		{
			name = Help::String::toLowercase(Help::String::trim(name));
			if(name.find(' ') == std::string::npos)
			{
				if(name != id)
					mAppInfoNames.set(name, info);
			}
		}
		
		std::string exec;
		char* exec_ = g_desktop_app_info_get_string(gAppInfo, "Exec");
		if(exec_ != NULL && exec_[0] != '\0') 
		{
			std::string execLine = Help::String::toLowercase(Help::String::pathBasename(Help::String::trim(exec_)));

			exec = Help::String::getWord(execLine, 0);
			if(exec != "env" && exec != "exo-open")
			{
				if(exec != id && exec != name)
					mAppInfoNames.set(Help::String::toLowercase(exec), info);
			}
		}

		std::string wmclass;
		char* wmclass_ = g_desktop_app_info_get_string(gAppInfo, "StartupWMClass");
		if(wmclass_ != NULL && wmclass_[0] != '\0')
		{
			wmclass = Help::String::toLowercase(Help::String::trim(wmclass_));

			if(wmclass != id && wmclass != name && wmclass != exec)
				mAppInfoNames.set(wmclass, info);
		}

		/*std::cout << "ID:" << id << std::endl;
		std::cout << "NAME:" << name << std::endl;
		std::cout << "EXEC:" << exec << std::endl;
		std::cout << "WMCLASS:" << wmclass << std::endl;
		std::cout << ":" << std::endl;*/
	}

	void loadXDGDirectories()
	{
		for(std::string xdgDir : mXdgDataDirs)
		{
			DIR* directory = opendir(xdgDir.c_str());
			if(directory == NULL) continue;
			
			struct dirent* entry;
			std::list<std::string> matches;
			while((entry = readdir(directory)) != NULL )
			{
				std::string filename = entry->d_name;

				if(filename.size() < 9 || filename.substr(filename.size() - 8, std::string::npos) != ".desktop")
					continue;

				std::string id = filename.substr(0, filename.size() - 8);

				loadDesktopEntry(xdgDir, id);
			}
		}
	}

	void init()
	{
		findXDGDirectories();
		loadXDGDirectories();
	}

	AppInfo* search(std::string id)
	{
		AppInfo* ai = mAppInfoIds.get(id);
		if(ai != NULL) return ai;

		uint pos = id.find(' ');
		if(pos != std::string::npos)
		{
			id = id.substr(0, pos);

			ai = mAppInfoIds.get(id);
			if(ai != NULL) return ai;
		}

		ai = mAppInfoNames.get(id);
		if(ai != NULL) return ai;

		pos = id.find(' ');
		if(pos != std::string::npos)
		{
			id = id.substr(0, pos);

			ai = mAppInfoNames.get(id);
			if(ai != NULL) return ai;
		}


		gchar*** gioPath = g_desktop_app_info_search(id.c_str());
		
		if(gioPath != NULL && gioPath[0] != NULL && gioPath[0][0] != NULL && gioPath[0][0][0] != '\0')
		{
			std::string gioId = gioPath[0][0];
			gioId = Help::String::toLowercase(gioId.substr(0, gioId.size()-8));

			ai = mAppInfoIds.get(gioId);

			for(int i = 0; gioPath[i] != NULL; ++i)
				g_strfreev(gioPath[i]);
			g_free(gioPath);

			if(ai != NULL) return ai;
		}

		return new AppInfo({"", "", id});
	}

	void launch(AppInfo* appInfo) //TODO move to AppInfo struct
	{
		GDesktopAppInfo* info = g_desktop_app_info_new_from_filename(appInfo->path.c_str());
		const gchar * const* actions = g_desktop_app_info_list_actions(info);

		g_app_info_launch((GAppInfo*)info, NULL, NULL, NULL);
	}
	
}