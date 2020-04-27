#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <glib.h>

#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "Plugin.hpp"

namespace Config
{

	void init();

	void saveFile();

	void setPinned(std::list<std::string> pinnedApps);
	std::list<std::string> getPinned();

}; // namespace Config

#endif