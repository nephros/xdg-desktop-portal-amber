/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 */

#ifndef XDG_DESKTOP_PORTAL_SAILFISH_ACCOUNT_H
#define XDG_DESKTOP_PORTAL_SAILFISH_ACCOUNT_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusMessage>

namespace Sailfish {
namespace XDP {
class AccountPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Account")

public:
    explicit AccountPortal(QObject *parent);
    //~AccountPortal() override;

public Q_SLOTS:
    uint GetUserInformation(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QVariantMap &options,
                      QVariantMap &results,
                      const QDBusMessage &message
                      );
};
}
}
#endif // XDG_DESKTOP_PORTAL_SAILFISH_EMAIL_H
