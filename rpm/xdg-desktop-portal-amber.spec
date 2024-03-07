Name:       xdg-desktop-portal-amber
Summary:    XDG Desktop Portal for Sailfish OS
Version:    1.0.0
Release:    0
License:    LGPLv2+ and LGPLv3+
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
Requires:   %{name}-sailfishos-config
Requires:   xdg-desktop-portal

# for documentation:
BuildRequires:  qt5-qmake
BuildRequires:  qt5-tools
BuildRequires:  qt5-qttools-qthelp-devel
BuildRequires:  sailfish-qdoc-template

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
License: CC-BY-SA-4.0
BuildArch: noarch

%description sailfishos-config
%{summary}.

%package qml
Summary: UI components for %{name}
License: Apache-2.0

%description qml
%{summary}.


%package devel
Summary:    Header files and library symbolic links for %{name}
Requires:   %{name} = %{version}-%{release}

%description devel
%{summary}.


%package doc
Summary:    Documentation for %{name}
License:    GFDL

%description devel
%{summary}.

%prep
%setup -q -n %{name}-%{version}

%build

%cmake

%make_build

pushd doc
# ignore build failures for doc
%qmake5 ||:
%make_build ||:
popd

%install
rm -rf %{buildroot}
%make_install
desktop-file-install --delete-original --dir %{buildroot}%{_datadir}/applications %{buildroot}%{_datadir}/applications/*.desktop

install -m 644 doc/html/*.html %{buildroot}/%{_docdir}/%{name}/
install -m 644 doc/html/%{name}.index %{buildroot}/%{_docdir}/%{name}/
install -m 644 doc/%{name}.qch %{buildroot}/%{_docdir}/%{name}/

%files
%defattr(-,root,root,-)
# must be privileged to show Dialogs via Access and windowprompt:
%attr(2755,root,privileged) %{_libexecdir}/xdg-desktop-portal-amber
%{_datadir}/dbus-1/services/*.service
%exclude %{_datadir}/dbus-1/services/org.freedesktop.impl.portal.desktop.amber.ui.service
%{_userunitdir}/%{name}.service
%{_datadir}/xdg-desktop-portal/portals/amber.portal
# what is this installed for??
%{_datadir}/applications/org.freedesktop.impl.portal.desktop.amber.desktop

%files sailfishos-config
%{_userunitdir}/xdg-desktop-portal-pre.service
%config %{_localstatedir}/lib/environment/amber/*.conf
%config %{_datadir}/xdg-desktop-portal/*-portals.conf
%config %{_sysconfdir}/sailjail/permissions/XDGPortal*.permission

%files qml
%{_bindir}/%{name}-ui
%{_datadir}/%{name}-ui/qml/*.qml
%{_datadir}/applications/xdg-desktop-portal-amber-ui.desktop
%{_datadir}/dbus-1/services/org.freedesktop.impl.portal.desktop.amber.ui.service

%files devel
%defattr(-,root,root,-)
#%%{_datadir}/pkgconfig/*pc

%files doc
%defattr(-,root,root,-)
%{_docdir}/%{name}/*
