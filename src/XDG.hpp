#ifndef XDG_HPP
#define XDG_HPP

#include <iostream>
#include <unistd.h>

#include <string>
#include <list>

#include "Utility.hpp"

#include <dirent.h>
#include <sys/types.h>
#include <regex>
#include <fstream>
#include <sstream>

namespace nmt
{

	class XDG {
		public:

			static void init()
			{
				//Setting directories for .desktop files
				//1.
				char* var = getenv("XDG_DATA_HOME");
				std::string xdgDataHome;
				if(var != NULL && var[0] != '\0')
					xdgDataDirs.push_back(var);
				else
					xdgDataDirs.push_back(std::string(getenv("HOME")) + "/.local/share");

				//2.
				var = getenv("XDG_DATA_DIRS");
				if(var != NULL && var[0] != '\0')
				{
					nmt::Utils::String::split(var, xdgDataDirs, ':');
				}
				else
				{
					xdgDataDirs.push_back("/usr/local/share");
					xdgDataDirs.push_back("/usr/share");
				}

				for (std::list<std::string>::iterator it = xdgDataDirs.begin(); it != xdgDataDirs.end(); ++it)
					*it += "/applications/";
			}

			static std::string findDesktopFile(std::string className)
			{
				std::string desktopFile = searchNameInDirs(className);
				if(!desktopFile.empty())
					return desktopFile;
				
				uint sep = className.find("-");
				if(sep != std::string::npos)
				{
					desktopFile = searchNameInDirs(className.substr(0, sep));
					if(!desktopFile.empty())
						return desktopFile;
				}

				//TODO try with many transformations of className (against exotics)
				return "";
			}

			static std::string readIconName(std::string fileName)
			{
				std::ifstream file(fileName);

				std::string line;
				std::getline(file, line);
				if(line != "[Desktop Entry]") return "";
				while (std::getline(file, line))
				{
					if(line[0] == 'I')
					{
						uint sep = line.find("=");
						if(line.substr(0,sep) == "Icon") return line.substr(sep+1, std::string::npos);
					}
				}

				return "";
			}

			static std::string searchNameInDirs(const std::string& name)
			{
				std::regex regexpr("(.*)(" + name + ")(.*)(\\.desktop)", std::regex::icase);

				for (std::list<std::string>::iterator dirit = xdgDataDirs.begin(); dirit != xdgDataDirs.end(); ++dirit)
				{
					DIR *directory = opendir(dirit->c_str());
					if(directory == NULL) continue;
					
					struct dirent *entry;
					std::list<std::string> matches;
					while((entry = readdir(directory)) != NULL )
					{
						if (std::regex_match(entry->d_name,regexpr))
							matches.push_back(entry->d_name);
					}
					if(matches.size() == 0) continue;
					
					uint minSize = UINT16_MAX;
					std::string selected;
					for (std::list<std::string>::iterator it = matches.begin(); it != matches.end(); ++it)
					{
						if(it->size() < minSize)
						{
							minSize = it->size();
							selected = *it;
						}
					}
					return dirit->c_str() + selected;
				}
				return "";
			}

		private:
			static std::list<std::string> xdgDataDirs;
	};

}

#endif