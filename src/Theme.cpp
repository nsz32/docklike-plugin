#include "Theme.hpp"

namespace Theme
{
	GdkScreen* mScreen;
	GtkCssProvider* mCssProvider;
	GtkStyleContext* mStyleContext;

	std::string setupColors();

	void init(GtkWidget* panel)
	{
		mScreen = gdk_screen_get_default();
		mCssProvider = gtk_css_provider_new();
		mStyleContext = gtk_widget_get_style_context(panel);

		g_signal_connect(G_OBJECT(mStyleContext), "changed",
		G_CALLBACK(+[](GtkStyleContext* stylecontext)
		{
			load();
		}), NULL);

		load();
	}

	void load()
	{
		GdkRGBA color;
		gtk_style_context_get_background_color(mStyleContext, GTK_STATE_FLAG_FOCUSED, &color);
		color.red = 1 - color.red; color.green = 1 - color.green; color.blue = 1 - color.blue;

		std::string bgColor = gdk_rgba_to_string(&color);

		std::string cssStyle = setupColors() +
		"button { border:none; border-radius:0; background:none; text-shadow:none; -gtk-icon-shadow:none; box-shadow:none; }"
		"button.docklike_menu { min-height:1.3em; }"
		"button.docklike_menu:hover { background-color:alpha(@dl_menu_bgcolor_hover,0.1); border:none; }"
		"box { margin:0; padding:0; }"
		".drop { border-left:5px solid red; }";

		/*"button { border:none; border-radius:0; background:none; text-shadow:none; -gtk-icon-shadow:none; box-shadow:none; }"
		"button.docklike_menu { min-height:1.3em; }"
		"button.docklike_menu:hover { background-color:alpha(@dl_menu_bgcolor_hover,1); color:@dl_menu_color_hover; border:none; }"
		"box { margin:0; padding:0; }"
		".drop { border-left:5px solid red; }";*/

		if(!gtk_css_provider_load_from_data(mCssProvider, cssStyle.c_str(), -1, NULL))
		{
			std::cerr << "ERROR: CSS LOAD FAILED !" << std::endl;
		}
		else
		{
			gtk_style_context_add_provider_for_screen(mScreen, GTK_STYLE_PROVIDER(mCssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
		}

		
	}

	std::string setupColors()
	{
		GtkWidget* item = gtk_menu_item_new();
		gtk_style_context_add_class(gtk_widget_get_style_context(item), "menuitem");
		gtk_style_context_add_class(gtk_widget_get_style_context(item), "button");
		gtk_style_context_add_class(gtk_widget_get_style_context(item), "flat");

		GtkStyleContext* sc = gtk_widget_get_style_context(item);

		GValue gv = G_VALUE_INIT;
		gtk_style_context_get_property(sc, "color", GTK_STATE_FLAG_PRELIGHT, &gv);
		GdkRGBA* rgba = (GdkRGBA*)g_value_get_boxed(&gv);
		std::string hover = gdk_rgba_to_string(rgba);

		gv = G_VALUE_INIT;
		gtk_style_context_get_property(sc, "background-color", GTK_STATE_FLAG_PRELIGHT, &gv);
		rgba = (GdkRGBA*)g_value_get_boxed(&gv);
		std::string bgh = gdk_rgba_to_string(rgba);

		gtk_widget_destroy(item);

		return	"@define-color dl_menu_color_hover " + hover + ";"
				"@define-color dl_menu_bgcolor_hover @theme_selected_bg_color;";
	}
}