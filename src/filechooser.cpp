/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 *
 */

#include "filechooser.h"

#include <QCoreApplication>
#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QThread>

Q_LOGGING_CATEGORY(XdgDesktopPortalAmberFileChooser, "xdp-amber-filechooser")

namespace Amber {
FileChooserPortal::FileChooserPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalAmberFileChooser) << "Desktop portal service: FileChooser";
    m_responseHandled = false;
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
    //Q_UNUSED(results);

    // clean up from last time:
    m_callResult = QVariantMap();
    m_callResponseCode = PickerResponse::Other;
    m_responseHandled = false;

    qCDebug(XdgDesktopPortalAmberFileChooser) << "Trying to show a dialog";

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

    // the call will cause the UI to show, but will not wait for user
    // selection.
    // So we set up a signal handler in setupPickerResponse, and continue
    // when we have received the signal.
    QDBusConnection::sessionBus().callWithCallback(
                    msg, this,
                    SLOT(setupPickerResponse()),
                    SLOT(handlePickerError())
                    );

    // busy loop ;)
    waitForPickerResponse();

    if (m_callResponseCode != PickerResponse::Other) {
        qCDebug(XdgDesktopPortalAmberFileChooser) << "Success";
    } else {
        qCDebug(XdgDesktopPortalAmberFileChooser) << "FileChooser failed";
    }
    // TODO: more properties in results map, like current_filter, choices, etc
    // uris (as)
    results.insert(QStringLiteral("uris"), m_callResult);
    // choices (a(ss))
    //results.insert(QStringLiteral("choices"), );
    // current_filter ((sa(us)))
    //results.insert(QStringLiteral("current_filter"), );
    // writable (b)
    //results.insert(QStringLiteral("writable"), );
    qCDebug(XdgDesktopPortalAmberFileChooser) << "Returning:" << m_callResponseCode << results;
    return (uint)m_callResponseCode;
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
void FileChooserPortal::handlePickerError()
{
    qCDebug(XdgDesktopPortalAmberFileChooser) << "Picker Response Error.";
    m_callResult = QVariantMap();
    m_callResponseCode = PickerResponse::Other;
    m_responseHandled = true;
}
void FileChooserPortal::handlePickerResponse(
                        const int &code,
                        const QVariantList &result)
                        //const QString &result)
                        //const QVariantMap &results)
{
    qCDebug(XdgDesktopPortalAmberFileChooser) << "Picker Response received:" << code << result;
    m_callResult = result;
    m_callResponseCode = static_cast<PickerResponse>(code);
    m_responseHandled = true;
}

void FileChooserPortal::setupPickerResponse()
{
    if(!QDBusConnection::sessionBus().connect(
                    QStringLiteral("org.freedesktop.impl.portal.desktop.amber.ui"),
                    QStringLiteral("/org/freedesktop/impl/portal/desktop/amber/ui"),
                    QStringLiteral("org.freedesktop.impl.portal.desktop.amber.ui"),
                    QStringLiteral("pickerDone"),
                    QStringLiteral("iav"),
                    this,
                    SLOT(handlePickerResponse(int, QVariantList))
                    ))
    {
        qCDebug(XdgDesktopPortalAmberFileChooser) << "Could not set up signal listener";
    } else {
        qCDebug(XdgDesktopPortalAmberFileChooser) << "Successfully set up signal listener";
    }
}
void FileChooserPortal::waitForPickerResponse()
{
    // loop until we got the signal
    while (!m_responseHandled) {
        QCoreApplication::processEvents();
        QThread::msleep(250);
        qCDebug(XdgDesktopPortalAmberFileChooser) << "Waiting for Picker...";
    }
    qCDebug(XdgDesktopPortalAmberFileChooser) << "OK, Picker done.";
}
} // namespace Amber
