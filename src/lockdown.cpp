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
#include <QDBusReply>
#include <QLoggingCategory>
#include <accesspolicy.h>
#include <QFile>
#include <QSettings>

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
    m_profiled = new QDBusInterface( QStringLiteral("com.nokia.profiled"), QStringLiteral("/com/nokia/profiled"), QStringLiteral("com.nokia.profiled"));
}

bool LockdownPortal::muted() const
{
    QDBusReply<QString> pcall = m_profiled->call(QStringLiteral("get_profile"));

    if (pcall.isValid()) {
        qCDebug(XdgDesktopPortalSailfishLockdown) << "Read profile value:" << pcall.value();
        return (pcall.value() == PROFILE_MUTED_NAME);
    }
    // what else?
    return false;
}

void LockdownPortal::mute(const bool &silent) const
{
    qCDebug(XdgDesktopPortalSailfishLockdown) << "Setting mute:" << silent;
    if (silent) {
         m_profiled->call(QStringLiteral("set_profile"), PROFILE_MUTED_NAME);
    } else {
        m_profiled->call(QStringLiteral("set_profile"), PROFILE_UNMUTED_NAME);
    }
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
    //m_policy->setLocationSettingsEnabled(!disable);
    setLocationEnabled(!disable);
};
void LockdownPortal::setMicrophoneDisabled(const bool &disable) const
{
    m_policy->setMicrophoneEnabled(!disable);
};
void LockdownPortal::setCameraDisabled(const bool &disable) const
{
    m_policy->setCameraEnabled(!disable);
};

void LockdownPortal::setMicMutePulse(const bool &muted) const
{
    qCDebug(XdgDesktopPortalSailfishLockdown) << "Setting mic mute:" << muted;
    // look up the pulse server socket:
    QDBusInterface *ifc = new QDBusInterface(
                       "org.PulseAudio1",
                       "/org/pulseaudio/server_lookup1",
                       "org.freedesktop.DBus.Properties");

    QDBusMessage result = ifc->call( "Get", "org.PulseAudio.ServerLookup1", "Address");
    QList<QVariant> rargs = result.arguments();
    if(rargs.isEmpty()) {
        qCDebug(XdgDesktopPortalSailfishLockdown) << "Could not detemine Pulse server address";
        return;
    }
    ifc->deleteLater();
    // create a p2p connection:
    QString address = rargs.first().toString();
    QDBusConnection conn = QDBusConnection::connectToPeer(address, "XDPPulse1");
    if (!conn.isConnected()) {
        qCDebug(XdgDesktopPortalSailfishLockdown) << "Could not connect to Pulse server";
        return;
    }
    // test the connection:
    QObject *core = conn.objectRegisteredAt("/org/pulseaudio/core1");
    qCDebug(XdgDesktopPortalSailfishLockdown) << "Got core object" << core->objectName() << core->metaObject()->className();;
    QDBusInterface *pulse = new QDBusInterface(
                          "org.PulseAudio1",
                          "/org/pulseaudio/core1",
                          "org.PulseAudio.Core1",
                         conn);
    QDBusMessage test = pulse->call( "Get", "org.PulseAudio.Core1", "Address");
    QList<QVariant> testargs = test.arguments();
    for (QVariant v : testargs) {
        qCDebug(XdgDesktopPortalSailfishLockdown) << "Got reply" << v.toString();
    }

}
// see also sailfishos/nemo-qml-plugin-systemsettings
void LockdownPortal::setLocationEnabled(const bool &enabled) const
{
    const QString LocationSettingsFile = QStringLiteral("/var/lib/location/location.conf");
    const QString CompatibilitySettingsFile = QStringLiteral("/etc/location/location.conf");
    const QString LocationSettingsSection = QStringLiteral("location");
    const QString LocationSettingsEnabledKey = QStringLiteral("enabled");

    // new file would be owned by creating process uid. we cannot allow this since the access is handled with group
    if (!QFile(LocationSettingsFile).exists()) {
        qWarning() << "Location settings configuration file does not exist. Refusing to create new.";
        return;
    }

    // write the values to the conf file
    QSettings settingsFile( LocationSettingsFile, QSettings::IniFormat);
    QSettings compatFile( CompatibilitySettingsFile, QSettings::IniFormat);
    settingsFile.setFallbacksEnabled(false);
    compatFile.setFallbacksEnabled(false);

    settingsFile.beginGroup(LocationSettingsSection);
    settingsFile.setValue(LocationSettingsEnabledKey, enabled);
    settingsFile.endGroup();

    compatFile.beginGroup(LocationSettingsSection);
    compatFile.setValue(LocationSettingsEnabledKey, enabled);
    compatFile.endGroup();

    settingsFile.sync();
    compatFile.sync();

}

}
}
