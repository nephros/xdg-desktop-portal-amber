/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 */

#ifndef XDG_DESKTOP_PORTAL_AMBER_LOCKDOWN_H
#define XDG_DESKTOP_PORTAL_AMBER_LOCKDOWN_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

namespace Amber {
class LockdownPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Lockdown")
    // note: the XDP spec uses hyphens as property names. They are not allowed
    // in C++ identifiers, and deprecated for D-Bus names.
    // soo, underscores.
    Q_PROPERTY(bool disable_location     READ getGPS WRITE setGPS)
    Q_PROPERTY(bool disable_sound_output READ getMute WRITE setMute)

public:
    explicit LockdownPortal(QObject *parent);
    //~LockdownPortal() override;

public Q_SLOTS:
    bool getGPS() const;
    void setGPS(const bool &off) const;
    bool getMute() const;
    void setMute(const bool &silent) const;

};
}
#endif // XDG_DESKTOP_PORTAL_AMBER_LOCKDOWN_H
