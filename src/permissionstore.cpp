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

Q_LOGGING_CATEGORY(XdgDesktopPortalAmber, "xdp-amber-permissionstore")

namespace Amber {
PermissionStorePortal::PermissionStorePortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalAmber) << "Desktop portal service: PermissionStore";
}

PermissionStorePortal::~PermissionStorePortal()
{
}

uint PermissionStorePortal::List(const QString &table, QStringList &results)
{
    qCDebug(XdgDesktopPortalAmber) << "Store called with parameters:";
    qCDebug(XdgDesktopPortalAmber) << "    table: " << table;

    if (table.isEmpty()) {
        qCDebug(XdgDesktopPortalAmber) << "Empty table!";
        results.append("");
        return 0;
    }

    qCDebug(XdgDesktopPortalAmber) << "Asking MDM for list";
    QMetaEnum metaEnum = QMetaEnum::fromType<Sailfish::PolicyValue::PolicyType>();

    for (int i=0; i < metaEnum.keyCount(); ++i) {
        results.append(metaEnum.key(i));
    }

    if (!results.isEmpty()) {
        qCDebug(XdgDesktopPortalAmber) << "Returning:" << results;
        return 0;
    }

    return 1;
}
} // namespace Amber
