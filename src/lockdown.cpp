/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 */

#include "lockdown.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QUrl>
#include <QLoggingCategory>
#include <accesspolicy.h>

Q_LOGGING_CATEGORY(XDPortalSailfishLockdown, "xdp-sailfish-lockdown")

const char* PROFILE_MUTED_NAME = "silent";
const char* PROFILE_UNMUTED_NAME = "general";

namespace Sailfish {
namespace XDP {
LockdownPortal::LockdownPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XDPortalSailfishLockdown) << "Desktop portal service: Lockdown";
    connect(m_access, cameraEnabledChanged, this, cameraEnabledChanged);
    connect(m_access, microphoneEnabledChanged, this, microphoneEnabledChanged);
    connect(m_access, locationSettingsEnabledChanged, this, locationSettingsEnabledChanged);
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

    pcall.waitForFinished();
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

    QList<QVariant> args;
    if (silent) {
        args.append(PROFILE_MUTED_NAME);
    } else {
        args.append(PROFILE_UNMUTED_NAME);
    }
    msg.setArguments(args);
    QDBusConnection::sessionBus().call(msg, QDBus::NoBlock);
}

}
}
