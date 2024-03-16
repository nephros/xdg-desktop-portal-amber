Name:       xdg-desktop-portal-sailfish
Summary:    XDG Desktop Portal for Sailfish OS
Version:    1.0.0
Release:    0
License:    LGPLv2+ and LGPLv3+
#URL:        https://invent.kde.org/plasma/xdg-desktop-portal-kde
Source0:    %{name}-%{version}.tar.bz2
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(systemd)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  pkgconfig(sailfishapp)
BuildRequires:  cmake
BuildRequires:  sailfish-svg2png
Requires:   %{name}-config
Requires:   xdg-desktop-portal

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

%package ui
Summary: UI components for %{name}
License: Apache-2.0
# PoC legacy, to be removed in due time:
Obsoletes: xdg-desktop-portal-amber-qml <= %{version}
Conflicts: xdg-desktop-portal-amber-qml

%description ui
%{summary}.

%prep
%setup -q -n %{name}-%{version}

%build

%cmake

%make_build


%install
rm -rf %{buildroot}
%make_install
desktop-file-install --delete-original --dir %{buildroot}%{_datadir}/applications %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
# must be privileged to show Dialogs via Access and windowprompt:
%attr(2755,root,privileged) %{_libexecdir}/xdg-desktop-portal-sailfish
%{_datadir}/dbus-1/services/*.service
%exclude %{_datadir}/dbus-1/services/org.freedesktop.impl.portal.desktop.sailfish.ui.service
%{_userunitdir}/%{name}.service
%{_datadir}/xdg-desktop-portal/portals/sailfish.portal
# what is this installed for??
%exclude %{_datadir}/applications/*.desktop

%files config
%config %{_localstatedir}/lib/environment/nemo/*.conf
%config %{_datadir}/xdg-desktop-portal/*-portals.conf
%config %{_sysconfdir}/sailjail/permissions/XDGPortal*.permission
# overrides
%config %{_userunitdir}/*.service.d/*.conf

%files ui
%{_bindir}/%{name}-ui
%{_datadir}/%{name}-ui/qml/*.qml
%{_datadir}/applications/xdg-desktop-portal-sailfish-ui.desktop
%{_datadir}/dbus-1/services/org.freedesktop.impl.portal.desktop.sailfish.ui.service
%{_datadir}/icons/*/*/apps/*


