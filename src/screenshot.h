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
    class ScreenshotPortal : public QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Screenshot")
        Q_PROPERTY(uint version READ version CONSTANT)

    public:
        uint version() const { return 2; }

        struct ColorRGB
        {
            double red;
            double green;
            double blue;
        };
        explicit ScreenshotPortal(QObject *parent);
        ~ScreenshotPortal() = default;

    public slots:
        uint PickColor(const QDBusObjectPath &handle,
                       const QString &app_id,
                       const QString &parent_window,
                       const QVariantMap &options,
                       QVariantMap &results);
        uint Screenshot(const QDBusObjectPath &handle,
                        const QString &app_id,
                        const QString &parent_window,
                        const QVariantMap &options,
                        QVariantMap &results);
    };
}
