/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 *
 */

#include "filechooser.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(XdgDesktopPortalAmber, "xdp-amber-filechooser")

namespace Amber {
FileChooserPortal::FileChooserPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalAmber) << "Desktop portal service: FileChooser";
}

FileChooserPortal::~FileChooserPortal()
{
}


uint FileChooserPortal::OpenFile(const QDBusObjectPath &handle,
                                const QString &app_id,
                                const QString &parent_window,
                                const QString &title,
                                const QVariantMap &options,
                                QVariantMap &results)
{
    qCDebug(XdgDesktopPortalAmber) << "FileChooserDialog called with parameters:";
    qCDebug(XdgDesktopPortalAmber) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalAmber) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalAmber) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalAmber) << "    title: " << title;
    qCDebug(XdgDesktopPortalAmber) << "    options: " << options;

    if (!options.isEmpty()) {
            qCDebug(XdgDesktopPortalAmber) << "FileChooser dialog options not supported.";
    }
    /*
       <method name="showTermsPrompt">
       <arg direction="in" type="a{sv}" name="promptConfig"/>
       <annotation value="QVariantMap" name="org.qtproject.QtDBus.QtTypeName.In0"/>
       </method>
    */

    qCDebug(XdgDesktopPortalAmber) << "Asking Lipstick daemon to show a dialog";
    // TODO: windowprompt should have a dedicated prompt for Portal reqests. For now, use showInfoWindow
    QDBusMessage msg = QDBusMessage::createMethodCall(
                    QStringLiteral("com.jolla.windowprompt"),
                    QStringLiteral("/com/jolla/windowprompt"),
                    QStringLiteral("com.jolla.windowprompt"),
                    //QStringLiteral("newPermissionPrompt"));
                    QStringLiteral("showTermsPrompt"));
                    //
    // TODO choices
    Q_UNUSED(results);

    QList<QVariant> args;
	//promptConfig
    args.append(QStringLiteral("{ \"componentName\": \"xdg-portal-amber/FilePickerDialog.qml\" }"));
    msg.setArguments(args);

    QDBusPendingReply<QString> pcall = QDBusConnection::sessionBus().call(msg);
    pcall.waitForFinished();
    if (pcall.isValid()) {
            qCDebug(XdgDesktopPortalAmber) << "Success";
            return 0;
    }
    qCDebug(XdgDesktopPortalAmber) << "FileChooser failed";
    return 1;
}
} // namespace Amber
