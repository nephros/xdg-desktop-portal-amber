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
#include <accesspolicy.h>

namespace Sailfish {
namespace XDP {
class LockdownPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Lockdown")
    // note: the XDP spec uses hyphens as property names. They are not allowed
    // in C++ identifiers, and deprecated for D-Bus names.
    // soo, underscores.

    // not supported, always false (enabled), discard writes
    Q_PROPERTY(bool disable_printing        READ disable_printing WRITE discard)
    Q_PROPERTY(bool disable_save_to_disk    READ disable_save_to_disk WRITE discard)
    Q_PROPERTY(bool disable_application_handlers READ disable_application_handlers WRITE discard)
    // supported
    Q_PROPERTY(bool disable_sound_output READ muted WRITE mute )
    // directly via MDM:
    Q_PROPERTY(bool disable_camera      READ disable_camera      WRITE setCameraDisabled)
    Q_PROPERTY(bool disable_microphone  READ disable_microphone  WRITE setMicrophoneDisabled)
    Q_PROPERTY(bool disable_location    READ disable_location    WRITE setLocationSettingsDisabled)

public:
    explicit LockdownPortal(QObject *parent);
    //~LockdownPortal() override;

public Q_SLOTS:
     void cameraDisabledChanged() {};
     void microphoneDisabledChanged() {};
     void locationSettingsDisabledChanged() {};

private:
    bool discard(const bool &dummy) const { return false; };
    bool disable_printing() const         { return false; };
    bool disable_save_to_disk() const     { return false; };
    bool disable_application_handlers() const { return false; };

    bool muted() const;
    void mute(const bool &silent) const;

    bool disable_camera() const;
    bool disable_microphone() const;
    bool disable_location() const;

    void setLocationSettingsDisabled(const bool &enable) const;
    void setMicrophoneDisabled(const bool &enable) const;
    void setCameraDisabled(const bool &enable) const;

    void setLocationEnabled(const bool &enabled) const;
    void setMicMutePulse(const bool &muted) const;

    AccessPolicy* m_policy;
};
}
}
#endif // XDG_DESKTOP_PORTAL_SAILFISH_LOCKDOWN_H
