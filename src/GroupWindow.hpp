/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#ifndef GROUPWINDOW_HPP
#define GROUPWINDOW_HPP

#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include <iostream>

#include "AppInfos.hpp"
#include "Dock.hpp"
#include "Group.hpp"
#include "GroupMenuItem.hpp"
#include "Helpers.hpp"
#include "Plugin.hpp"
#include "Wnck.hpp"

class GroupMenuItem;
class Group;

class GroupWindow
{
  public:
	GroupWindow(WnckWindow* wnckWindow);
	~GroupWindow();

	void getInGroup();
	void leaveGroup();

	void onActivate();
	void onUnactivate();

	bool getState(WnckWindowState flagMask);

	void activate(guint32 timestamp);
	void minimize();
	void showMenu();

	Group* mGroup;
	bool mGroupAssociated;

	WnckWindow* mWnckWindow;
	GroupMenuItem* mGroupMenuItem;

	GdkMonitor* mMonitor = NULL;

	void updateState();
	unsigned short mState;
};

#endif