Name:       xdg-desktop-portal-amber
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
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(sailfishapp)
BuildRequires:  cmake
Requires:   %{name}-sailfishos-config
Requires:   xdg-desktop-portal

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


%package sailfishos-config
Summary: User session environment configuration for %{name}
BuildArch: noarch

%description sailfishos-config
%{summary}.

%package qml
Summary: UI components for %{name}
BuildArch: noarch

%description qml
%{summary}.


%package devel
Summary:    Header files and library symbolic links for %{name}
Requires:   %{name} = %{version}-%{release}

%description devel
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
%attr(2755,root,privileged) %{_libexecdir}/xdg-desktop-portal-amber
%{_datadir}/dbus-1/services/*.service
%{_userunitdir}/%{name}.service
%{_datadir}/xdg-desktop-portal/portals/amber.portal
# what is this installed for??
%exclude %{_datadir}/applications/*.desktop

%files sailfishos-config
%{_userunitdir}/xdg-desktop-portal-pre.service
%config %{_localstatedir}/lib/environment/amber/*.conf
%config %{_datadir}/xdg-desktop-portal/*-portals.conf
%config %{_sysconfdir}/sailjail/permissions/XDGPortal*.permission

%files qml
%{_bindir}/%{name}-ui
%{_datadir}/%{name}/*.qml
%{_datadir}/applications/*.desktop

%files devel
%defattr(-,root,root,-)
#%%{_datadir}/pkgconfig/*pc
