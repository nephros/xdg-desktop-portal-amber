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
    qCDebug(XdgDesktopPortalAmberWallpaper) << "Desktop portal service: Wallpaper";
}

uint WallpaperPortal::SetWallpaperURI(const QDBusObjectPath &handle,
                                  const QString &app_id,
                                  const QString &parent_window,
                                  const QString &uri,
                                  const QVariantMap &options,
                                  uint &result)
{
    qCDebug(XdgDesktopPortalAmberWallpaper) << "Wallpaper called with parameters:";
    qCDebug(XdgDesktopPortalAmberWallpaper) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalAmberWallpaper) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalAmberWallpaper) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalAmberWallpaper) << "    uri: " << uri;
    qCDebug(XdgDesktopPortalAmberWallpaper) << "    options: " << options;

    if (!options.isEmpty()) {
        qCDebug(XdgDesktopPortalAmberWallpaper) << "Wallpaper options not supported.";
    }
    if (options.contains(QStringLiteral("show-preview"))) {
        if (options.value(QStringLiteral("show-preview")).toBool())
            qCDebug(XdgDesktopPortalAmberWallpaper) << "Was asked to show a preview";
    }
    if (options.contains(QStringLiteral("set-on"))) {
        qCDebug(XdgDesktopPortalAmberWallpaper) << "Was asked to set wallpaper for" << options.value(QStringLiteral("set-on")).toString();
    }
    qCDebug(XdgDesktopPortalAmberWallpaper) << "Asking Ambience daemon to set wallpaper";
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
    qCDebug(XdgDesktopPortalAmberWallpaper) << "Wallpaper failed:" << pcall.error().name() << pcall.error().message();
    result = 1;
    return 1;
}
} // namespace Amber
