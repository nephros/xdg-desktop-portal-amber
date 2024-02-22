// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
// SPDX-FileCopyrightText: 2024 Sailfish OS Community
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QDBusObjectPath>
#include <QDBusAbstractAdaptor>
#include <qobjectdefs.h>
#include <QDBusInterface>

namespace Amber {
    class WallpaperPortal : public QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Wallpaper")

    public:
        explicit WallpaperPortal(QObject *parent);
        ~WallpaperPortal() = default;

    public slots:
        uint SetWallpaperURI(const QDBusObjectPath &handle,
                        const QString &app_id,
                        const QString &parent_window,
                        const QString &uri,
                        const QVariantMap &options,
                        uint &result);
    };
}
