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
    qCDebug(XdgDesktopPortalSailfishLockdown) << "Desktop portal service: Lockdown";
    m_policy = new AccessPolicy(this);
    QObject::connect(m_policy, SIGNAL(cameraEnabledChanged()), this, SLOT(cameraDisabledChanged()));
    QObject::connect(m_policy, SIGNAL(microphoneEnabledChanged()), this, SLOT(microphoneDisabledChanged()));
    QObject::connect(m_policy, SIGNAL(locationSettingsEnabledChanged()), this, SLOT(locationSettingsDisabledChanged()));
}

bool LockdownPortal::muted() const
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
        qCDebug(XdgDesktopPortalSailfishLockdown) << "Read profile value:" << pcall.value();
        return (pcall.value() == PROFILE_MUTED_NAME);
    }
}

void LockdownPortal::mute(const bool &silent) const
{
    qCDebug(XdgDesktopPortalSailfishLockdown) << "Setting mute:" << silent;
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

bool LockdownPortal::disable_camera() const
{
    return m_policy->cameraEnabled();
};
bool LockdownPortal::disable_microphone() const
{
    return m_policy->microphoneEnabled();
};
bool LockdownPortal::disable_location() const
{
    return m_policy->locationSettingsEnabled();
};

void LockdownPortal::setLocationSettingsDisabled(const bool &disable) const
{
    m_policy->setLocationSettingsEnabled(!disable);
};
void LockdownPortal::setMicrophoneDisabled(const bool &disable) const
{
    m_policy->setMicrophoneEnabled(!disable);
};
void LockdownPortal::setCameraDisabled(const bool &disable) const
{
    m_policy->setCameraEnabled(!disable);
};

}
}
