Name:       xdg-desktop-portal-amber
Summary:    XDG Desktop Portal for Sailfish OS
Version:    1.0.0
Release:    0
Group:      Applications
#TODO:
License:    LGPLv2+ and LGPLv3+
#URL:        https://invent.kde.org/plasma/xdg-desktop-portal-kde
Source0:    %{name}-%{version}.tar.bz2
BuildRequires:  pkgconfig(systemd)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  cmake

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


%package devel
Summary:    Header files and library symbolic links for %{name}
Group:      Development
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


%files
%defattr(-,root,root,-)
%{_libexecdir}/*
%{_localstatedir}/lib/environment/*.conf
%{_userunitdir}/*
%{_datadir}/dbus-1/*
%{_datadir}/%{name}
# >> files
# << files

%files devel
%defattr(-,root,root,-)
%{_datadir}/pkgconfig/*pc
# >> files devel
# << files devel
