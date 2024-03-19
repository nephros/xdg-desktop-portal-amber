Name:       xdg-desktop-portal-sailfish
Summary:    XDG Desktop Portal for Sailfish OS
Version:    1.0.0
Release:    0
License:    LGPLv2+ and LGPLv3+
Source0:    %{name}-%{version}.tar.bz2
BuildRequires:  desktop-file-utils
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(systemd)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  pkgconfig(sailfishapp)
BuildRequires:  pkgconfig(sailfishusermanager)
BuildRequires:  pkgconfig(sailfishpolicy)
BuildRequires:  cmake
BuildRequires:  sailfish-svg2png
Requires(post): systemd
Requires(postun): systemd
Requires:   %{name}-config
Requires:   xdg-desktop-portal

# for documentation:
BuildRequires:  qt5-qmake
BuildRequires:  qt5-tools
BuildRequires:  qt5-qttools-qthelp-devel
BuildRequires:  sailfish-qdoc-template
# depends in from qdocconf:
BuildRequires:  user-managerd-doc
BuildRequires:  nemo-qml-plugin-dbus-qt5-doc
# not available:
#BuildRequires:  nemo-qml-plugin-systemsettings-doc
#BuildRequires:  libprofile-doc

# PoC legacy, to be removed in due time:
Obsoletes: xdg-desktop-portal-amber <= %{version}
Conflicts: xdg-desktop-portal-amber


%description
%{summary}.
%if "%{?vendor}" == "chum"
Title: XDG Desktop Portal for Sailfish OS (PoC)
PackagedBy: nephros
DeveloperName: Sailfish OS Community
Categories:
 - System
Custom:
  Repo: %{url}
Links:
  Homepage: %{url}
%endif


%package config
Summary: User session environment configuration for %{name}
License: CC-BY-SA-4.0
BuildArch: noarch
# PoC legacy, to be removed in due time:
Obsoletes: xdg-desktop-portal-amber-sailfishos-config <= %{version}
Conflicts: xdg-desktop-portal-amber-sailfishos-config

%description config
%{summary}.

%package tools
Summary: Tools for %{name}
License: CC-BY-SA-4.0
BuildArch: noarch
Requires: collectd

%description tools
%{summary}.

%package ui
Summary: UI components for %{name}
License: Apache-2.0
# PoC legacy, to be removed in due time:
Obsoletes: xdg-desktop-portal-amber-qml <= %{version}
Conflicts: xdg-desktop-portal-amber-qml

%description ui
%{summary}.

%package doc
Summary:    Documentation for %{name}
License:    GFDL

%description doc
%{summary}.

%prep
%setup -q -n %{name}-%{version}

%build

%cmake

%make_build

pushd doc
# ignore build failures for doc
export QDOC_SHOW_INTERNAL=true
%qmake5 ||:
%make_build ||:
popd

%install
rm -rf %{buildroot}
%make_install
desktop-file-install --delete-original --dir %{buildroot}%{_datadir}/applications %{buildroot}%{_datadir}/applications/*.desktop

install -d %{buildroot}/%{_docdir}/%{name}/
install -m 644 doc/html/*.html %{buildroot}/%{_docdir}/%{name}/
install -m 644 doc/html/%{name}.index %{buildroot}/%{_docdir}/%{name}/
install -m 644 doc/%{name}.qch %{buildroot}/%{_docdir}/%{name}/

%preun
if [ $1 -eq 0 ]; then
systemctl-user stop %{name}.service
fi

%post
systemctl-user daemon-reload ||:

%postun
systemctl-user daemon-reload ||:

%post config
systemctl-user daemon-reload ||:

%postun config
systemctl-user daemon-reload ||:

%post ui
systemctl-user daemon-reload ||:

%postun ui
systemctl-user daemon-reload ||:

%files
%defattr(-,root,root,-)
# must be privileged to show Dialogs via Access and windowprompt:
%attr(2755,root,privileged) %{_libexecdir}/xdg-desktop-portal-sailfish
%{_datadir}/dbus-1/services/*.service
%exclude %{_datadir}/dbus-1/services/org.freedesktop.impl.portal.desktop.sailfish.ui.service
%{_userunitdir}/%{name}.service
%{_datadir}/xdg-desktop-portal/portals/sailfish.portal
# what is this installed for??
%{_datadir}/applications/org.freedesktop.impl.portal.desktop.sailfish.desktop

%files tools
%config %{_sysconfdir}/collectd.d/*.conf

%files config
%config %{_localstatedir}/lib/environment/nemo/*.conf
%config %{_datadir}/xdg-desktop-portal/*-portals.conf
%config %{_sysconfdir}/sailjail/permissions/XDGPortal*.permission
# overrides for the main XDP service
%config %{_userunitdir}/*.service.d/*.conf

%files ui
%{_libexecdir}/%{name}-ui
%{_datadir}/%{name}-ui/qml/*.qml
%{_datadir}/applications/xdg-desktop-portal-sailfish-ui.desktop
%{_datadir}/dbus-1/services/org.freedesktop.impl.portal.desktop.sailfish.ui.service
%{_datadir}/icons/*/*/apps/*


%files doc
%defattr(-,root,root,-)
%{_docdir}/%{name}/*
