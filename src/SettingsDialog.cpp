// ** opensource.org/licenses/GPL-3.0

#include "SettingsDialog.hpp"

namespace SettingsDialog
{
	void popup()
	{
		xfce_panel_plugin_block_menu(Plugin::mXfPlugin);

		/* Hook to make sure GtkBuilder knows are the XfceTitledDialog object */
		if (xfce_titled_dialog_get_type() == 0)
			return;
		GtkBuilder* builder = gtk_builder_new_from_resource("/_dialogs.xml");
		GtkWidget* dialog = (GtkWidget*)gtk_builder_get_object(builder, "dialog");
		gtk_window_set_wmclass(GTK_WINDOW(dialog), "xfce4-panel", "xfce4-panel");
		gtk_widget_show_all(dialog);

		g_signal_connect(
			gtk_builder_get_object(builder, "b_close"), "clicked",
			G_CALLBACK(+[](GtkButton* button, GtkWidget* dialogWindow) {
				gtk_widget_hide(dialogWindow);
				gtk_dialog_response((GtkDialog*)dialogWindow, 0);
				xfce_panel_plugin_unblock_menu(Plugin::mXfPlugin);
			}),
			dialog);

		GObject* noListForSingleWindow = gtk_builder_get_object(builder, "c_noListForSingleWindow");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(noListForSingleWindow), Settings::noWindowsListIfSingle);
		g_signal_connect(noListForSingleWindow, "toggled",
			G_CALLBACK(+[](GtkToggleButton* noWindowsListIfSingle) {
				Settings::noWindowsListIfSingle.set(gtk_toggle_button_get_active(noWindowsListIfSingle));
			}),
			NULL);

		GObject* indicatorStyle = gtk_builder_get_object(builder, "co_indicatorStyle");
		gtk_combo_box_set_active(GTK_COMBO_BOX(indicatorStyle), Settings::indicatorStyle);
		g_signal_connect(indicatorStyle, "changed",
			G_CALLBACK(+[](GtkComboBox* indicatorStyle) {
				Settings::indicatorStyle.set(gtk_combo_box_get_active(GTK_COMBO_BOX(indicatorStyle)));
			}),
			NULL);

		GObject* iconSize = gtk_builder_get_object(builder, "e_iconSize");
		gtk_entry_set_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(iconSize))), std::to_string(Settings::iconSize).c_str());
		gtk_widget_set_sensitive(GTK_WIDGET(iconSize), Settings::forceIconSize);
		g_signal_connect(iconSize, "changed",
			G_CALLBACK(+[](GtkComboBox* iconSize) {
				GtkEntry* entry = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(iconSize)));
				std::string value = Help::String::numericOnly(gtk_entry_get_text(entry));
				gtk_entry_set_text(entry, value.c_str());
				Settings::iconSize.set(std::stoi("0" + value));
			}),
			NULL);

		GObject* forceIconSize = gtk_builder_get_object(builder, "c_forceIconSize");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(forceIconSize), Settings::forceIconSize);
		g_signal_connect(forceIconSize, "toggled",
			G_CALLBACK(+[](GtkToggleButton* forceIconSize, GtkWidget* iconSize) {
				Settings::forceIconSize.set(gtk_toggle_button_get_active(forceIconSize));
				gtk_widget_set_sensitive(GTK_WIDGET(iconSize), Settings::forceIconSize);
			}),
			iconSize);

		return;

		// Indicator style

		/*GtkWidget* label = gtk_label_new("Indicator style : ");
		gtk_grid_attach(grid, label, 0, 1, 1, 1);

		GtkWidget* indicatorStyle = gtk_combo_box_text_new();
		for (const char* choice : {"Bar", "Dots"})
			gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(indicatorStyle), choice);
		gtk_combo_box_set_active(GTK_COMBO_BOX(indicatorStyle), Settings::indicatorStyle);

		g_signal_connect(
			G_OBJECT(indicatorStyle), "changed",
			G_CALLBACK(+[](GtkComboBox* indicatorStyle) {
				Settings::indicatorStyle.set(gtk_combo_box_get_active(GTK_COMBO_BOX(indicatorStyle)));
			}),
			NULL);

		gtk_grid_attach(grid, indicatorStyle, 1, 1, 1, 1);

		// Force icon size

		GtkWidget* iconSizeToggle = gtk_check_button_new_with_label("Force icon size : ");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(iconSizeToggle), Settings::forceIconSize);
		gtk_grid_attach(grid, iconSizeToggle, 0, 2, 1, 1);

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

		gtk_grid_attach(grid, iconSizeValue, 1, 2, 1, 1);

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
			dialogWindow);*/
	}
} // namespace SettingsDialog