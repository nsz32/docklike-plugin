#include "Theme.hpp"

namespace Theme
{
	GdkScreen* mScreen;
	GtkCssProvider* mCssProvider;
	GtkStyleContext* mStyleContext;

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

		std::string cssStyle = "@define-color docklike_bg_color " + bgColor + ";"
		"button { border:none; border-radius:0; background:none; }"
		/*"button.opened { box-shadow: inset 0px -2px alpha(@docklike_bg_color, 0.5); }"
		"button.active { box-shadow: inset 0px -2px alpha(@docklike_bg_color, 1); }"*/
		"button.drop { border-left: 5px solid @theme_selected_bg_color; }";

		if(!gtk_css_provider_load_from_data(mCssProvider, cssStyle.c_str(), -1, NULL))
		{
			std::cerr << "ERROR: CSS LOAD FAILED !" << std::endl;
		}
		else
		{
			gtk_style_context_add_provider_for_screen(mScreen, GTK_STYLE_PROVIDER(mCssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
		}
	}
}