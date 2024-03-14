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

Q_LOGGING_CATEGORY(XdgDesktopPortalSailfishAccess, "xdp-sailfish-access")

namespace Sailfish {
AccessPortal::AccessPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalSailfishAccess) << "Desktop portal service: Access";
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
    qCDebug(XdgDesktopPortalSailfishAccess) << "AccessDialog called with parameters:";
    qCDebug(XdgDesktopPortalSailfishAccess) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalSailfishAccess) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalSailfishAccess) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalSailfishAccess) << "    title: " << title;
    qCDebug(XdgDesktopPortalSailfishAccess) << "    subtitle: " << subtitle;
    qCDebug(XdgDesktopPortalSailfishAccess) << "    body: " << body;
    qCDebug(XdgDesktopPortalSailfishAccess) << "    options: " << options;

    if (!options.isEmpty()) {
            qCDebug(XdgDesktopPortalSailfishAccess) << "Access dialog options not supported.";
    }
    qCDebug(XdgDesktopPortalSailfishAccess) << "Asking Lipstick daemon to show a dialog";
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
            qCDebug(XdgDesktopPortalSailfishAccess) << "Success";
            return 0;
    }
    qCDebug(XdgDesktopPortalSailfishAccess) << "Access failed:" << pcall.error().name() << pcall.error().message() ;
    return 1;
}
} // namespace Sailfish
