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

#include <QApplication>
#include <QDBusConnection>
#include <QLoggingCategory>

#include "desktopportal.h"

Q_LOGGING_CATEGORY(XdgDesktopPortalAmber, "xdp-amber")

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_DisableSessionManager);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app{argc, argv};
    app.setApplicationName(QStringLiteral("xdg-desktop-portal-amber"));
    app.setQuitOnLastWindowClosed(false);

    QDBusConnection sessionBus = QDBusConnection::sessionBus();

    if (sessionBus.registerService(QStringLiteral("org.freedesktop.impl.portal.desktop.amber"))) {
        const auto desktopPortal = new Amber::DesktopPortal{&a};
        if (sessionBus.registerObject(QStringLiteral("/org/freedesktop/portal/desktop"), desktopPortal, QDBusConnection::ExportAdaptors)) {
            qCDebug(XdgDesktopPortalAmber) << "Desktop portal registered successfully";
        } else {
            qCDebug(XdgDesktopPortalAmber) << "Failed to register desktop portal";
        }
    } else {
        qCDebug(XdgDesktopPortalAber) << "Failed to register org.freedesktop.impl.portal.desktop.amber service";
        return 1;
    }

    return app.exec();
}
