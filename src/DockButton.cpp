#include "DockButton.hpp"

#include "Dock.hpp"
#include "DockButtonMenu.hpp"

DockButton::DockButton(bool pinned):
	mDockButtonMenu(this)
{
	mButton = gtk_button_new();
	gtk_style_context_add_class(gtk_widget_get_style_context(mButton), "flat");
	gtk_style_context_add_class(gtk_widget_get_style_context(mButton), "docklike_group");
	
	mPinned = pinned;

	mSFocus = mSOpened = mSMany = mSHover = false;

	g_signal_connect(G_OBJECT(mButton), "drag-motion",
	G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, gint x, gint y, guint time, DockButton* me){
		me->onDragMotion(context, x, y, time);
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mButton), "drag-leave",
	G_CALLBACK(+[](GtkWidget* widget, GdkDragContext *context, guint time, DockButton* me){
		me->onDragLeave(context, time);
	}), this);

	g_signal_connect(G_OBJECT(mButton), "drag-data-get",
	G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, GtkSelectionData* data, guint info, guint time, DockButton* me){
		me->onDragDataGet(context, data, info, time);
	}), this);

	g_signal_connect(G_OBJECT(mButton), "drag-data-received",
	G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, gint x, gint y, GtkSelectionData* data, guint info, guint time, DockButton* me){
		me->onDragDataReceived(context, x, y, data, info, time);
	}), this);

	g_signal_connect(G_OBJECT(mButton), "enter-notify-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEvent* event, DockButton* me){
		me->onMouseEnter();
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mButton), "leave-notify-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEvent* event, DockButton* me){
		me->setMouseLeaveTimeout();
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mButton), "draw",
	G_CALLBACK(+[](GtkWidget* widget, cairo_t* cr, DockButton* me){
		me->onDraw(cr); return false;
	}), this);
}

void DockButton::resize()
{
	gtk_widget_set_size_request(mButton, (((Dock::mPanelSize*1.2)/2)*2)-1, Dock::mPanelSize);

	GtkWidget* img = gtk_button_get_image(GTK_BUTTON(mButton));
	gtk_image_set_pixel_size(GTK_IMAGE(img), Dock::mIconSize);
}

void DockButton::setStyle(Style style, bool val)
{
	switch(style) {
		case Style::Focus : {
			if(mSFocus != val){ mSFocus = val; gtk_widget_queue_draw(mButton); }
			break;
		}
		case Style::Opened: {
			if(mSOpened != val){ mSOpened = val; gtk_widget_queue_draw(mButton); }
			break;
		}
		case Style::Many: {
			if(mSMany != val){ mSMany = val; gtk_widget_queue_draw(mButton); }
			break;
		}
		case Style::Hover: {
			if(mSHover != val){ mSHover = val; gtk_widget_queue_draw(mButton); }
			break;
		}
	}
}

void DockButton::onDraw(cairo_t* cr)
{
	double aBack = 0.0;
	
	if(mSHover) aBack += 0.3;
	if(mSFocus) aBack += 0.5;

	int w = gtk_widget_get_allocated_width(GTK_WIDGET(mButton));
	int h = gtk_widget_get_allocated_height(GTK_WIDGET(mButton));

	if(aBack > 0){
		cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, aBack);
		cairo_rectangle(cr, 0, 0, w, h);
		cairo_fill(cr);
	}

	if(mSOpened){
		if(mSFocus)
			cairo_set_source_rgba(cr, 0.30, 0.65, 0.90, 1);
		else
			cairo_set_source_rgba(cr, 0.7, 0.7, 0.7, 1);
		
		cairo_rectangle(cr, 0, h*0.9231, w, h);
		cairo_fill(cr);
	}

	if(mSMany && (mSOpened || mSHover))
	{
		int x1 = (int) w*0.85;
		cairo_pattern_t *pat = cairo_pattern_create_linear(x1, 0, w, 0);

		cairo_pattern_add_color_stop_rgba(pat, 0.0, 0, 0, 0, 0.45);
		cairo_pattern_add_color_stop_rgba(pat, 0.1, 0, 0, 0, 0.35);
		cairo_pattern_add_color_stop_rgba(pat, 0.3, 0, 0, 0, 0.15);

		if(false && aBack > 0)
			cairo_rectangle(cr, x1, 0, w, h);
		else
			cairo_rectangle(cr, x1, h*0.9231, w, h);

		cairo_set_source(cr, pat);
		cairo_fill(cr);
	}

}

void DockButton::onMouseEnter()
{
	setStyle(Style::Hover, true);
	mLeaveTimeout.stop();
	mDockButtonMenu.popup();
}

void DockButton::onMouseLeave()
{
	if(!mDockButtonMenu.mMouseHover)
	{
		this->setStyle(Style::Hover, false);
		mDockButtonMenu.hide();
	}
}

void DockButton::setMouseLeaveTimeout()
{
	mLeaveTimeout.start(5, G_SOURCE_FUNC(+[](DockButton* me){
		me->onMouseLeave();
	}), this);
}

bool DockButton::onDragMotion(GdkDragContext* context, int x, int y, guint time)
{
	gtk_style_context_add_class(gtk_widget_get_style_context(mButton), "drop");

	gdk_drag_status(context, GDK_ACTION_MOVE, time);
	return true;
}

void DockButton::onDragLeave(const GdkDragContext* context, guint time)
{
	gtk_style_context_remove_class(gtk_widget_get_style_context(mButton), "drop");
}

void DockButton::onDragDataGet(const GdkDragContext* context, GtkSelectionData* selectionData, guint info, guint time)
{
	DockButton* me = this;
	std::cout << "pme:" << me << std::endl;

	//TODO is the source object copied or passed by a pointer ?
	gtk_selection_data_set(selectionData, gdk_atom_intern("button", false), 32, (const guchar*)me, sizeof (gpointer)*32);
}

void DockButton::onDragDataReceived(const GdkDragContext* context, int x, int y, const GtkSelectionData* selectionData, guint info, guint time)
{
	GdkAtom dt = gtk_selection_data_get_data_type(selectionData);
	//if(gdk_atom_name(dt) == "button")
	
	DockButton* source = (DockButton*)gtk_selection_data_get_data(selectionData);
	Dock::moveButton(source, this);
	
	
}

