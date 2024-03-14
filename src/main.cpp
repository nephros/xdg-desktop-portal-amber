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

#include <QCoreApplication>
#include <QDBusConnection>
#include <QLoggingCategory>

#include <unistd.h>
#include <sys/types.h>
#include <grp.h>

#include "desktopportal.h"

Q_LOGGING_CATEGORY(XdgDesktopPortalSailfish, "xdp-sailfish")

const char* dbusName   = "org.freedesktop.impl.portal.desktop.sailfish";
const char* dbusObject = "/org/freedesktop/portal/desktop";

int main(int argc, char *argv[])
{
    QCoreApplication app{argc, argv};
    app.setApplicationName(QStringLiteral("xdg-desktop-portal-sailfish"));

    // determine GID, check prvileged group:
    struct group *grp = getgrnam("privileged"); /* don't free, see getgrnam() for details */
    if ( getgid() != grp->gr_gid ) {
        qCWarning(XdgDesktopPortalSailfish) << "Not running privileged, some features will not work.";
    }

    QDBusConnection sessionBus = QDBusConnection::sessionBus();

    qCDebug(XdgDesktopPortalSailfish) << "Attempting to register Desktop portal:" << dbusName << dbusObject;
    if (sessionBus.registerService(dbusName)) {
        const auto desktopPortal = new Sailfish::DesktopPortal{&app};
        qCDebug(XdgDesktopPortalSailfish) << "Desktop portal bus registered successfully";
        if (sessionBus.registerObject(dbusObject, desktopPortal, QDBusConnection::ExportAdaptors)) {
            qCDebug(XdgDesktopPortalSailfish) << "Desktop portal object registered successfully";
        } else {
            qCDebug(XdgDesktopPortalSailfish) << "Failed to register object";
        }
    } else {
        qCDebug(XdgDesktopPortalSailfish) << "Failed to register service.";
        return 1;
    }

    return app.exec();
}
