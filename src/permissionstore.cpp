// SPDX-FileCopyrightText: 2024 Sailfish OS Community
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "permissionstore.h"

// Sailfish MDM
#include <libsailfishmdm/mdm-sysinfo.h>
#include <libsailfishpolicy/policyvalue.h>

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QMetaEnum>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(XdgDesktopPortalAmberPermissionStore, "xdp-amber-permissionstore")

namespace Amber {
PermissionStorePortal::PermissionStorePortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalAmberPermissionStore) << "Desktop portal service: PermissionStore";
}

PermissionStorePortal::~PermissionStorePortal()
{
}

uint PermissionStorePortal::List(const QString &table, QStringList &results)
{
    qCDebug(XdgDesktopPortalAmberPermissionStore) << "Store called with parameters:";
    qCDebug(XdgDesktopPortalAmberPermissionStore) << "    table: " << table;

    if (table.isEmpty()) {
        qCDebug(XdgDesktopPortalAmberPermissionStore) << "Empty table!";
        results.append("");
        return 0;
    }

    qCDebug(XdgDesktopPortalAmberPermissionStore) << "Asking MDM for list";
    QMetaEnum metaEnum = QMetaEnum::fromType<Sailfish::PolicyValue::PolicyType>();

    for (int i=0; i < metaEnum.keyCount(); ++i) {
        results.append(metaEnum.key(i));
    }

    if (!results.isEmpty()) {
        qCDebug(XdgDesktopPortalAmberPermissionStore) << "Returning:" << results;
        return 0;
    }

    return 1;
}

void PermissionStorePortal::Lookup( const QString &table,
             const QString &id,
             QList<QVariantMap> &permissions,
             QVariant &data
             )
{
    qCDebug(XdgDesktopPortalAmberPermissionStore) << "This method is not implemented";
}
uint PermissionStorePortal::GetPermission( const QString &table,
             const QString &id,
             const QString &app,
             QStringList &permissions
             )
{
    qCDebug(XdgDesktopPortalAmberPermissionStore) << "This method is not implemented";
    return 1;
}

void PermissionStorePortal::Set( const QString &table,
             const bool &create,
             const QString &id,
             const QList<QVariantMap> &permissions,
             const QVariant &data
             )
{
    qCDebug(XdgDesktopPortalAmberPermissionStore) << "This method is not implemented";
}

void PermissionStorePortal::SetValue( const QString &table,
             const bool &create,
             const QString &id,
             const QVariant &data
             )
{
    qCDebug(XdgDesktopPortalAmberPermissionStore) << "This method is not implemented";
}

void PermissionStorePortal::SetPermission( const QString &table,
             const bool &create,
             const QString &id,
             const QString &app,
             const QStringList &permissions
             )
{
    qCDebug(XdgDesktopPortalAmberPermissionStore) << "This method is not implemented";
}

/* added in version 2 of the interface:
uint PermissionStorePortal::DeletePermission( const QString &table,
             const QString &id,
             const QString &app,
             )
{
    qCDebug(XdgDesktopPortalAmberPermissionStore) << "This method is not implemented";
    return 1;
}
*/

void PermissionStorePortal::Delete( const QString &table, const QString &id)
{
    qCDebug(XdgDesktopPortalAmberPermissionStore) << "This method is not implemented";
}

} // namespace Amber
