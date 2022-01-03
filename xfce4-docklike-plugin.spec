%global git_hash 0a64857f

%if 0%{?fedora} >= 29
    %global xfceversion 4.13
%else
    %global xfceversion 4.12
%endif

Name:           xfce4-docklike-plugin
Version:        0.1
Release:        2.git%{git_hash}%{?dist}
Summary:        A modern taskbar for XFCE

License:        GPLv3
URL:            http://goodies.xfce.org/projects/panel-plugins/%{name}
Source0:        https://github.com/nsz32/docklike-plugin/archive/master.tar.gz

BuildRequires:  gcc-c++
BuildRequires:  libxfce4ui-devel >= %{xfceversion}
BuildRequires:  xfce4-panel-devel >= %{xfceversion}
BuildRequires:  xfce4-dev-tools
BuildRequires:  gtk3-devel
BuildRequires:  cairo-devel
BuildRequires:  libwnck3-devel
BuildRequires:  libtool
Requires:       xfce4-panel >= %{xfceversion}
Requires:       xfce4-settings


%description
A WIP modern taskbar for XFCE C++ - GTK3 - CSS - DOCK-LIKE - MINIMALISM

%prep
%setup -q -n docklike-plugin-master


%build
./autogen.sh
%configure --disable-static
%make_build


%install
%make_install

# remove la file
find %{buildroot} -name '*.la' -exec rm -f {} ';'

# make sure debuginfo is generated properly
chmod -c +x %{buildroot}%{_libdir}/xfce4/panel/plugins/*.so

#%find_lang %{name}


%files
%license LICENCE
%doc AUTHORS README.md
%{_libdir}/xfce4/panel/plugins/*.so
%{_datadir}/xfce4/panel/plugins/*.desktop
/usr/share/locale/*/LC_MESSAGES/xfce4-docklike-plugin.mo

%changelog
* Mon Jan 3 2021 Tsvetan Gerov <tsvetan@gerov.eu> - 0.1-2-git
- Added locales
* Thu Jun 13 2019 Pavel Podkorytov <pod.pavel@gmail.com> - 0.1-git
- Initial build
