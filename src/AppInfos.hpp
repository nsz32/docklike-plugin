/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#ifndef APPINFOS_HPP
#define APPINFOS_HPP

#include <pthread.h>
#include <sys/inotify.h>

#include <iostream>

#include "Helpers.hpp"
#include "Store.tpp"

struct AppInfo
{
	const std::string path;
	const std::string icon;
	const std::string name;
};

namespace AppInfos
{
	void init();

	AppInfo* search(std::string id);
	void launch(AppInfo* appInfo);
} // namespace AppInfos

#endif