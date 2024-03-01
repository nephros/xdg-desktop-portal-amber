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
    Q_PROPERTY(uint version READ version CONSTANT)
public:
    explicit PermissionStorePortal(QObject *parent);
    ~PermissionStorePortal() override;

    uint version() const { return 1; }
public Q_SLOTS:

    uint List( const QString &table, QStringList &results);
    void Lookup( const QString &table,
                 const QString &id,
                 QList<QVariantMap> &permissions,
                 QVariant &data
                 );
    uint GetPermission( const QString &table,
                 const QString &id,
                 const QString &app,
                 QStringList &permissions
                 );
    void Set( const QString &table,
                 const bool &create,
                 const QString &id,
                 const QList<QVariantMap> &permissions,
                 const QVariant &data
                 );
    void SetValue( const QString &table,
                 const bool &create,
                 const QString &id,
                 const QVariant &data
                 );
    void SetPermission( const QString &table,
                 const bool &create,
                 const QString &id,
                 const QString &app,
                 const QStringList &permissions
                 );
    /* added in version 2 of the interface:
    uint DeletePermission( const QString &table,
                 const QString &id,
                 const QString &app,
                 );
    */
    void Delete( const QString &table,
                 const QString &id
               );

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
