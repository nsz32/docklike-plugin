#include "Settings.hpp"

namespace Settings
{
	GtkWidget* popup()
	{

		GtkWidget* dialogWindow = xfce_titled_dialog_new_with_buttons(
			"Docklike Taskbar",
			GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(Plugin::mXfPlugin))),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			"gtk-close", GTK_RESPONSE_OK, NULL);

		gtk_window_set_icon_name(GTK_WINDOW(dialogWindow), "preferences-system-windows");

		g_signal_connect(
			G_OBJECT(dialogWindow), "response",
			G_CALLBACK(+[](GtkDialog* dialog, gint responseId, GtkWidget* dialogWindow) {
				gtk_widget_hide(dialogWindow);
			}),
			dialogWindow);

		g_object_set_data(G_OBJECT(Plugin::mXfPlugin), "dialog", dialogWindow);

		
		gtk_window_set_position(GTK_WINDOW(dialogWindow), GTK_WIN_POS_CENTER);
		gtk_widget_show(dialogWindow);

		return dialogWindow;
	}
} // namespace Settings