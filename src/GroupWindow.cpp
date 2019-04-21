// ** opensource.org/licenses/GPL-3.0

#include "GroupWindow.hpp"

GroupWindow::GroupWindow(WnckWindow* wnckWindow):
	mDockButtonMenuItem(this)
{ 
	mWnckWindow = wnckWindow;

	std::string groupName = Wnck::getGroupName(this); //check here for exotic group association (like libreoffice)
	AppInfo* appInfo = AppInfos::search(groupName);

	getInGroup(Dock::prepareGroup(appInfo));

	//signal connection
	g_signal_connect(G_OBJECT(mWnckWindow), "name-changed",
	G_CALLBACK(+[](WnckWindow* window, GroupWindow* me)
	{ 
		me->mDockButtonMenuItem.updateLabel();
	}), this);

	g_signal_connect(G_OBJECT(mWnckWindow), "icon-changed",
	G_CALLBACK(+[](WnckWindow* window, GroupWindow* me)
	{ 
		me->mDockButtonMenuItem.updateIcon();
	}), this);

	g_signal_connect(G_OBJECT(mWnckWindow), "state-changed",
	G_CALLBACK(+[](WnckWindow* window, WnckWindowState changed_mask, WnckWindowState new_state, GroupWindow* me)
	{ 
		me->updateState(new_state, changed_mask);
	}), this);

	//initial state
	updateState(Wnck::getState(this));

	mDockButtonMenuItem.updateIcon();
	mDockButtonMenuItem.updateLabel();
}

GroupWindow::~GroupWindow()
{
	leaveGroup(mGroup);
}

void GroupWindow::getInGroup(Group* group)
{
	std::cout << "IN:" << 1 << std::endl;

	mGroup = group;
	group->mWindows.push(this);
	group->mDockButtonMenu.add(mDockButtonMenuItem);


	group->updateStyle();
}

void GroupWindow::leaveGroup(Group* group)
{
	group->setStyle(DockButton::Style::Focus, false);
	bool wasTopWindow = group->mWindows.getIndex(this) == group->mTopWindowIndex;

	group->mWindows.pop(this);
	group->mDockButtonMenu.remove(mDockButtonMenuItem);

	if(wasTopWindow) group->electNewTopWindow();

	group->updateStyle();
}

void GroupWindow::onActivate()
{
	Help::Gtk::cssClassAdd(GTK_WIDGET(mDockButtonMenuItem.mItem), "active");
	gtk_widget_queue_draw(GTK_WIDGET(mDockButtonMenuItem.mItem));

	mGroup->onWindowActivate(this);
}

void GroupWindow::onUnactivate()
{
	Help::Gtk::cssClassRemove(GTK_WIDGET(mDockButtonMenuItem.mItem), "active");
	gtk_widget_queue_draw(GTK_WIDGET(mDockButtonMenuItem.mItem));

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

void GroupWindow::showMenu()
{
	
}

void GroupWindow::updateState(ushort state, ushort changeMask)
{
	mState = state;

	if(changeMask & WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST)
	{
		mGroup->updateStyle();
		
		if(state & WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST)
			gtk_widget_hide(GTK_WIDGET(mDockButtonMenuItem.mItem));
		else
			gtk_widget_show(GTK_WIDGET(mDockButtonMenuItem.mItem));
	}
}