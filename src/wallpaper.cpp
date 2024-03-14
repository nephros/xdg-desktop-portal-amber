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

Q_LOGGING_CATEGORY(XdgDesktopPortalSailfishWallpaper, "xdp-sailfish-wallpaper")

namespace Sailfish {
WallpaperPortal::WallpaperPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalSailfishWallpaper) << "Desktop portal service: Wallpaper";
}

uint WallpaperPortal::SetWallpaperURI(const QDBusObjectPath &handle,
                                  const QString &app_id,
                                  const QString &parent_window,
                                  const QString &uri,
                                  const QVariantMap &options,
                                  uint &result)
{
    qCDebug(XdgDesktopPortalSailfishWallpaper) << "Wallpaper called with parameters:";
    qCDebug(XdgDesktopPortalSailfishWallpaper) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalSailfishWallpaper) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalSailfishWallpaper) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalSailfishWallpaper) << "    uri: " << uri;
    qCDebug(XdgDesktopPortalSailfishWallpaper) << "    options: " << options;

    if (options.contains(QStringLiteral("set-on"))) { // background, lockscreen or both.
        if (!options.value(QStringLiteral("set-on")).toString().compare(QStringLiteral("background"), Qt::CaseInsensitive)) {
            qCDebug(XdgDesktopPortalSailfishWallpaper) << "Setting wallpaper for Lock Screen is not supported.";
        }
        if (options.value(QStringLiteral("set-on")).toString().compare(QStringLiteral("lockscreen"), Qt::CaseInsensitive)) {
            return 1;
        }
    }

    QDBusMessage msg;
    if (options.contains(QStringLiteral("show-preview"))) {
        if (options.value(QStringLiteral("show-preview")).toBool()) {
            qCDebug(XdgDesktopPortalSailfishWallpaper) << "Was asked to show a preview, opening Gallery to create an Ambience";
            msg = QDBusMessage::createMethodCall(
                    QStringLiteral("com.jolla.gallery"),
                    QStringLiteral("/com/jolla/gallery/ui"),
                    QStringLiteral("com.jolla.gallery.ui"),
                    QStringLiteral("openFile"));
        } else {
            qCDebug(XdgDesktopPortalSailfishWallpaper) << "Asking Ambience daemon to set wallpaper";
            msg = QDBusMessage::createMethodCall(
                    QStringLiteral("com.jolla.ambienced"),
                    QStringLiteral("/com/jolla/ambienced"),
                    QStringLiteral("com.jolla.ambienced"),
                    QStringLiteral("setAmbience"));
        }
    }

    QList<QVariant> args;
    args.append(uri);
    msg.setArguments(args);

    QDBusPendingReply<QString> pcall = QDBusConnection::sessionBus().call(msg);
    pcall.waitForFinished();
    if (pcall.isValid()) {
        qCDebug(XdgDesktopPortalSailfishWallpaper) << "Success";
        result = 0;
        return 0;
    }
    qCDebug(XdgDesktopPortalSailfishWallpaper) << "Wallpaper failed:" << pcall.error().name() << pcall.error().message();
    result = 1;
    return 1;
}
} // namespace Sailfish
