/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 */

#ifndef XDG_DESKTOP_PORTAL_SAILFISH_EMAIL_H
#define XDG_DESKTOP_PORTAL_SAILFISH_EMAIL_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

namespace Sailfish {
namespace XDP {
class EmailPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Email")

public:
    explicit EmailPortal(QObject *parent);
    ~EmailPortal() override;

public Q_SLOTS:
    uint ComposeEmail(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QVariantMap &options,
                      QVariantMap &results);
};
}
}
#endif // XDG_DESKTOP_PORTAL_SAILFISH_EMAIL_H
