CC = g++

CFLAGS = -g $(shell pkg-config --cflags libxfce4panel-2.0 libxfce4ui-2 gtkmm-3.0 libwnck-3.0)
LDLIBS = $(shell pkg-config --libs libxfce4panel-2.0 libxfce4ui-2 gtkmm-3.0 libwnck-3.0)

DEFINES = -DPACKAGE_VERSION="0.1" -DPACKAGE_NAME="\"nmt\"" -DGETTEXT_PACKAGE="\"gettext_package\"" -DPACKAGE_LOCALE_DIR="\"/\"" -DWNCK_I_KNOW_THIS_IS_UNSTABLE

OBJS := obj/register.o obj/Plugin.o obj/Group.o obj/Taskbar.o obj/GroupWindow.o obj/AppInfos.o obj/Helpers.o obj/Config.o obj/Wnck.o
-include $(OBJS:.o=.d)

all : libnmt.so

libnmt.so: $(OBJS)
	$(CC) $(LDLIBS) -Wl,--no-undefined -shared -o libnmt.so $(OBJS)

obj/%.o: src/%.cpp
	$(CC) $(DEFINES) $(CFLAGS) -fPIC -o $@ -c $<
	$(CC) $(DEFINES) -MT $@ -MM $< > obj/$*.d

obj/register.o:
	gcc $(CFLAGS) -fPIC -o $@ -c src/register.c

clean:
	rm -f obj/*.o obj/*.d *.so

install:
	cp libnmt.so /usr/lib/xfce4/panel/plugins/
	cp nmt.desktop /usr/share/xfce4/panel/plugins/

uninstall:
	rm /usr/lib/xfce4/panel/plugins/libnmt.so
	rm /usr/share/xfce4/panel/plugins/nmt.desktop