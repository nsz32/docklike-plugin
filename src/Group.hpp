// ** opensource.org/licenses/GPL-3.0

#ifndef GROUP_HPP
#define GROUP_HPP

#include <iostream>
#include <vector>

#include <gtkmm.h>
#include <libwnck/libwnck.h>

#include "Store.tpp"

#include "Helpers.hpp"
#include "AppInfos.hpp"
#include "Taskbar.hpp"
#include "GroupWindow.hpp"


class GroupWindow;


class Group: public Gtk::Button
{
	public:
		Group(std::string groupName, AppInfo* appInfo, bool pinned);

		void addWindow(GroupWindow* window);
		void removeWindow(gulong XID);

		void updateVisibility();

		void onWindowActivate(gulong XID);
		void onWindowUnactivate();

		//bool onEvent(GdkEvent* event);
		bool onButtonPress(GdkEventButton* event);
		bool onButtonRelease(GdkEventButton* event);
		bool onMouseScroll(GdkEventScroll* scroll_event);

		void onDragBegin(const Glib::RefPtr<Gdk::DragContext>& context);
		bool OnDragMotion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
		void onDragLeave(const Glib::RefPtr<Gdk::DragContext>& context, guint time);
		void onDragDataGet(const Glib::RefPtr<Gdk::DragContext>& context, Gtk::SelectionData& selection_data, guint info, guint time);
		void onDragDataReceived(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);

		std::string mGroupName;
		bool mPinned;
		AppInfo* mAppInfo;
	private:

		bool hasWindows();

		bool mActive;

		Store::KeyStore<gulong, GroupWindow*> mWindows;
};

#endif