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

public:
    explicit LockdownPortal(QObject *parent);
    //~LockdownPortal() override;

    //Q_PROPERTY(bool disable-printing  READ foo WRITE bar);
    //Q_PROPERTY(bool disable-save-to-disk  READ foo WRITE bar);
    //Q_PROPERTY(bool disable-application-handlers  READ foo WRITE bar);
    Q_PROPERTY(bool disable-location     READ getGPS WRITE setGPS);
    //Q_PROPERTY(bool disable-camera       READ foo WRITE bar);
    //Q_PROPERTY(bool disable-microphone   READ getMic WRITE setMic);
    Q_PROPERTY(bool disable-sound-output READ getMute WRITE setMute);

public Q_SLOTS:
    bool getGPS() const;
    void setGPS(const bool &off) const;
    bool getMute() const;
    void setMute(const bool &silent const) const;

}
#endif // XDG_DESKTOP_PORTAL_AMBER_LOCKDOWN_H
