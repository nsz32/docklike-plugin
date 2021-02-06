/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include "Theme.hpp"

namespace Theme
{
	GdkScreen* mScreen;
	GtkCssProvider* mCssProvider;
	GtkStyleContext* mStyleContext;

	std::string setupColors();

	void init()
	{
		mScreen = gdk_screen_get_default();
		mCssProvider = gtk_css_provider_new();
		mStyleContext = gtk_widget_get_style_context(Dock::mBox);

		g_signal_connect(G_OBJECT(mStyleContext), "changed",
			G_CALLBACK(+[](GtkStyleContext* stylecontext) { load(); }), NULL);

		load();
	}

	void load()
	{
		GValue gv = G_VALUE_INIT;
		GdkRGBA* rgba;
		std::string bgColor;

		gtk_style_context_get_property(mStyleContext, "background-color", GTK_STATE_FLAG_NORMAL, &gv);
		rgba = (GdkRGBA*)g_value_get_boxed(&gv);
		rgba->red = 1 - rgba->red;
		rgba->green = 1 - rgba->green;
		rgba->blue = 1 - rgba->blue;
		bgColor = gdk_rgba_to_string(rgba);

		std::string cssStyle =
			setupColors() +
			".stld button { border:none; border-radius:0; background:none; "
			"text-shadow:none; -gtk-icon-shadow:none; box-shadow:none; padding:0px;}"
			//"grid { min-height:1.3em; background-color:red; }"

			".menu_item image { margin-left:0.4em;  }"
			".menu_item label { margin:0.5em 0.5em 0.4em 0.5em; }"
			".menu_item button { margin:0; padding:0; color:@dl_menu_item_color; }"
			".menu_item button label { margin:0.1em 0.7em 0 0.7em; padding:0; "
			"font-weight:bold; }"
			".menu_item grid button { background-color:transparent; }"

			".menu_item { background-color:transparent; color:@dl_menu_item_color; "
			"}"
			".menu_item.hover grid { "
			"background-color:alpha(@dl_menu_item_bgcolor_hover,0.5); }"
			".menu_item.active grid { "
			"background-color:@dl_menu_item_bgcolor_hover; "
			"color:@dl_menu_item_color_hover; }"
			".menu_item.active button { color:@dl_menu_item_color_hover; }"

			".menu_item.active label { color:@dl_menu_item_color; }"

			".menu_item button:hover { background-color:alpha(#888, 0.5); }"

			".stld box { margin:0; padding:0; border:0; border-radius:0; }"
			".menu { background-color:@dl_menu_bgcolor; box-shadow:inset 0 0 0 1px "
			"alpha(black, 0.2); }"
			".stld .drop { border-left:0.5em solid slateblue; }";

		/*"button { border:none; border-radius:0; background:none; text-shadow:none;
    -gtk-icon-shadow:none; box-shadow:none; }" "button.docklike_menu {
    min-height:1.3em; }" "button.docklike_menu:hover {
    background-color:alpha(@dl_menu_bgcolor_hover,1);
    color:@dl_menu_color_hover; border:none; }" "box { margin:0; padding:0; }"
    ".drop { border-left:5px solid red; }";*/

		if (!gtk_css_provider_load_from_data(mCssProvider, cssStyle.c_str(), -1, NULL))
		{
			std::cerr << "ERROR: CSS LOAD FAILED !" << std::endl;
		}
		else
		{
			gtk_style_context_add_provider_for_screen(mScreen, GTK_STYLE_PROVIDER(mCssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
		}

		/*return	"@define-color dl_menu_bgcolor " + menuBg + ";"
                    "@define-color dl_menu_item_color " + itemLabel + ";"
                    "@define-color dl_menu_item_color_hover " + itemLabelHover +
       ";"
                    "@define-color dl_menu_item_bgcolor_hover " + itemBgHover +
       ";";*/
	}

	void applyDefault(GtkWidget* widget)
	{
		gtk_style_context_remove_provider(gtk_widget_get_style_context(widget), GTK_STYLE_PROVIDER(mCssProvider));
	}

	std::string setupColors()
	{
		GtkWidget* menu = gtk_menu_new();

		GtkStyleContext* sc = gtk_widget_get_style_context(menu);

		GValue gv = G_VALUE_INIT;
		gtk_style_context_get_property(sc, "background-color", GTK_STATE_FLAG_NORMAL, &gv);
		GdkRGBA* rgba = (GdkRGBA*)g_value_get_boxed(&gv);
		std::string menuBg = gdk_rgba_to_string(rgba);

		GtkWidget* item = gtk_menu_item_new();
		gtk_menu_attach(GTK_MENU(menu), item, 0, 1, 0, 1);

		sc = gtk_widget_get_style_context(item);

		gv = G_VALUE_INIT;
		gtk_style_context_get_property(sc, "color", GTK_STATE_FLAG_NORMAL, &gv);
		rgba = (GdkRGBA*)g_value_get_boxed(&gv);
		std::string itemLabel = gdk_rgba_to_string(rgba);

		gv = G_VALUE_INIT;
		gtk_style_context_get_property(sc, "color", GTK_STATE_FLAG_PRELIGHT, &gv);
		rgba = (GdkRGBA*)g_value_get_boxed(&gv);
		std::string itemLabelHover = gdk_rgba_to_string(rgba);

		gv = G_VALUE_INIT;
		gtk_style_context_get_property(sc, "background-color", GTK_STATE_FLAG_PRELIGHT, &gv);
		rgba = (GdkRGBA*)g_value_get_boxed(&gv);
		std::string itemBgHover = gdk_rgba_to_string(rgba);

		gtk_widget_destroy(item);
		gtk_widget_destroy(menu);

		return "@define-color dl_menu_bgcolor " + menuBg +
			";"
			"@define-color dl_menu_item_color " +
			itemLabel +
			";"
			"@define-color dl_menu_item_color_hover " +
			itemLabelHover +
			";"
			"@define-color dl_menu_item_bgcolor_hover " +
			itemBgHover + ";";
	}
} // namespace Theme