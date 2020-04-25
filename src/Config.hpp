#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <glib.h>

#include <iostream>
#include <list>
#include <string>
#include <vector>

class Config
{
  public:
	Config(std::string path);
	void save();

	void setPinned(std::list<std::string> pinnedApps);
	std::list<std::string> getPinned();

  private:
	GKeyFile* mFile;
	std::string mPath;
};

#endif