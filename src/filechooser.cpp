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

Q_LOGGING_CATEGORY(XdgDesktopPortalSailfishFileChooser, "xdp-sailfish-filechooser")

namespace Sailfish {
FileChooserPortal::FileChooserPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qDBusRegisterMetaType<QMap<QString, QVariantList>>();

    qCDebug(XdgDesktopPortalSailfishFileChooser) << "Desktop portal service: FileChooser";
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
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "FileChooser.OpenFile called with parameters:";
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    title: " << title;
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    options: " << options;

    // TODO choices
    //Q_UNUSED(results);

    // clean up from last time:
    m_callResult = QStringList();
    m_callResponseCode = PickerResponse::Other;
    m_responseHandled = false;

    QObject *pHandle = new QObject(this);
    QDBusConnection::sessionBus().registerObject(handle.path(), pHandle, QDBusConnection::ExportScriptableContents);

    qCDebug(XdgDesktopPortalSailfishFileChooser) << "Trying to show a dialog";

    QDBusMessage msg = QDBusMessage::createMethodCall(
                    QStringLiteral("org.freedesktop.impl.portal.desktop.sailfish.ui"),
                    QStringLiteral("/org/freedesktop/impl/portal/desktop/sailfish/ui"),
                    QStringLiteral("org.freedesktop.impl.portal.desktop.sailfish.ui"),
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
        qCDebug(XdgDesktopPortalSailfishFileChooser) << "Success";
    } else {
        qCDebug(XdgDesktopPortalSailfishFileChooser) << "FileChooser failed";
    }

    // ugly hack copied over from KDE:
    QObject *obj = QObject::parent();
    if (!obj) {
        qCWarning(XdgDesktopPortalSailfishFileChooser) << "Failed to get dbus context for reply";
        return;
    }
    void *ptr = obj->qt_metacast("QDBusContext");
    QDBusContext *q_ptr = reinterpret_cast<QDBusContext *>(ptr);
    if (!q_ptr) {
        qCWarning(XdgDesktopPortalSailfishFileChooser) << "Failed to get dbus context for reply";
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

    qCDebug(XdgDesktopPortalSailfishFileChooser) << "Returning:" << reply.arguments();
    QDBusConnection::sessionBus().send(reply);

    pHandle->deleteLater();

}

void FileChooserPortal::SaveFile(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QString &title,
                      const QVariantMap &options)
{
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "FileChooser.SaveFiles called with parameters:";
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    title: " << title;
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    options: " << options;
    /*
    if (!options.isEmpty()) {
            qCDebug(XdgDesktopPortalSailfishFileChooser) << "FileChooser dialog options not supported.";
    }
    */

    // ugly hack copied over from KDE:
    QObject *obj = QObject::parent();
    if (!obj) {
        qCWarning(XdgDesktopPortalSailfishFileChooser) << "Failed to get dbus context for reply";
        return;
    }
    void *ptr = obj->qt_metacast("QDBusContext");
    QDBusContext *q_ptr = reinterpret_cast<QDBusContext *>(ptr);
    if (!q_ptr) {
        qCWarning(XdgDesktopPortalSailfishFileChooser) << "Failed to get dbus context for reply";
        return;
    }
    QDBusMessage message = q_ptr->message();

    qCDebug(XdgDesktopPortalSailfishFileChooser) << "This method is not implemented";
    QDBusMessage reply = message.createErrorReply(QDBusError::NotSupported, QStringLiteral("This method is not imlemented"));
    QDBusConnection::sessionBus().send(reply);

}

void FileChooserPortal::SaveFiles(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QString &title,
                      const QVariantMap &options)
{
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "FileChooser.SaveFile called with parameters:";
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    title: " << title;
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "    options: " << options;
    /*
    if (!options.isEmpty()) {
            qCDebug(XdgDesktopPortalSailfishFileChooser) << "FileChooser dialog options not supported.";
    }
    */

    // ugly hack copied over from KDE:
    QObject *obj = QObject::parent();
    void *ptr = obj->qt_metacast("QDBusContext");
    QDBusContext *q_ptr = reinterpret_cast<QDBusContext *>(ptr);
    if (!q_ptr) {
        qCWarning(XdgDesktopPortalSailfishFileChooser) << "Failed to get dbus context for reply";
        return;
    }
    QDBusMessage message = q_ptr->message();

    qCDebug(XdgDesktopPortalSailfishFileChooser) << "This method is not implemented";
    QDBusMessage reply = message.createErrorReply(QDBusError::NotSupported, QStringLiteral("This method is not imlemented"));
    QDBusConnection::sessionBus().send(reply);
}

void FileChooserPortal::handlePickerError()
{
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "Picker Response Error.";
    m_callResult = QStringList();
    m_callResponseCode = PickerResponse::Other;
    m_responseHandled = true;
}
void FileChooserPortal::handlePickerResponse(
                        const int &code,
                        const QVariantList &result)
{
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "Picker Response received:" << code << result;
    foreach(QVariant v, result) {
      m_callResult << v.toString();
    }
    m_callResponseCode = static_cast<PickerResponse>(code);
    m_responseHandled = true;
}

void FileChooserPortal::setupPickerResponse()
{
    if(!QDBusConnection::sessionBus().connect(
                    QStringLiteral("org.freedesktop.impl.portal.desktop.sailfish.ui"),
                    QStringLiteral("/org/freedesktop/impl/portal/desktop/sailfish/ui"),
                    QStringLiteral("org.freedesktop.impl.portal.desktop.sailfish.ui"),
                    QStringLiteral("pickerDone"),
                    QStringLiteral("iav"),
                    this,
                    SLOT(handlePickerResponse(int, QVariantList))
                    ))
    {
        qCDebug(XdgDesktopPortalSailfishFileChooser) << "Could not set up signal listener";
    } else {
        qCDebug(XdgDesktopPortalSailfishFileChooser) << "Successfully set up signal listener";
    }
}
void FileChooserPortal::waitForPickerResponse()
{
    // loop until we got the signal
    while (!m_responseHandled) {
        QCoreApplication::processEvents();
        QThread::msleep(250);
        qCDebug(XdgDesktopPortalSailfishFileChooser) << "Waiting for Picker...";
    }
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "OK, Picker done.";
}
} // namespace Sailfish
