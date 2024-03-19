/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt-project.org
 *
 * Copyright: 2016 Red Hat Inc
 * Copyright: 2016 Jan Grulich <jgrulich@redhat.com>
 * Copyright: 2021~ LXQt team
 * Authors:
 *   Palo Kisa <palo.kisa@gmail.com>
 * Copyright: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "desktopportal.h"
#include "access.h"
#include "account.h"
#include "email.h"
#include "filechooser.h"
#include "lockdown.h"
#include "settings.h"
#include "screenshot.h"
#include "wallpaper.h"

/*! \namespace Sailfish */
/*! \namespace Sailfish::XDP
    \brief Contains backend implementations of the XDG Desktop Portal specification for Sailfish OS.
    \inmodule XDGDesktopPortalSailfish

    See \l {XDG Desktop Portal Specification} for details.
*/

/*! \class Sailfish::XDP::AccessPortal
    \inmodule XDGDesktopPortalSailfish
    \brief Backend interface of the Access Portal

    See \l {XDG Desktop Portal Backend Specification} for details.
*/
/*! \class Sailfish::XDP::AccountPortal
    \inmodule XDGDesktopPortalSailfish
    \brief Backend interface of the Account Portal

    See \l {XDG Desktop Portal Backend Specification} for details.
*/
/*! \class Sailfish::XDP::EmailPortal
    \inmodule XDGDesktopPortalSailfish
    \brief Backend interface of the Email Portal

    See \l {XDG Desktop Portal Backend Specification} for details.
*/
/*! \class Sailfish::XDP::LockdownPortal
    \inmodule XDGDesktopPortalSailfish
    \brief Backend interface of the Lockdown Portal

    See \l {XDG Desktop Portal Backend Specification} for details.

    \note While the specification of all those properties is simple, the
    implementatio of the various methods uses widely different technologies to
    enable or disable a feature:

    \list
    \li Printing, Save To Disk, and Application Handlers: not supported at all, setting them has no effect.
    \li Location: reads and writes the \c location.conf init files, like Nemo QML Plugin Systemsettings
    \li Microphone: uses a DBus connection to PulseAudio to mute the default source
    \li Sound Output: uses Profile Daemon to set a silent or default profile
    \li Camera: uses Sailfish MDM Access Policies to enable or disable the use of the camera.
    \endlist

    \sa [sailfish-policy]{AccessPolicy}, [libprofile]{com.nokia.profiled}, [nemo-qml-plugin-systemsettings]{Nemo QML Plugin Systemsettings}
*/
/*! \class Sailfish::XDP::FileChooserPortal
    \inmodule XDGDesktopPortalSailfish
    \brief Backend interface of the FileChooser Portal

    See \l {XDG Desktop Portal Backend Specification} for details.
*/
/*! \class Sailfish::XDP::ScreenshotPortal
    \inmodule XDGDesktopPortalSailfish
    \brief Backend interface of the Screenshot Portal

    See \l {XDG Desktop Portal Backend Specification} for details.
*/
/*! \class Sailfish::XDP::SettingsPortal
    \inmodule XDGDesktopPortalSailfish
    \brief Backend interface of the Settings Portal

    See \l {XDG Desktop Portal Backend Specification} for details.

    Features restrieving the properties from the \c org.freedesktop.appearance namespace
    as well as a custom namespace called \c org.sailfishos.desktop, and optionally others.

    See \l{SettingsPortalNamespaces}{Supported Namespaces} for details.
*/
/*! \class Sailfish::XDP::WallpaperPortal
    \inmodule XDGDesktopPortalSailfish
    \brief Backend interface of the Wallpaper Portal

    See \l {XDG Desktop Portal Backend Specification} for details.
*/

namespace Sailfish {
namespace XDP {
    DesktopPortal::DesktopPortal(QObject *parent)
        : QObject(parent)
        , m_access(new AccessPortal(this))
        , m_account(new AccountPortal(this))
        , m_email(new EmailPortal(this))
        , m_filechooser(new FileChooserPortal(this))
        , m_lockdown(new LockdownPortal(this))
        , m_screenshot(new ScreenshotPortal(this))
        , m_settings(new SettingsPortal(this))
        , m_wallpaper(new WallpaperPortal(this))
    {
        //m_access = new AccessPortal(this);
        //m_screenshot = new ScreenshotPortal(this);
        //m_wallpaper = new WallpaperPortal(this);

    }
    DesktopPortal::~DesktopPortal()
    {
    }
}
}

