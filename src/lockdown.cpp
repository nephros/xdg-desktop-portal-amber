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
#include <QDBusConnection>
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

    m_defaultSource = nullptr;
    if(!setupDefaultSource()) {
        qCCritical(XDPortalSailfishLockdown) << "Could not set up pulse source interface.";
    } else {
        if ( m_defaultSource != nullptr) {
            QObject::connect(m_defaultSource, SIGNAL(MuteUpdated(bool)), this, SLOT(microphoneDisabledChanged(bool)));
        }
    };

    QObject::connect(m_policy, SIGNAL(cameraEnabledChanged()), this, SLOT(cameraDisabledChanged()));
    //QObject::connect(m_policy, SIGNAL(microphoneEnabledChanged()), this, SLOT(microphoneDisabledChanged()));
    //QObject::connect(m_policy, SIGNAL(locationSettingsEnabledChanged()), this, SLOT(locationSettingsDisabledChanged()));
    // FIXME: signatures do not match:
    QObject::connect(m_profiled, SIGNAL(profile_changed()), this, SLOT(profileChanged()));

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
    qCDebug(XDPortalSailfishLockdown) << "Read: Camera setting";
    return m_policy->cameraEnabled();
};
void LockdownPortal::setCameraDisabled(const bool &disable) const
{
    qCDebug(XDPortalSailfishLockdown) << "Set: Camera disable:" << disable;
    m_policy->setCameraEnabled(!disable);
};

/************************************/
/***** Location                 *****/
/************************************/
bool LockdownPortal::disable_location() const
{
    qCDebug(XDPortalSailfishLockdown) << "Read: Location setting";
    return LockdownPortal::getLocationEnabled();
    //return m_policy->locationSettingsEnabled();
};
void LockdownPortal::setLocationSettingsDisabled(const bool &disable) const
{
    qCDebug(XDPortalSailfishLockdown) << "Set: Location disable:" << disable;
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
bool LockdownPortal::disable_microphone()
{
    qCDebug(XDPortalSailfishLockdown) << "Read: Microphone setting";
    return m_defaultSource->property("Mute").toBool();
    //return m_policy->microphoneEnabled();
};
void LockdownPortal::setMicrophoneDisabled(const bool &disable)
{
    qCDebug(XDPortalSailfishLockdown) << "Set: Microphone disable:" << disable;
    m_defaultSource->setProperty("Mute", QVariant(disable));
    //m_policy->setMicrophoneEnabled(!disable);
};

/* \fn bool LockdownPortal::setupDefaultSource()

    Looks up the PulseAudio default Source via D-Bus, and sets up a QDBusInterface for accessing it.
    The interface is represented by the \c m_defaultSource member.

    Returns \c true on success, \c false otherwise.
*/
bool LockdownPortal::setupDefaultSource()
{

    // FIXME: these appear to be device- or adaptation-dependent.
    static const QStringList sourceNames = {
            QStringLiteral("default-source"),   // named by user
            QStringLiteral("source.primary_input"), // Xperia10III
            QStringLiteral("source.droid"),         // Xperia10
            QStringLiteral("source.null")
            };

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
    // Create a peer-to-peer connection to the server
    QDBusConnection pulse = QDBusConnection::connectToPeer(address, pulsePeerConnName);
    if (!pulse.isConnected()) {
        qCCritical(XDPortalSailfishLockdown) << "Could not connect to Pulse server";
        ifc->deleteLater();
        return false;
    }
    qCInfo(XDPortalSailfishLockdown) << "Pulse P2P Connection established";

    // Use Core control interface to look up source
    QDBusInterface *core = new QDBusInterface(
                           QStringLiteral(""),
                           QStringLiteral("/org/pulseaudio/core1"),
                           QStringLiteral("org.PulseAudio.Core1"),
                           pulse);
    if(!core->isValid()) {
        qCCritical(XDPortalSailfishLockdown) << "Could not set up Core interface";
        return false;
    }
    // TODO: should we just iterate all sources and find the active one instead?
    // if yes:
    //  - get the org.PulseAudio.Core1.Device interface on the object path
    //  - get the State property
    //  - if State != 2 (Suspended)
    //  - if State == 0 (Running)
    //  - if State == 1 (Idle)
    // ... or should we just mute all the devices unconditionally?
    // Try to find the object path of a valid source by name:
    QDBusReply<QDBusObjectPath> source;
    QString input;
    for (QString sourceName : sourceNames) {
        source = core->call("GetSourceByName", sourceName);
        if(!source.isValid()) {
            qCDebug(XDPortalSailfishLockdown) << "Could not find find default input" << sourceName;
        } else {
            input = source.value().path();
            break;
        }
    }
    if(!source.isValid()) {
        qCCritical(XDPortalSailfishLockdown) << "Could not find any valid source, trying builtin fallback.";
        input = core->property("FallbackSource").toString();
    }
    if(input.isEmpty()) {
        qCCritical(XDPortalSailfishLockdown) << "Could not find any valid input source.";
        return false;
    }
    qCDebug(XDPortalSailfishLockdown) << "default input:" << input;
    core->deleteLater();

    // Using the device path, set up the Device interface
    m_defaultSource = new QDBusInterface(
                      QStringLiteral(""),
                      input,
                      QStringLiteral("org.PulseAudio.Core1.Device"),
                      pulse);
    if(!m_defaultSource->isValid()) {
        qCCritical(XDPortalSailfishLockdown) << "Could not set up Device interface";
        return false;
    }
    return true;
}

}
}
