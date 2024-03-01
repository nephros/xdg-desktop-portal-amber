/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 */

#ifndef XDG_DESKTOP_PORTAL_AMBER_PERMISSIONSTORE_H
#define XDG_DESKTOP_PORTAL_AMBER_PERMISSIONSTORE_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

namespace Amber {
class PermissionStorePortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.PermissionStore")
public:
    explicit PermissionStorePortal(QObject *parent);
    ~PermissionStorePortal() override;

public Q_SLOTS:
    uint List( const QString &table, QStringList &results);

signals:
    void Changed( const QString &table,
                  const QString &id,
                  const bool &deleted,
                  const QVariant &data,
                  const QVariantMap &permissions
                  );
};
}
#endif // XDG_DESKTOP_PORTAL_AMBER_PERMISSIONSTORE_H
