// ** opensource.org/licenses/GPL-3.0

#include "Group.hpp"

#include <gtkmm/stylecontext.h>
#include <iostream>

#include "XDG.hpp"

nmt::Group::Group(std::string classGroupName)
{
	className = classGroupName;

	set_label("");
	set_tooltip_text(className);

	add_events(Gdk::EventMask::SCROLL_MASK);

	init();
	initIcon();

	set_always_show_image(true);
}

void nmt::Group::addWindow(Window* window)
{
	mWindows.push(window->getXID(), window);
}

void nmt::Group::removeWindow(gulong XID)
{
	Window* window = mWindows.pop(XID);
	delete window;

	updateVisibility();
}

void nmt::Group::updateVisibility()
{
	Window* window = mWindows.findIf([](std::pair<gulong, Window*> r)->bool
		{ return !r.second->getState(WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST); });
	if(window != NULL)
		show();
	else
		hide();
}

bool nmt::Group::onButtonClicked(GdkEventButton* event)
{
	if(active)
	{
		mWindows.last()->minimize();
	}
	else
	{
		guint32 timestamp = event->time;
		mWindows.forEach([&timestamp](std::pair<gulong, Window*> w)->void { w.second->activate(timestamp); });
	}

	return true;
}

bool nmt::Group::onMouseScroll(GdkEventScroll* event)
{
	if(event->direction == GDK_SCROLL_DOWN)
	{
		mWindows.shiftToBack();

		guint32 timestamp = event->time;

		mWindows.last()->activate(timestamp);
	}
	else if(event->direction == GDK_SCROLL_UP)
	{
		mWindows.shiftToFront();
		
		guint32 timestamp = event->time;

		mWindows.last()->activate(timestamp);
	}
	return true;
}

void nmt::Group::onWindowActivate(gulong XID)
{
	Window* window = mWindows.moveBack(XID);

	active = true;
	get_style_context()->add_class("active");

	set_label(wnck_window_get_name(window->mWnckWindow));
}

void nmt::Group::onWindowUnactivate()
{
	active = false;
	get_style_context()->remove_class("active");

	set_label("");
}

void nmt::Group::init()
{
	signal_button_release_event().connect(sigc::mem_fun(*this, &nmt::Group::onButtonClicked));
	signal_scroll_event().connect(sigc::mem_fun(*this, &nmt::Group::onMouseScroll));
}

void nmt::Group::initIcon()
{
	std::string df = nmt::XDG::findDesktopFile(className);
	if(!df.empty())
	{
		std::string in = nmt::XDG::readIconName(df);
		set_image_from_icon_name(in);
	}
	else
	{
		set_image_from_icon_name("application-x-executable");
	}
	
}
