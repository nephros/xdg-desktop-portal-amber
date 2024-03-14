// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
// SPDX-FileCopyrightText: 2024 Sailfish OS Community
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QDBusObjectPath>
#include <QDBusAbstractAdaptor>
#include <qobjectdefs.h>
#include <QDBusInterface>

namespace Sailfish {
    class WallpaperPortal : public QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Wallpaper")
        Q_PROPERTY(uint version READ version CONSTANT)

    public:
        explicit WallpaperPortal(QObject *parent);
        ~WallpaperPortal() = default;

        uint version() const { return 1; }
    public slots:
        uint SetWallpaperURI(const QDBusObjectPath &handle,
                        const QString &app_id,
                        const QString &parent_window,
                        const QString &uri,
                        const QVariantMap &options,
                        uint &result);
    };
}
