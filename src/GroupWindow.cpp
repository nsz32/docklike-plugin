/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include "GroupWindow.hpp"

GroupWindow::GroupWindow(WnckWindow* wnckWindow)
{
	mWnckWindow = wnckWindow;
	mGroupMenuItem = new GroupMenuItem(this);

	std::string groupName = Wnck::getGroupName(this); // check here for exotic group association (like libreoffice)
	AppInfo* appInfo = AppInfos::search(groupName);

	std::cout << "SEARCHING GROUPNAME:" << groupName << std::endl;
	if (appInfo == NULL)
		std::cout << "NO MATCH:" << 0 << std::endl;
	else
	{
		std::cout << "> APPINFO NAME:" << appInfo->name << std::endl;
		std::cout << "> APPINFO PATH:" << appInfo->path << std::endl;
		std::cout << "> APPINFO ICON:" << appInfo->icon << std::endl;
	}

	getInGroup(Dock::prepareGroup(appInfo));

	// signal connection
	g_signal_connect(G_OBJECT(mWnckWindow), "name-changed",
		G_CALLBACK(+[](WnckWindow* window, GroupWindow* me) {
			me->mGroupMenuItem->updateLabel();
		}),
		this);

	g_signal_connect(G_OBJECT(mWnckWindow), "icon-changed",
		G_CALLBACK(+[](WnckWindow* window, GroupWindow* me) {
			me->mGroupMenuItem->updateIcon();
		}),
		this);

	g_signal_connect(G_OBJECT(mWnckWindow), "state-changed",
		G_CALLBACK(+[](WnckWindow* window, WnckWindowState changed_mask,
						WnckWindowState new_state, GroupWindow* me) {
			me->updateState(new_state, changed_mask);
		}),
		this);

	// initial state
	updateState(Wnck::getState(this));

	mGroupMenuItem->updateIcon();
	mGroupMenuItem->updateLabel();
}

GroupWindow::~GroupWindow()
{
	leaveGroup(mGroup);
	delete mGroupMenuItem;
}

void GroupWindow::getInGroup(Group* group)
{
	mGroup = group;
	mGroup->add(this);
}

void GroupWindow::leaveGroup(Group* group) { group->remove(this); }

void GroupWindow::onActivate()
{
	Help::Gtk::cssClassAdd(GTK_WIDGET(mGroupMenuItem->mItem), "active");
	gtk_widget_queue_draw(GTK_WIDGET(mGroupMenuItem->mItem));

	mGroup->onWindowActivate(this);
}

void GroupWindow::onUnactivate()
{
	Help::Gtk::cssClassRemove(GTK_WIDGET(mGroupMenuItem->mItem), "active");
	gtk_widget_queue_draw(GTK_WIDGET(mGroupMenuItem->mItem));

	mGroup->onWindowUnactivate();
}

bool GroupWindow::getState(WnckWindowState flagMask)
{
	return (mState & flagMask) != 0;
}

void GroupWindow::activate(guint32 timestamp)
{
	Wnck::activate(this, timestamp);
}

void GroupWindow::minimize()
{
	Wnck::minimize(this);
}

void GroupWindow::showMenu() {}

void GroupWindow::updateState(ushort state, ushort changeMask)
{
	mState = state;

	if (changeMask & WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST)
	{
		mGroup->mWindowsCount.updateState();

		if (state & WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST)
			gtk_widget_hide(GTK_WIDGET(mGroupMenuItem->mItem));
		else
			gtk_widget_show(GTK_WIDGET(mGroupMenuItem->mItem));
	}
}