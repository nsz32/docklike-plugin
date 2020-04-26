#include "Settings.hpp"

namespace Settings
{
	GtkWidget* mDialogWindow;

	void init()
	{
	}

	void popup()
	{
		GtkWidget* dialog = xfce_titled_dialog_new_with_buttons(
			"Docklike Taskbar",
			GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(Plugin::mXfPlugin))),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			"gtk-close", GTK_RESPONSE_OK, NULL);

		gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

		gtk_window_set_icon_name(GTK_WINDOW(dialog), "xfce4-settings");

		g_object_set_data(G_OBJECT(Plugin::mXfPlugin), "dialog", dialog);

		//g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(handler), context);

		gtk_widget_show(dialog);
	}
} // namespace Settings