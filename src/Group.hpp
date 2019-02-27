// ** opensource.org/licenses/GPL-3.0

#ifndef GROUP_HPP
#define GROUP_HPP

#include <gtkmm.h>
#include <libwnck/libwnck.h>

#include "Store.tpp"

#include "Window.hpp"

namespace nmt {

	class Window;

	class Group: public Gtk::Button
	{
		public:
			Group();
			Group(std::string classGroupName);

			void addWindow(Window* window);
			void removeWindow(gulong XID);

			void updateVisibility();

			void onWindowActivate(gulong XID);
			void onWindowUnactivate();

			bool onButtonClicked(GdkEventButton* event);
			bool onMouseScroll(GdkEventScroll* scroll_event);


		private:
			std::string className;
			void init();
			void initIcon();

			bool active;

			KeyStore<gulong, Window*> mWindows;

	};

}

#endif