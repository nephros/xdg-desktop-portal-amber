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
} // namespace Amber
