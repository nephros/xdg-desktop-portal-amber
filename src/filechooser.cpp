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

Q_LOGGING_CATEGORY(XdgDesktopPortalAmberFileChooser, "xdp-amber-access")

namespace Amber {
FileChooserPortal::FileChooserPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalAmberFileChooser) << "Desktop portal service: FileChooser";
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
    qCDebug(XdgDesktopPortalAmberFileChooser) << "FileChooserDialog called with parameters:";
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    title: " << title;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    options: " << options;

    if (!options.isEmpty()) {
            qCDebug(XdgDesktopPortalAmberFileChooser) << "FileChooser dialog options not supported.";
    }
    /*
       <method name="showTermsPrompt">
       <arg direction="in" type="a{sv}" name="promptConfig"/>
       <annotation value="QVariantMap" name="org.qtproject.QtDBus.QtTypeName.In0"/>
       </method>
    */

    qCDebug(XdgDesktopPortalAmberFileChooser) << "Asking Lipstick daemon to show a dialog";
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
    args.append(QStringLiteral("{ \"componentName\": \"xdg-portal-amber/FilePickerDialog.qml\" }");
    msg.setArguments(args);

    QDBusPendingReply<QString> pcall = QDBusConnection::sessionBus().call(msg);
    pcall.waitForFinished();
    if (pcall.isValid()) {
            qCDebug(XdgDesktopPortalAmberFileChooser) << "Success";
            return 0;
    }
    qCDebug(XdgDesktopPortalAmberFileChooser) << "FileChooser failed";
    return 1;
}
} // namespace Amber
