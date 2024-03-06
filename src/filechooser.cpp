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
#include <QDBusContext>
#include <QDBusVariant>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QThread>

Q_LOGGING_CATEGORY(XdgDesktopPortalAmberFileChooser, "xdp-amber-filechooser")

namespace Amber {
FileChooserPortal::FileChooserPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qDBusRegisterMetaType<QMap<QString, QVariantList>>();

    qCDebug(XdgDesktopPortalAmberFileChooser) << "Desktop portal service: FileChooser";
    m_responseHandled = false;
}

FileChooserPortal::~FileChooserPortal()
{
}


void FileChooserPortal::OpenFile(const QDBusObjectPath &handle,
                                const QString &app_id,
                                const QString &parent_window,
                                const QString &title,
                                const QVariantMap &options)
{
    qCDebug(XdgDesktopPortalAmberFileChooser) << "FileChooser.OpenFile called with parameters:";
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    title: " << title;
    qCDebug(XdgDesktopPortalAmberFileChooser) << "    options: " << options;

    // TODO choices
    //Q_UNUSED(results);

    // clean up from last time:
    m_callResult = QVariantList();
    m_callResponseCode = PickerResponse::Other;
    m_responseHandled = false;

    QObject *pHandle = new QObject(this);
    QDBusConnection::sessionBus().registerObject(handle.path(), pHandle, QDBusConnection::ExportScriptableContents);

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

    // ugly hack copied over from KDE:
    QObject *obj = QObject::parent();
    if (!obj) {
        qCWarning(XdgDesktopPortalAmberFileChooser) << "Failed to get dbus context for reply";
        return;
    }
    void *ptr = obj->qt_metacast("QDBusContext");
    QDBusContext *q_ptr = reinterpret_cast<QDBusContext *>(ptr);
    if (!q_ptr) {
        qCWarning(XdgDesktopPortalAmberFileChooser) << "Failed to get dbus context for reply";
        return;
    }
    QDBusMessage reply;
    QDBusMessage message = q_ptr->message();

    reply.setDelayedReply(true);
    //“(ua{sv})”
    reply = message.createReply(QVariant());
    // TODO: more properties in results map, like current_filter, choices, etc
    // uris (as)
    // choices (a(ss))
    // current_filter ((sa(us)))
    // writable (b)
    reply.setArguments({
        (uint)m_callResponseCode, QVariantMap {
            {"uris",  QVariant::fromValue(m_callResult) }
        }
    });

    qCDebug(XdgDesktopPortalAmberFileChooser) << "Returning:" << reply.arguments();
    QDBusConnection::sessionBus().send(reply);

    pHandle->deleteLater();

}

void FileChooserPortal::SaveFile(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QString &title,
                      const QVariantMap &options)
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
    qCDebug(XdgDesktopPortalAmberFileChooser) << "This dialog is not implemented.";

}
void FileChooserPortal::SaveFiles(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QString &title,
                      const QVariantMap &options)
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
    qCDebug(XdgDesktopPortalAmberFileChooser) << "This dialog is not implemented.";
}
void FileChooserPortal::handlePickerError()
{
    qCDebug(XdgDesktopPortalAmberFileChooser) << "Picker Response Error.";
    m_callResult = QVariantList();
    m_callResponseCode = PickerResponse::Other;
    m_responseHandled = true;
}
void FileChooserPortal::handlePickerResponse(
                        const int &code,
                        const QVariantList &result)
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
