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
class LockdownPortal : public QDBusAbstractAdaptor, public Sailfish::AccessPolicy
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Lockdown")
    // note: the XDP spec uses hyphens as property names. They are not allowed
    // in C++ identifiers, and deprecated for D-Bus names.
    // soo, underscores.
    Q_PROPERTY(bool disable_printing MEMBER m_printing)
    Q_PROPERTY(bool disable_save_to_disk MEMBER m_save)
    Q_PROPERTY(bool disable_application_handlers MEMBER m_apphandlers)
    Q_PROPERTY(bool disable_sound_output MEMBER m_sound)
    // directly from MDM:
    Q_PROPERTY(bool disable_camera
                    MEMBER cameraEnabled
                    READ cameraEnabled
                    NOTIFY cameraEnabledChanged
              )
    Q_PROPERTY(bool disable_microphone
                    MEMBER microphoneEnabled
                    READ microphoneEnabled
                    NOTIFY microphoneEnabledChanged
                    )
    Q_PROPERTY(bool disable_location
                    MEMBER locationSettingsEnabled
                    NOTIFY locationSettingsEnabledChanged
                    )

public:
    explicit LockdownPortal(QObject *parent);
    //~LockdownPortal() override;

signals:
    void cameraEnabledChanged() = default;
    void microphoneEnabledChanged() = default;
    void locationSettingsEnabledChanged() = default;

private:
    AccessPolicy m_access;
};
}
}
#endif // XDG_DESKTOP_PORTAL_SAILFISH_LOCKDOWN_H
