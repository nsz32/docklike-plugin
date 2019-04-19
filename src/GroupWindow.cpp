// ** opensource.org/licenses/GPL-3.0

#include "GroupWindow.hpp"

GroupWindow::GroupWindow(WnckWindow* wnckWindow):
	mDockButtonMenuItem(this)
{ 
	mWnckWindow = wnckWindow;
	
	mDockButtonMenuItem.setLabel(Wnck::getName(this));

	std::string groupName = Wnck::getGroupName(this); //check here for exotic group association (like libreoffice)
	AppInfo* appInfo = AppInfos::search(groupName);
	mGroup = Dock::prepareGroup(appInfo);
	getInGroup(mGroup);

	//signal connection
	g_signal_connect(G_OBJECT(mWnckWindow), "name-changed",
	G_CALLBACK(+[](WnckWindow* window, GroupWindow* me)
	{ 
		me->mDockButtonMenuItem.setLabel(Wnck::getName(me));
	}), this);

	g_signal_connect(G_OBJECT(mWnckWindow), "state-changed",
	G_CALLBACK(+[](WnckWindow* window, WnckWindowState changed_mask, WnckWindowState new_state, GroupWindow* me)
	{ 
		me->updateState(new_state, changed_mask);
	}), this);

	//initial state
	updateState(Wnck::getState(this));
}

GroupWindow::~GroupWindow()
{
	leaveGroup(mGroup);
}

void GroupWindow::getInGroup(Group* group)
{
	group->mWindows.push(this);
	group->mDockButtonMenu.add(mDockButtonMenuItem);

	group->updateStyle();
}

void GroupWindow::leaveGroup(Group* group)
{
	bool wasTopWindow = group->mWindows.getIndex(this) == group-> mTopWindowIndex;

	group->mWindows.pop(this);
	group->mDockButtonMenu.remove(mDockButtonMenuItem);

	if(wasTopWindow) group->electNewTopWindow();

	group->updateStyle();
}

void GroupWindow::onActivate()
{
	Help::Gtk::cssClassAdd(mDockButtonMenuItem.mTitleButton, "active");
	gtk_widget_queue_draw(mDockButtonMenuItem.mTitleButton);

	mGroup->onWindowActivate(this);
}

void GroupWindow::onUnactivate()
{
	Help::Gtk::cssClassRemove(mDockButtonMenuItem.mTitleButton, "active");
	gtk_widget_queue_draw(mDockButtonMenuItem.mTitleButton);

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
			gtk_widget_hide(mDockButtonMenuItem.mTitleButton);
		else
			gtk_widget_show(mDockButtonMenuItem.mTitleButton);
	}
}