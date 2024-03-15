// SPDX-FileCopyrightText: 2024 Sailfish OS Community
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "account.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMessage>
#include <QLoggingCategory>
#include <sailfishusermanagerinterface.h>

Q_LOGGING_CATEGORY(XdgDesktopPortalSailfishAccount, "xdp-sailfish-email")

namespace Sailfish {
namespace XDP {
AccountPortal::AccountPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalSailfishAccount) << "Desktop portal service: Account";
}

/*
AccountPortal::~AccountPortal()
{
}
*/

/*! \fn uint Sailfish::XDP::AccountPortal::GetUserInformation(const QDBusObjectPath &handle, const QString &app_id, const QString &parent_window, const QVariantMap &options, QVariantMap &results)

     Retrieves basic user information

     See the \l{XDG Desktop Portal Specification} for possible values of \a options.
     See the \l{XDG Desktop Portal Backend Specification} for the meaning of \a handle, \a app_id, \a parent_window, \a results.

     \sa Sailfish User Manager Daemon
*/
uint AccountPortal::GetUserInformation(const QDBusObjectPath &handle,
                                  const QString &app_id,
                                  const QString &parent_window,
                                  const QVariantMap &options,
                                  QVariantMap &results,
                                  const QDBusMessage &message
                                  )
{
    qCDebug(XdgDesktopPortalSailfishAccount) << "Account called with parameters:";
    qCDebug(XdgDesktopPortalSailfishAccount) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalSailfishAccount) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalSailfishAccount) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalSailfishAccount) << "    options: " << options;

    if (!options.isEmpty()) {
        qCDebug(XdgDesktopPortalSailfishAccount) << "Account options not supported.";
    }

    message.setDelayedReply(true);

    QDBusInterface iface( QStringLiteral("org.sailfishos.usermanager"), QStringLiteral("/"), QStringLiteral("org.sailfishos.usermanager"));

    // get user id
    uint userid;
    QDBusReply<uint> ucall = iface.call("currentUser");
    if (ucall.isValid()) {
        qCDebug(XdgDesktopPortalSailfishAccount) << "Success";
        userid = ucall.value();
    }

    // get list of users
    QList<SailfishUserManagerEntry> users;
    QDBusReply<QList<SailfishUserManagerEntry>> lcall = iface.call("users");
    if (lcall.isValid()) {
        qCDebug(XdgDesktopPortalSailfishAccount) << "Success";
        users = lcall.value();
    }

    // find our user in the list:
    QString username;
    for (SailfishUserManagerEntry u : users) {
        if (u.uid == userid)
            username = u.name;
    }


    // send reply
    results.insert("id", QString(userid));
    results.insert("name", username);
    results.insert("image", QString());

    QDBusMessage reply;
    if (userid >= 0) {
        reply = message.createReply();
    } else {
        reply = message.createErrorReply(QDBusError::Failed, QStringLiteral("Not found."));
    }
    QDBusConnection::sessionBus().send(reply);
    return 0;
}
} // namespace XDP
} // namespace Sailfish
