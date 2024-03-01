/*
 * SPDX-FileCopyrightText: 2017 Red Hat Inc
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2017 Jan Grulich <jgrulich@redhat.com>
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 */

#ifndef XDG_DESKTOP_PORTAL_AMBER_ACCESS_H
#define XDG_DESKTOP_PORTAL_AMBER_ACCESS_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

namespace Amber {
class AccessPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Access")

public:
    explicit AccessPortal(QObject *parent);
    ~AccessPortal() override;

public Q_SLOTS:
    uint AccessDialog(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QString &title,
                      const QString &subtitle,
                      const QString &body,
                      const QVariantMap &options,
                      QVariantMap &results);
};
}
#endif // XDG_DESKTOP_PORTAL_AMBER_ACCESS_H
