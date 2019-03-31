#ifndef DOCK_BUTTON_HPP
#define DOCK_BUTTON_HPP

#include <iostream>

#include <gtk/gtk.h>



class DockButton
{
	public:

		enum Style
		{
			Focus,
			Opened,
			Many,
			Hover
		};

		DockButton(bool pinned);

		void resize();
		void setStyle(Style style, bool val);

		void onDraw(cairo_t* cr);

		bool onDragMotion(GdkDragContext* context, int x, int y, guint time);
		void onDragLeave(const GdkDragContext* context, guint time);
		void onDragDataGet(const GdkDragContext* context, GtkSelectionData* selectionData, guint info, guint time);
		void onDragDataReceived(const GdkDragContext* context, int x, int y, const GtkSelectionData* selectionData, guint info, guint time);

		bool mPinned;
		GtkWidget* mButton;

	private:

		bool mSFocus;
		bool mSOpened;
		bool mSMany;
		bool mSHover;

};

#endif 