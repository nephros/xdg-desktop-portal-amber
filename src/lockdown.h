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
#include <QDBusInterface>
#include <accesspolicy.h>

namespace Sailfish {
namespace XDP {
using ProfiledProfile = QList<QList<QString>>;
class LockdownPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Lockdown")
    // note: the XDP spec uses hyphens as property names. They are not allowed
    // in C++ identifiers, and deprecated for D-Bus names.
    // Soo, underscores.

    // not supported, always false (enabled), discard writes
    Q_PROPERTY(bool disable_printing             MEMBER printing_dummy )
    Q_PROPERTY(bool disable_save_to_disk         MEMBER save_dummy )
    Q_PROPERTY(bool disable_application_handlers MEMBER handlers_dummy )
    // supported
    Q_PROPERTY(bool disable_sound_output READ isSilent            WRITE setSilent NOTIFY silentChanged)
    Q_PROPERTY(bool disable_location     READ disable_location    WRITE setLocationSettingsDisabled)
    Q_PROPERTY(bool disable_microphone   READ disable_microphone  WRITE setMicrophoneDisabled)
    // directly via MDM:
    Q_PROPERTY(bool disable_camera       READ disable_camera      WRITE setCameraDisabled)

public:
    explicit LockdownPortal(QObject *parent);
    //~LockdownPortal() override;
    Q_SIGNAL void silentChanged();

public Q_SLOTS:
     void cameraDisabledChanged() {};
     void microphoneDisabledChanged(const bool) {};
     void locationSettingsDisabledChanged() {};

     // signal from profiled is profile_changed( bbsa(sss) )
     void profileChanged(const bool, const bool, const QString, const ProfiledProfile) { emit silentChanged(); };

private:
    bool printing_dummy = false;
    bool save_dummy     = false;
    bool handlers_dummy = false;

    bool isSilent() const;
    void setSilent(const bool &silent) const;

    bool disable_camera() const;
    bool disable_microphone();
    bool disable_location() const;

    void setLocationSettingsDisabled(const bool &enable) const;
    void setMicrophoneDisabled(const bool &enable);
    void setCameraDisabled(const bool &enable) const;

    void setLocationEnabled(const bool &enabled) const;
    bool getLocationEnabled() const;

    bool setupDefaultSource();

    AccessPolicy* m_policy;
    QDBusInterface* m_profiled;
    QDBusInterface* m_defaultSource;
};
}
}
Q_DECLARE_METATYPE(Sailfish::XDP::ProfiledProfile);

#endif // XDG_DESKTOP_PORTAL_SAILFISH_LOCKDOWN_H
