/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 */

#ifndef XDG_DESKTOP_PORTAL_SAILFISH_LOCKDOWN_H
#define XDG_DESKTOP_PORTAL_SAILFISH_LOCKDOWN_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <policyvalue.h>

namespace Sailfish {
namespace XDP {
class LockdownPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Lockdown")
    // note: the XDP spec uses hyphens as property names. They are not allowed
    // in C++ identifiers, and deprecated for D-Bus names.
    // soo, underscores.

    // not supported, always false (enabled)
    Q_PROPERTY(bool disable_printing READ printingDisabled CONSTANT)
    Q_PROPERTY(bool disable_save_to_disk READ saveDisabled CONSTANT)
    Q_PROPERTY(bool disable_application_handlers READ apphandlersDisabled CONSTANT)
    // supported
    Q_PROPERTY(bool disable_sound_output READ getMute WRITE setMute )
    // directly via MDM:
    Q_PROPERTY(bool disable_camera
                    MEMBER cameraEnabled
                    READ cameraEnabled
                    WRITE setCameraEnabled
              )
    Q_PROPERTY(bool disable_microphone
                    MEMBER microphoneEnabled
                    READ microphoneEnabled
                    WRITE setMicrophoneEnabled
              )
    Q_PROPERTY(bool disable_location
                    READ locationSettingsEnabled
                    WRITE setLocationSettingsEnabled
              )

public:
    explicit LockdownPortal(QObject *parent);
    //~LockdownPortal() override;

public Q_SLOTS:
    bool printingDisabled() const { return false; };
    bool saveDisabled() const { return false; };
    bool apphandlersDisabled() const { return false; };

    bool getMute() const;
    void setMute(const bool &silent) const;
    bool cameraEnabled()           { return !Sailfish::PolicyValue::keyValue(Sailfish::PolicyValue::CameraEnabled).toBool(); };
    bool microphoneEnabled()       { return !Sailfish::PolicyValue::keyValue(Sailfish::PolicyValue::MicrophoneEnabled).toBool(); };
    bool locationSettingsEnabled() { return !Sailfish::PolicyValue::keyValue(Sailfish::PolicyValue::LocationSettingsEnabled).toBool(); };
    void setLocationSettingsEnabled(const bool &enable) {
        Sailfish::PolicyValue::setKeyValue(Sailfish::PolicyValue::LocationSettingsEnabled, QVariant(enable));
    };
    void setMicrophoneEnabled(const bool &enable) {
        Sailfish::PolicyValue::setKeyValue(Sailfish::PolicyValue::MicrophoneEnabled, QVariant(enable));
    };
    void setCameraEnabled(const bool &enable) {
        Sailfish::PolicyValue::setKeyValue(Sailfish::PolicyValue::CameraEnabled, QVariant(enable));
    };

};
}
}
#endif // XDG_DESKTOP_PORTAL_SAILFISH_LOCKDOWN_H
