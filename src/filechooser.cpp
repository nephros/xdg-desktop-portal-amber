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
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>

#include <QProcess>

Q_LOGGING_CATEGORY(XdgDesktopPortalAmberFileChooser, "xdp-amber-filechooser")

namespace Amber {

const char* FileChooserPortal::ui_helper = "/usr/bin/xdg-desktop-portal-amber-ui";

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
    qCDebug(XdgDesktopPortalAmberFileChooser) << "FileChooser.OpenFile called with parameters:";
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    title: " << title;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    options: " << options;

    /*
    qCDebug(XdgDesktopPortalAmberFileChooser) << "Asking Lipstick daemon to show a dialog";
    // TODO: windowprompt should have a dedicated prompt for Portal reqests. For now, use showInfoWindow
    QDBusMessage msg = QDBusMessage::createMethodCall(
                    QStringLiteral("com.jolla.windowprompt"),
                    QStringLiteral("/com/jolla/windowprompt"),
                    QStringLiteral("com.jolla.windowprompt"),
                    //QStringLiteral("newPermissionPrompt"));
                    QStringLiteral("showTermsPrompt"));

    QList<QVariant> args;
    //promptConfig
    args.append(QStringLiteral("/usr/share/lipstick-xdg-desktop-portal-amber/FilePickerDialog.qml"));
    msg.setArguments(args);
    */

    // TODO choices
    Q_UNUSED(results);

    qCDebug(XdgDesktopPortalAmberFileChooser) << "Trying to show a dialog";

    // pass the handle so we can respond from the dialog
    QStringList args;
    args.append(QStringLiteral("--handle"));
    args.append(handle.path());

    args.append(QStringLiteral("--title"));
    QString titlestr = title;
    titlestr.prepend("'"); titlestr.append("'"); // guard agains special chacacters
    args.append(titlestr);
    // just pass the options as-is, we can possibly deal with it in the UI:
    QString jopts = QJsonDocument(QJsonObject::fromVariantMap(options)).toJson();
    jopts.prepend("'"); jopts.append("'");
    args.append(QStringLiteral("--options"));
    args.append(jopts);

    QProcess dialog;
    dialog.setProgram(FileChooserPortal::ui_helper);
    dialog.setArguments(args);
    dialog.start();
    if (dialog.waitForFinished()) {
        qCDebug(XdgDesktopPortalAmberFileChooser) << "Success";
        auto out = dialog.readAllStandardOutput();
        auto err = dialog.readAllStandardError();
        auto lines = out.split("\n");
        QString resline;
        for (auto i = lines.begin(), end = lines.end(); i != end; ++i) {
            if (*i.startsWith("### Results:")) {
                i++;
                resline << *i;
            }
        }
        return 0;
    }
    qCDebug(XdgDesktopPortalAmberFileChooser) << "FileChooser failed:" << pcall.error().name() << pcall.error().message() ;
    return 1;
    /*
    QDBusMessage msg = QDBusMessage::createMethodCall(
                    QStringLiteral("org.freedesktop.impl.portal.desktop.amber.ui"),
                    QStringLiteral("/org/freedesktop/impl/portal/desktop/amber/ui"),
                    QStringLiteral("org.freedesktop.impl.portal.desktop.amber.ui"),
                    QStringLiteral("openFilePicker")
                    );

    // pass the handle so we can respond from the dialog
    QList<QVariant> args;
    args.append(handle.path());

    args.append(title);
    // just pass the options as-is, we can possibly deal with it in the UI:
    QString jopts = QJsonDocument(QJsonObject::fromVariantMap(options)).toJson();
    args.append(jopts);

    msg.setArguments(args);

    QDBusPendingReply<> pcall = QDBusConnection::sessionBus().call(msg);
    pcall.waitForFinished();
    if (pcall.isValid()) {
            qCDebug(XdgDesktopPortalAmberFileChooser) << "Success";
            return 0;
    }
    qCDebug(XdgDesktopPortalAmberFileChooser) << "FileChooser failed:" << pcall.error().name() << pcall.error().message() ;
    return 1;
    */
}

uint FileChooserPortal::SaveFile(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QString &title,
                      const QVariantMap &options,
                      QVariantMap &results)
{
    qCDebug(XdgDesktopPortalAmberFileChooser) << "FileChooser.SaveFiles called with parameters:";
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    title: " << title;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    options: " << options;
    if (!options.isEmpty()) {
            qCDebug(XdgDesktopPortalAmberFileChooser) << "FileChooser dialog options not supported.";
    }
    Q_UNUSED(results);
    qCDebug(XdgDesktopPortalAmberFileChooser) << "This dialog is not implemented.";
    return 1;

}
uint FileChooserPortal::SaveFiles(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QString &title,
                      const QVariantMap &options,
                      QVariantMap &results)
{
    qCDebug(XdgDesktopPortalAmberFileChooser) << "FileChooser.SaveFile called with parameters:";
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    title: " << title;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    options: " << options;
    if (!options.isEmpty()) {
            qCDebug(XdgDesktopPortalAmberFileChooser) << "FileChooser dialog options not supported.";
    }
    Q_UNUSED(results);
    qCDebug(XdgDesktopPortalAmberFileChooser) << "This dialog is not implemented.";
    return 1;
}
} // namespace Amber
