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

/*! \property Sailfish::FileChooserPortal::version
    \brief Contains the backend implementation version
*/
/*! \enum Sailfish::FileChooserPortal::PickerResponse

    Possible return values of a dialog interaction. The values correspond to the \a response poperties of the calls in this class.

    \value  Accepted
        The dialog has been accepted
    \value  Cancelled
        The dialog has been cancelled
    \value  Other
        Something else has happened (e.g. a timeout)
*/

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


/*!  \fn void Sailfish::FileChooserPortal::OpenFile(const QDBusObjectPath &handle, const QString &app_id, const QString &parent_window, const QString &title, const QVariantMap &options)

     Presents a file selection popup to the user. If \a title is given, it will be the title of the dialog window.
     See the \l{XDG Desktop Portal Backend Specification} for the meaning of \a handle, \a app_id, \a parent_window.
     See the \l{XDG Desktop Portal Specification} for possible \a options.
*/
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

/*!  \fn void Sailfish::FileChooserPortal::SaveFile(const QDBusObjectPath &handle, const QString &app_id, const QString &parent_window, const QString &title, const QVariantMap &options)

     Presents a file selection popup to the user. If \a title is given, it will be the title of the dialog window.
     See the \l{XDG Desktop Portal Backend Specification} for the meaning of \a handle, \a app_id, \a parent_window.
     See the \l{XDG Desktop Portal Specification} for possible \a options.

     \warning This currently does nothing.
*/
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

/*!  \fn void Sailfish::FileChooserPortal::SaveFiles(const QDBusObjectPath &handle, const QString &app_id, const QString &parent_window, const QString &title, const QVariantMap &options)

     Presents a file selection popup to the user. If \a title is given, it will be the title of the dialog window.
     See the \l{XDG Desktop Portal Backend Specification} for the meaning of \a handle, \a app_id, \a parent_window.
     See the \l{XDG Desktop Portal Specification} for possible \a options.

     \warning This currently does nothing.
*/
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

/*! \fn void Sailfish::FileChooserPortal::handlePickerError()

    Receives the results from the picker Dialog.

    \sa uidoc
    \internal
*/
void FileChooserPortal::handlePickerError()
{
    qCDebug(XdgDesktopPortalSailfishFileChooser) << "Picker Response Error.";
    m_callResult = QStringList();
    m_callResponseCode = PickerResponse::Other;
    m_responseHandled = true;
}
/*! \fn void Sailfish::FileChooserPortal::handlePickerResponse( const int &code, const QVariantList &result)

    Receives the results from the picker Dialog. \a code corresponds to the
    dialog response options, \a result ia an array of string variants listing the
    selected file(s) or directories.

    \note \a result is a QVariantList mainly because the Nemo.DBus plugin
    prefers to marshal most things as Variants. If you use the results in a DBus
    reply, make sure to transform it into e.g. a simple QStringList before sending.

    \sa uidoc, Sailfish::FileChooserPortal::PickerResponse, Nemo.DBus
    \internal
*/
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

/*! \fn void Sailfish::FileChooserPortal::setupPickerResponse()

    After the GUI has been launched, listens for the \c pickerDone signal, and
    calls FileChooserPortal::handlePickerResponse with the response.

    \sa uidoc, handlePickerResponse
    \internal
*/
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
/*! \fn void Sailfish::FileChooserPortal::waitForPickerResponse()
    Since launching the GUI via D-Bus is asynchronous (or rather, returns
    immediately), we have to wait for a Done signal to arrive from the application.

    \internal
*/
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
