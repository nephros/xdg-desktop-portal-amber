/*
 * SPDX-FileCopyrightText: 2017 Red Hat Inc
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2017 Jan Grulich <jgrulich@redhat.com>
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 */

#ifndef XDG_DESKTOP_PORTAL_SAILFISH_ACCESS_H
#define XDG_DESKTOP_PORTAL_SAILFISH_ACCESS_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <accesspolicyplugin.h>

namespace Sailfish {
namespace XDP {
class AccessMDMPlugin : public Sailfish::AccessPolicyPlugin
{
    Q_OBJECT
public:
    explicit AccessMDMPlugin();
    //~AccessMDMPlugin() override;

    virtual QVariant keyValue(const QString &key) override;
    virtual void setKeyValue(const QString &key, const QVariant &value) override;
signals:
    //void keyValueChanged(const QString &key, const QVariant &value) override;
};
class AccessPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Access")

public:
    explicit AccessPortal(QObject *parent);
    ~AccessPortal() override;

    enum DialogResponse : uint
    {
        Accepted = 0,
        Cancelled = 1,
        Other = 2
    };
    Q_ENUM(DialogResponse)

public Q_SLOTS:
    uint AccessDialog(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QString &title,
                      const QString &subtitle,
                      const QString &body,
                      const QVariantMap &options,
                      QVariantMap &results);
    void handleDialogError();
    void handleDialogResponse( const int &code);
    void waitForDialogResponse();
    void setupDialogResponse();

private:
    bool m_responseHandled;
    DialogResponse m_callResponseCode;
    AccessMDMPlugin m_plugin;
};
}
}
#endif // XDG_DESKTOP_PORTAL_SAILFISH_ACCESS_H
