/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include "SettingsDialog.hpp"

namespace SettingsDialog
{
	void updateKeyComboActiveWarning(GtkWidget* widget)
	{
		if (!Settings::keyComboActive || Hotkeys::mGrabbedKeys == Hotkeys::NbHotkeys)
		{
			gtk_widget_hide(widget);
		}
		else
		{
			std::string tooltip;
			if (Hotkeys::mGrabbedKeys > 0)
				tooltip = "<b>Only the first " + std::to_string(Hotkeys::mGrabbedKeys) + " hotkeys(s) are enabled.</b>\n";
			else
				tooltip = "";

			tooltip += "The &lt;SUPER&gt;+" + std::to_string(Hotkeys::mGrabbedKeys + 1) +
				" combination seems already in use by another process.\nCheck your other settings.";

			gtk_widget_set_tooltip_markup(widget, tooltip.c_str());
			gtk_image_set_from_icon_name(GTK_IMAGE(widget), (Hotkeys::mGrabbedKeys == 0) ? "gtk-dialog-error" : "gtk-dialog-warning", GTK_ICON_SIZE_SMALL_TOOLBAR);
			gtk_widget_show(widget);
		}
	}

	void popup()
	{
		xfce_panel_plugin_block_menu(Plugin::mXfPlugin);

		/* Hook to make sure GtkBuilder knows are the XfceTitledDialog object */
		if (xfce_titled_dialog_get_type() == 0)
			return;
		GtkBuilder* builder = gtk_builder_new_from_resource("/_dialogs.xml");
		GtkWidget* dialog = (GtkWidget*)gtk_builder_get_object(builder, "dialog");
		gtk_window_set_wmclass(GTK_WINDOW(dialog), "xfce4-panel", "xfce4-panel");
		gtk_widget_show(dialog);

		g_signal_connect(
			gtk_builder_get_object(builder, "b_close"), "clicked",
			G_CALLBACK(+[](GtkButton* button, GtkWidget* dialogWindow) {
				gtk_widget_hide(dialogWindow);
				gtk_dialog_response((GtkDialog*)dialogWindow, 0);
				xfce_panel_plugin_unblock_menu(Plugin::mXfPlugin);
			}),
			dialog);

		g_signal_connect(dialog, "close",
			G_CALLBACK(+[](GtkDialog* dialog) {
				xfce_panel_plugin_unblock_menu(Plugin::mXfPlugin);
			}),
			NULL);

		g_signal_connect(dialog, "response",
			G_CALLBACK(+[](GtkDialog* dialog, gint response) {
				xfce_panel_plugin_unblock_menu(Plugin::mXfPlugin);
			}),
			NULL);

		// =====================================================================

		GObject* noListForSingleWindow = gtk_builder_get_object(builder, "c_noListForSingleWindow");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(noListForSingleWindow), Settings::noWindowsListIfSingle);
		g_signal_connect(noListForSingleWindow, "toggled",
			G_CALLBACK(+[](GtkToggleButton* noWindowsListIfSingle) {
				Settings::noWindowsListIfSingle.set(gtk_toggle_button_get_active(noWindowsListIfSingle));
			}),
			NULL);

		// =====================================================================

		GObject* indicatorStyle = gtk_builder_get_object(builder, "co_indicatorStyle");
		gtk_combo_box_set_active(GTK_COMBO_BOX(indicatorStyle), Settings::indicatorStyle);
		g_signal_connect(indicatorStyle, "changed",
			G_CALLBACK(+[](GtkComboBox* indicatorStyle, GtkWidget* g) {
				Settings::indicatorStyle.set(gtk_combo_box_get_active(GTK_COMBO_BOX(indicatorStyle)));
			}),
			dialog);

		// =====================================================================

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

		// =====================================================================

		GObject* keyComboActiveWarning = gtk_builder_get_object(builder, "c_keyComboActiveWarning");
		GObject* keyComboActive = gtk_builder_get_object(builder, "c_keyComboActive");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(keyComboActive), Settings::keyComboActive);
		g_signal_connect(keyComboActive, "toggled",
			G_CALLBACK(+[](GtkToggleButton* keyComboActive, GtkWidget* tooltip) {
				Settings::keyComboActive.set(gtk_toggle_button_get_active(keyComboActive));
				updateKeyComboActiveWarning(tooltip);
			}),
			keyComboActiveWarning);
		updateKeyComboActiveWarning(GTK_WIDGET(keyComboActiveWarning));

		// =====================================================================

		GObject* keyAloneActive = gtk_builder_get_object(builder, "c_keyAloneActive");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(keyAloneActive), Settings::keyAloneActive);
		g_signal_connect(keyAloneActive, "toggled",
			G_CALLBACK(+[](GtkToggleButton* keyAloneActive) {
				Settings::keyAloneActive.set(gtk_toggle_button_get_active(keyAloneActive));
			}),
			NULL);
		if (!Hotkeys::mXIExtAvailable)
		{
			gtk_widget_set_sensitive(GTK_WIDGET(keyAloneActive), false);

			GObject* keyAloneActiveWarning = gtk_builder_get_object(builder, "c_keyAloneActiveWarning");
			gtk_widget_show(GTK_WIDGET(keyAloneActiveWarning));
		}
	}
} // namespace SettingsDialog