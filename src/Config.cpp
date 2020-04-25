#include "Config.hpp"

Config::Config(std::string path)
{
	mPath = path;

	std::cout << "SAVEPATH:" << path << std::endl;

	mFile = g_key_file_new();
	g_key_file_load_from_file(mFile, mPath.c_str(), G_KEY_FILE_NONE, NULL);
}

void Config::save() { g_key_file_save_to_file(mFile, mPath.c_str(), NULL); }

void Config::setPinned(std::list<std::string> pinnedApps)
{
	std::vector<char*> buf;
	for (std::string& s : pinnedApps)
		buf.push_back(&s[0]);

	g_key_file_set_string_list(mFile, "user", "pinned", buf.data(), buf.size());
}

std::list<std::string> Config::getPinned()
{
	std::list<std::string> ret;
	gchar** clist = g_key_file_get_string_list(mFile, "user", "pinned", NULL, NULL);

	if (clist != NULL)
		for (int i = 0; clist[i] != NULL; ++i)
			ret.push_back(clist[i]);

	g_strfreev(clist);
	return ret;
}