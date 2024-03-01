/*
 * SPDX-FileCopyrightText: 2017 Red Hat Inc
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2017 Jan Grulich <jgrulich@redhat.com>
 * SPDX-FileCopyrightText: 2022 Harald Sitter <sitter@kde.org>
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 *
 */

#include "access.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(XdgDesktopPortalAmber, "xdp-amber-access")

namespace Amber {
AccessPortal::AccessPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalAmber) << "Desktop portal service: Access";
}

AccessPortal::~AccessPortal()
{
}


uint AccessPortal::AccessDialog(const QDBusObjectPath &handle,
                                const QString &app_id,
                                const QString &parent_window,
                                const QString &title,
                                const QString &subtitle,
                                const QString &body,
                                const QVariantMap &options,
                                QVariantMap &results)
{
    qCDebug(XdgDesktopPortalAmber) << "AccessDialog called with parameters:";
    qCDebug(XdgDesktopPortalAmber) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalAmber) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalAmber) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalAmber) << "    title: " << title;
    qCDebug(XdgDesktopPortalAmber) << "    subtitle: " << subtitle;
    qCDebug(XdgDesktopPortalAmber) << "    body: " << body;
    qCDebug(XdgDesktopPortalAmber) << "    options: " << options;

    if (!options.isEmpty()) {
            qCDebug(XdgDesktopPortalAmber) << "Access dialog options not supported.";
    }
    qCDebug(XdgDesktopPortalAmber) << "Asking Lipstick daemon to show a dialog";
    // TODO: windowprompt should have a dedicated prompt for Portal reqests. For now, use showInfoWindow
    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("com.jolla.windowprompt"),
                    QStringLiteral("/com/jolla/windowprompt"),
                    QStringLiteral("com.jolla.windowprompt"),
                    //QStringLiteral("newPermissionPrompt"));
                    QStringLiteral("showInfoWindow"));
                    //
    // TODO choices
    Q_UNUSED(results);

    /*
    <method name="newPermissionPrompt">
      <arg direction="in" type="s" name="desktopFile"/>
      <arg direction="in" type="a{sas}" name="permissions"/>
      <annotation value="PermissionClassMap" name="org.qtproject.QtDBus.QtTypeName.In1"/>
      <annotation value="required" name="com.jolla.windowprompt.PermissionClass"/>
      <arg direction="out" type="o" name="prompt"/>
    </method>
    <method name="showInfoWindow">
      <arg direction="in" type="s" name="title"/>
      <arg direction="in" type="s" name="subtitle"/>
      <arg direction="in" type="s" name="body"/>
    </method>
    */

    QList<QVariant> args;
    args.append(title);
    args.append(subtitle);
    args.append(body);
    msg.setArguments(args);

    QDBusPendingReply<QString> pcall = QDBusConnection::sessionBus().call(msg);
    pcall.waitForFinished();
    if (pcall.isValid()) {
            qCDebug(XdgDesktopPortalAmber) << "Success";
            return 0;
    }
    qCDebug(XdgDesktopPortalAmber) << "Access failed";
    return 1;
}
} // namespace Amber
