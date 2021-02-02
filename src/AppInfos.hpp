/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#ifndef APPINFOS_HPP
#define APPINFOS_HPP

#include <pthread.h>
#include <sys/inotify.h>

#include <gio/gdesktopappinfo.h>

#include <iostream>

#include "Helpers.hpp"
#include "Store.tpp"

struct AppInfo
{
	const std::string path;
	const std::string icon;
	const std::string name;
	const gchar* const* actions;

	void launch();
	void launch_action(const gchar* action);
	void edit();
};

namespace AppInfos
{
	void init();

	AppInfo* search(std::string id);
} // namespace AppInfos

#endif