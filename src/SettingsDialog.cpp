#include "SettingsDialog.hpp"

namespace SettingsDialog
{
	void popup()
	{
		GtkWidget* dialogWindow = xfce_titled_dialog_new_with_buttons(
			"Docklike taskbar",
			GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(Plugin::mXfPlugin))),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			"gtk-close", GTK_RESPONSE_OK, NULL);

		gtk_window_set_wmclass(GTK_WINDOW(dialogWindow), "xfce4-panel", "xfce4-panel");
		gtk_window_set_icon_name(GTK_WINDOW(dialogWindow), "preferences-system-windows");
		xfce_titled_dialog_set_subtitle(XFCE_TITLED_DIALOG(dialogWindow), "‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾");

		GtkGrid* grid = (GtkGrid*)gtk_grid_new();
		gtk_grid_set_row_homogeneous(grid, false);
		gtk_grid_set_column_homogeneous(grid, false);
		gtk_grid_set_column_spacing(grid, 10);

		gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialogWindow))), GTK_WIDGET(grid));

		// Force icon size

		GtkWidget* iconSizeToggle = gtk_check_button_new_with_label("Force icon size : ");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(iconSizeToggle), Settings::forceIconSize);
		gtk_grid_attach(grid, iconSizeToggle, 0, 0, 1, 1);

		g_signal_connect(
			G_OBJECT(iconSizeToggle), "toggled",
			G_CALLBACK(+[](GtkToggleButton* iconSizeToggle) {
				Settings::forceIconSize.set(gtk_toggle_button_get_active(iconSizeToggle));
			}),
			NULL);

		GtkWidget* iconSizeValue = gtk_combo_box_text_new_with_entry();
		gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(iconSizeValue), 5);
		gtk_entry_set_width_chars(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(iconSizeValue))), 3);
		gtk_entry_set_max_length(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(iconSizeValue))), 3);
		gtk_entry_set_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(iconSizeValue))), std::to_string(Settings::iconSize).c_str());
		for (const char* choice : {"16", "24", "32", "48", "64", "96", "256"})
			gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(iconSizeValue), choice);

		g_signal_connect(
			G_OBJECT(iconSizeValue), "changed",
			G_CALLBACK(+[](GtkComboBox* iconSizeValue) {
				GtkEntry* entry = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(iconSizeValue)));

				std::string value = Help::String::numericOnly(gtk_entry_get_text(entry));
				gtk_entry_set_text(entry, value.c_str());
				Settings::iconSize.set(std::stoi("0" + value));
			}),
			NULL);

		gtk_grid_attach(grid, iconSizeValue, 2, 0, 1, 1);

		// No windows list if single

		GtkWidget* noWindowsListIfSingle = gtk_check_button_new_with_label("Don't display list for a single window");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(noWindowsListIfSingle), Settings::noWindowsListIfSingle);
		gtk_grid_attach(grid, noWindowsListIfSingle, 0, 1, 1, 1);

		g_signal_connect(
			G_OBJECT(noWindowsListIfSingle), "toggled",
			G_CALLBACK(+[](GtkToggleButton* noWindowsListIfSingle) {
				Settings::noWindowsListIfSingle.set(gtk_toggle_button_get_active(noWindowsListIfSingle));
			}),
			NULL);

		// ------------------

		xfce_panel_plugin_block_menu(Plugin::mXfPlugin);

		gtk_window_set_position(GTK_WINDOW(dialogWindow), GTK_WIN_POS_CENTER);
		gtk_widget_show_all(dialogWindow);

		g_signal_connect(
			G_OBJECT(dialogWindow), "response",
			G_CALLBACK(+[](GtkDialog* dialog, gint responseId, GtkWidget* dialogWindow) {
				gtk_widget_hide(dialogWindow);
				xfce_panel_plugin_unblock_menu(Plugin::mXfPlugin);
			}),
			dialogWindow);
	}
} // namespace SettingsDialog