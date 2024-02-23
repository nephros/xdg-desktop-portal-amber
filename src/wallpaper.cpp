// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
// SPDX-FileCopyrightText: 2024 Sailfish OS Community
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "wallpaper.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QUrl>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(XdgDesktopPortalAmberWallpaper, "xdp-amber-wallpaper")

namespace Amber {
WallpaperPortal::WallpaperPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalAmberWallpaper) << "Wallpaper init";
}

uint WallpaperPortal::SetWallpaperURI(const QDBusObjectPath &handle,
                                  const QString &app_id,
                                  const QString &parent_window,
                                  const QString &uri,
                                  const QVariantMap &options,
                                  uint &result)
{
    qCDebug(XdgDesktopPortalAmberWallpaper) << "Start screenshot";
    if (!options.isEmpty()) {
        qCDebug(XdgDesktopPortalAmberWallpaper) << "Wallpaper options not implemented.";
    }
    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("com.jolla.ambienced"),
                                                      QStringLiteral("/com/jolla/ambienced"),
                                                      QStringLiteral("com.jolla.ambienced"),
                                                      QStringLiteral("createAmbience"));
    QList<QVariant> args;
    args.append(uri);
    msg.setArguments(args);

    QDBusPendingReply<QString> pcall = QDBusConnection::sessionBus().call(msg);
    pcall.waitForFinished();
    if (pcall.isValid()) {
        qCDebug(XdgDesktopPortalAmberWallpaper) << "Success";
        result = 0;
        return 0;
    }
    qCDebug(XdgDesktopPortalAmberWallpaper) << "Wallpaper failed";
    result = 1;
    return 1;
}
} // namespace Amber
