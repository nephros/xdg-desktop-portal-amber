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

const QString pulsePeerConnName = QStringLiteral("XDPPulsePeer1");

namespace Sailfish {
namespace XDP {
LockdownPortal::LockdownPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XDPortalSailfishLockdown) << "Desktop portal service: Lockdown";
    m_policy = new AccessPolicy(this);
    m_profiled = new QDBusInterface( QStringLiteral("com.nokia.profiled"), QStringLiteral("/com/nokia/profiled"), QStringLiteral("com.nokia.profiled"));
    if(!setupDefaultSource()) {
        qCCritical(XDPortalSailfishLockdown) << "Could not set up pulse source interface.";
    };

    QObject::connect(m_policy, SIGNAL(cameraEnabledChanged()), this, SLOT(cameraDisabledChanged()));
    //QObject::connect(m_policy, SIGNAL(microphoneEnabledChanged()), this, SLOT(microphoneDisabledChanged()));
    //QObject::connect(m_policy, SIGNAL(locationSettingsEnabledChanged()), this, SLOT(locationSettingsDisabledChanged()));
    // FIXME: signatures do not match:
    QObject::connect(m_policy, SIGNAL(profile_changed()), this, SLOT(locationSettingsDisabledChanged()));
    QObject::connect(m_defaultSource, SIGNAL(MuteUpdated(bool)), this, SLOT(microphoneDisabledChanged(bool)));

}

/*******************************************/
/***** Sound Output / Profile / Silent *****/
/*******************************************/
bool LockdownPortal::isSilent() const
{
    QDBusReply<QString> pcall = m_profiled->call(QStringLiteral("get_profile"));

    if (pcall.isValid()) {
        qCDebug(XDPortalSailfishLockdown) << "Read profile value:" << pcall.value();
        return (pcall.value() == PROFILE_MUTED_NAME);
    }
    // what else?
    return false;
}

void LockdownPortal::setSilent(const bool &silent) const
{
    qCDebug(XDPortalSailfishLockdown) << "Setting mute:" << silent;
    if (silent) {
         m_profiled->call(QStringLiteral("set_profile"), PROFILE_MUTED_NAME);
    } else {
        m_profiled->call(QStringLiteral("set_profile"), PROFILE_UNMUTED_NAME);
    }
}

/************************************/
/***** Camera                   *****/
/************************************/
bool LockdownPortal::disable_camera() const
{
    qCDebug(XDPortalSailfishLockdown) << "Reading Camera setting";
    return m_policy->cameraEnabled();
};
void LockdownPortal::setCameraDisabled(const bool &disable) const
{
    qCDebug(XDPortalSailfishLockdown) << "Setting Camera setting, disable:" << disable;
    m_policy->setCameraEnabled(!disable);
};

