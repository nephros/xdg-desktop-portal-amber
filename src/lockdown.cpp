// SPDX-FileCopyrightText: 2024 Sailfish OS Community
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "lockdown.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QUrl>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(XdgDesktopPortalAmberLockdown, "xdp-amber-lockdown")

const char* PROFILE_MUTED_NAME = "silent";
const char* PROFILE_UNMUTED_NAME = "general";

namespace Amber {
LockdownPortal::LockdownPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalAmberLockdown) << "Desktop portal service: Lockdown";
}

bool LockdownPortal::getGPS() const
{
}
void LockdownPortal::setGPS(const bool &off) const
{
}
bool LockdownPortal::getMute() const
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
            QStringLiteral("com.nokia.profiled"),
            QStringLiteral("/com/nokia/profiled"),
            QStringLiteral("com.nokia.profiled"),
            QStringLiteral("get_profile")
            );
    QDBusPendingReply<QString> pcall = QDBusConnection::sessionBus().call(msg);

    if (pcall.isValid()) {
        return (pcall.value() == PROFILE_MUTED_NAME);
    }
}

void LockdownPortal::setMute(const bool &silent) const
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
            QStringLiteral("com.nokia.profiled"),
            QStringLiteral("/com/nokia/profiled"),
            QStringLiteral("com.nokia.profiled"),
            QStringLiteral("set_profile")
            );

    QString profile;
    if (silent) {
        msg.setArguments(PROFILE_MUTED_NAME);
    } else {
        msg.setArguments(PROFILE_UNMUTED_NAME);
    }
    QDBusConnection::sessionBus().call(msg);
}

}
