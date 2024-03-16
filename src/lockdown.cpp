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
#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QDBusObjectPath>
#include <QLoggingCategory>
#include <accesspolicy.h>
#include <QFile>
#include <QSettings>

Q_LOGGING_CATEGORY(XDPortalSailfishLockdown, "xdp-sailfish-lockdown")

const char* PROFILE_MUTED_NAME = "silent";
const char* PROFILE_UNMUTED_NAME = "general";

const QString LocationSettingsFile = QStringLiteral("/var/lib/location/location.conf");
const QString CompatibilitySettingsFile = QStringLiteral("/etc/location/location.conf");
const QString LocationSettingsSection = QStringLiteral("location");
const QString LocationSettingsEnabledKey = QStringLiteral("enabled");

namespace Sailfish {
namespace XDP {
LockdownPortal::LockdownPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XDPortalSailfishLockdown) << "Desktop portal service: Lockdown";
    qCDebug(XdgDesktopPortalSailfishLockdown) << "Desktop portal service: Lockdown";
    m_policy = new AccessPolicy(this);
    m_profiled = new QDBusInterface( QStringLiteral("com.nokia.profiled"), QStringLiteral("/com/nokia/profiled"), QStringLiteral("com.nokia.profiled"));

    QObject::connect(m_policy, SIGNAL(cameraEnabledChanged()), this, SLOT(cameraDisabledChanged()));
    //QObject::connect(m_policy, SIGNAL(microphoneEnabledChanged()), this, SLOT(microphoneDisabledChanged()));
    //QObject::connect(m_policy, SIGNAL(locationSettingsEnabledChanged()), this, SLOT(locationSettingsDisabledChanged()));
    // FIXME: signatures do not match:
    QObject::connect(m_profiled, SIGNAL(profile_changed()), this, SLOT(locationSettingsDisabledChanged()));
    QObject::connect(m_defaultSource, SIGNAL(MuteUpdated(bool)), this, SLOT(microphoneDisabledChanged(bool)));

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
    return LockdownPortal::getLocationEnabled();
    //return m_policy->locationSettingsEnabled();
};

void LockdownPortal::setLocationSettingsDisabled(const bool &disable) const
{
    //m_policy->setLocationSettingsEnabled(!disable);
    setLocationEnabled(!disable);
};
void LockdownPortal::setMicrophoneDisabled(const bool &disable) const
{
    qCDebug(XdgDesktopPortalSailfishLockdown) << "Setting Microphone via Pulse, disable:" << disable;
    setMicMutePulse(disable);
    //qCDebug(XdgDesktopPortalSailfishLockdown) << "Setting Microphone policy, disable:" << disable;
    //m_policy->setMicrophoneEnabled(!disable);
};
void LockdownPortal::setCameraDisabled(const bool &disable) const
{
    qCDebug(XdgDesktopPortalSailfishLockdown) << "Setting Camera policy, disable:" << disable;
    m_policy->setCameraEnabled(!disable);
};

void LockdownPortal::setMicMutePulse(const bool &muted) const
{
    qCDebug(XdgDesktopPortalSailfishLockdown) << "Setting pulse source muted:" << muted;
    QDBusInterface *device = getPulseSource();
    if(!device->isValid()) {
        qCDebug(XdgDesktopPortalSailfishLockdown) << "Could not set up interface";
        return;
    }
    device->setProperty("Mute", QVariant(muted));
}
QDBusInterface* LockdownPortal::getPulseSource() const
{

    static const QString sourceName(QStringLiteral("source.primary_input"));
    static const QString peerConnName(QStringLiteral("XDPPulsePeer1"));

    qCDebug(XdgDesktopPortalSailfishLockdown) << "Looking for Pulse source";

    // look up the pulse server socket:
    QDBusInterface *ifc = new QDBusInterface(
                       QStringLiteral("org.pulseaudio.Server"),
                       QStringLiteral("/org/pulseaudio/server_lookup1"),
                       QStringLiteral("org.PulseAudio.ServerLookup1")
                       );

    if (!ifc->isValid()) {
        qCDebug(XdgDesktopPortalSailfishLockdown) << "Could not look up Pulse server address";
        return nullptr;
    }
    QString address = ifc->property("Address").toString();

    qCDebug(XdgDesktopPortalSailfishLockdown) << "Got address" << address;
    ifc->deleteLater();

    // create a p2p connection:
    QDBusConnection peer = QDBusConnection::connectToPeer(address, peerConnName);
    if (!peer.isConnected()) {
        qCDebug(XdgDesktopPortalSailfishLockdown) << "Could not connect to Pulse server";
        return nullptr;
    }
    qCDebug(XdgDesktopPortalSailfishLockdown) << "Pulse P2P Connection established";
    QDBusInterface *core = new QDBusInterface(
                          QStringLiteral(""),
                          QStringLiteral("/org/pulseaudio/core1"),
                          QStringLiteral("org.PulseAudio.Core1"),
                          peer);
    if(!core->isValid()) {
        qCDebug(XdgDesktopPortalSailfishLockdown) << "Could not set up interface";
        return nullptr;
    }
    QDBusReply<QDBusObjectPath> source = core->call( "GetSourceByName", sourceName);
    if(!source.isValid()) {
        qCDebug(XdgDesktopPortalSailfishLockdown) << "Could find default input";
        return nullptr;
    }
    QString input = source.value().path();
    qCDebug(XdgDesktopPortalSailfishLockdown) << "default input:" << input;
    core->deleteLater();
    QDBusInterface *device = new QDBusInterface(
                          QStringLiteral(""),
                          input,
                          QStringLiteral("org.PulseAudio.Core1.Device"),
                          peer);
    if(!device->isValid()) {
        qCDebug(XdgDesktopPortalSailfishLockdown) << "Could not set up interface";
        return nullptr;
    }
    QDBusConnection peer = QDBusConnection::disConnectFromPeer(peerConnName);
    return device;
}

bool LockdownPortal::getLocationEnabled() const
{
    qCDebug(XdgDesktopPortalSailfishLockdown) << "Getting Location setting from file";
    if (!QFile(LocationSettingsFile).exists()) {
        qWarning() << "Location settings configuration file does not exist.";
        return false;
    }
    QSettings settingsFile( LocationSettingsFile, QSettings::IniFormat);
    //QSettings compatFile( CompatibilitySettingsFile, QSettings::IniFormat);
    settingsFile.beginGroup(LocationSettingsSection);
    bool value = settingsFile.value(LocationSettingsEnabledKey).toBool();
    settingsFile.endGroup();
    return value;
}
// see also sailfishos/nemo-qml-plugin-systemsettings
void LockdownPortal::setLocationEnabled(const bool &enabled) const
{
    qCDebug(XdgDesktopPortalSailfishLockdown) << "Setting Location setting to enabled: :" << enabled;


    // new file would be owned by creating process uid. we cannot allow this since the access is handled with group
    if (!QFile(LocationSettingsFile).exists()) {
        qWarning() << "Location settings configuration file does not exist. Refusing to create new.";
        return;
    }

    // write the values to the conf file
    // FIXME: we should mutex or somesuuch here;
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