/************************************/
/***** Location                 *****/
/************************************/
bool LockdownPortal::disable_location() const
{
    qCDebug(XDPortalSailfishLockdown) << "Reading Location setting";
    return LockdownPortal::getLocationEnabled();
    //return m_policy->locationSettingsEnabled();
};
void LockdownPortal::setLocationSettingsDisabled(const bool &disable) const
{
    qCDebug(XDPortalSailfishLockdown) << "Setting Location setting, disable:" << disable;
    //m_policy->setLocationSettingsEnabled(!disable);
    setLocationEnabled(!disable);
};
bool LockdownPortal::getLocationEnabled() const
{
    qCDebug(XDPortalSailfishLockdown) << "Getting Location setting from file";
    if (!QFile(LocationSettingsFile).exists()) {
        qWarning() << "Location settings configuration file does not exist.";
        return false;
    }
    QSettings settingsFile( LocationSettingsFile, QSettings::IniFormat);
    settingsFile.beginGroup(LocationSettingsSection);
    bool value = settingsFile.value(LocationSettingsEnabledKey).toBool();
    settingsFile.endGroup();
    return value;
}
// see also sailfishos/nemo-qml-plugin-systemsettings
void LockdownPortal::setLocationEnabled(const bool &enabled) const
{
    qCDebug(XDPortalSailfishLockdown) << "Setting Location setting to enabled: :" << enabled;


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

/************************************/
/***** Microphone               *****/
/************************************/
bool LockdownPortal::disable_microphone() const
{
    qCDebug(XDPortalSailfishLockdown) << "Reading Mic setting";
    return getMicMutePulse();
    //return m_policy->microphoneEnabled();
};
void LockdownPortal::setMicrophoneDisabled(const bool &disable)
{
    qCDebug(XDPortalSailfishLockdown) << "Setting Microphone setting, disable:" << disable;
    setMicMutePulse(disable);
    //m_policy->setMicrophoneEnabled(!disable);
};
bool LockdownPortal::getMicMutePulse()
{
    return m_defaultSource->property("Mute").toBool();
}

void LockdownPortal::setMicMutePulse(const bool &muted)
{
    qCDebug(XDPortalSailfishLockdown) << "Setting pulse source muted:" << muted;
    m_defaultSource->setProperty("Mute", QVariant(muted));
}

/*! \fn bool LockdownPortal::connectToPulse()

    Establishes a D-Buse peer-to-peer connection to PulseAudio.
    The connection is represented by the \c m_pulse member.

    Returns \c true on success, \c false otherwise.
*/
bool LockdownPortal::connectToPulse()
{
    if ( m_pulse->isConnected()) {
        qCWarning(XDPortalSailfishLockdown) << "Trying to connect to Pulse Peer while connected";
    }
    // look up the pulse server socket:
    QDBusInterface *ifc = new QDBusInterface(
                       QStringLiteral("org.pulseaudio.Server"),
                       QStringLiteral("/org/pulseaudio/server_lookup1"),
                       QStringLiteral("org.PulseAudio.ServerLookup1")
                       );

    if (!ifc->isValid()) {
        qCCritical(XDPortalSailfishLockdown) << "Could not look up Pulse server address";
        ifc->deleteLater();
        return false;
    }
    QString address = ifc->property("Address").toString();
    if (address.isEmpty()) {
        qCCritical(XDPortalSailfishLockdown) << "Empty/invalid Pulse server address";
        ifc->deleteLater();
        return false;
    }

    //m_pulse = new QDBusConnection::connectToPeer(address, pulsePeerConnName);
    *m_pulse = QDBusConnection::connectToPeer(address, pulsePeerConnName);
    if (!m_pulse->isConnected()) {
        qCCritical(XDPortalSailfishLockdown) << "Could not connect to Pulse server";
        ifc->deleteLater();
        delete m_pulse;
        m_pulse = nullptr;
        return false;
    }
    qCInfo(XDPortalSailfishLockdown) << "Pulse P2P Connection established";
    return true;
}

/*! \fn bool LockdownPortal::setupDefaultSource()

    Looks up the PulseAudio default Source via D-Bus, and sets up a QDBusInterface for accessing it.
    The interface is represented by the \c m_defaultSource member.

    Returns \c true on success, \c false otherwise.
*/
bool LockdownPortal::setupDefaultSource()
{
    if (m_defaultSource->isValid()) {
        qCWarning(XDPortalSailfishLockdown) << "Pulse source already set.";
        return true;
    }
    static const QString sourceName(QStringLiteral("source.primary_input"));


    if(!connectToPulse()) {
        qCCritical(XDPortalSailfishLockdown) << "Could not set up Pulse peer";
        return false;
    }
    QDBusInterface *core = new QDBusInterface(
                          QStringLiteral(""),
                          QStringLiteral("/org/pulseaudio/core1"),
                          QStringLiteral("org.PulseAudio.Core1"),
                          *m_pulse);
    if(!core->isValid()) {
        qCCritical(XDPortalSailfishLockdown) << "Could not set up Core interface";
        return false;
    }
    QDBusReply<QDBusObjectPath> source = core->call( "GetSourceByName", sourceName);
    if(!source.isValid()) {
        qCCritical(XDPortalSailfishLockdown) << "Could find default input";
        return false;
    }
    QString input = source.value().path();
    qCDebug(XDPortalSailfishLockdown) << "default input:" << input;
    core->deleteLater();
    m_defaultSource = new QDBusInterface(
                          QStringLiteral(""),
                          input,
                          QStringLiteral("org.PulseAudio.Core1.Device"),
                          *m_pulse);
    if(!m_defaultSource->isValid()) {
        qCCritical(XDPortalSailfishLockdown) << "Could not set up Device interface";
        return false;
    }
    return true;
}
}
}
