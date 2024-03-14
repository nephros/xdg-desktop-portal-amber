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
    Q_PROPERTY(bool disable_printing READ printingDisabled CONSTANT)
    Q_PROPERTY(bool disable_save_to_disk READ saveDisabled CONSTANT)
    Q_PROPERTY(bool disable_application_handlers READ apphandlersDisabled CONSTANT)
    // supported
    Q_PROPERTY(bool disable_sound_output READ getMute WRITE setMute )
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

public Q_SLOTS:
    bool printingDisabled() const { return false; };
    bool saveDisabled() const { return false; };
    bool apphandlersDisabled() const { return false; };

    bool getMute() const;
    void setMute(const bool &silent) const;

signals:
    void cameraEnabledChanged();
    void microphoneEnabledChanged();
    void locationSettingsEnabledChanged();

private:
    AccessPolicy m_access;
};
}
}
#endif // XDG_DESKTOP_PORTAL_SAILFISH_LOCKDOWN_H
