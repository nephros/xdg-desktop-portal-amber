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

#include <QCoreApplication>
#include <QThread>
#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QLoggingCategory>
#include <QJsonDocument>
#include <QJsonObject>

Q_LOGGING_CATEGORY(XdgDesktopPortalSailfishAccess, "xdp-sailfish-access")

/*! \enum Sailfish::XDP::AccessPortal::DialogResponse

    Possible return values of a dialog interaction. The values correspond to the \a response poperties of the calls in this class.

    \value  Accepted
        The dialog has been accepted
    \value  Cancelled
        The dialog has been cancelled
    \value  Other
        Something else has happened (e.g. a timeout)
*/

namespace Sailfish {
namespace XDP {
AccessPortal::AccessPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalSailfishAccess) << "Desktop portal service: Access";
    m_responseHandled = false;
}

AccessPortal::~AccessPortal()
{
}


/*! \fn Sailfish::XDP::AccessPortal::AccessDialog(const QDBusObjectPath &handle, const QString &app_id, const QString &parent_window, const QString &title, const QString &subtitle, const QString &body, const QVariantMap &options, QVariantMap &results)
    Presents the user with a prompt they can accept or deny, and several other options.

    \a title, \a subtitle, and \a body can be used to configure the dialog appearance.

    See the \l{XDG Desktop Portal Specification} for possible values of \a options.
    See the \l{XDG Desktop Portal Backend Specification} for the meaning of \a handle, \a app_id, \a parent_window, \a results.

    \warning The implementation of triggering the UI is incomplete. It serves
    as proof-of-concept only. Specifically, some options like \c modal, or \c
    choices are not handled.

*/
uint AccessPortal::AccessDialog(const QDBusObjectPath &handle,
                                const QString &app_id,
                                const QString &parent_window,
                                const QString &title,
                                const QString &subtitle,
                                const QString &body,
                                const QVariantMap &options,
                                QVariantMap &results
                                )
{
    qCDebug(XdgDesktopPortalSailfishAccess) << "AccessDialog called with parameters:";
    qCDebug(XdgDesktopPortalSailfishAccess) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalSailfishAccess) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalSailfishAccess) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalSailfishAccess) << "    title: " << title;
    qCDebug(XdgDesktopPortalSailfishAccess) << "    subtitle: " << subtitle;
    qCDebug(XdgDesktopPortalSailfishAccess) << "    body: " << body;
    qCDebug(XdgDesktopPortalSailfishAccess) << "    options: " << options;

    if (options.contains(QStringLiteral("choices")) || options.contains(QStringLiteral("modal"))) {
            qCDebug(XdgDesktopPortalSailfishAccess) << "Some Access dialog options are not supported.";
    }

    /*
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
    */

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

    qCDebug(XdgDesktopPortalSailfishAccess) << "Trying to show a dialog";

    QDBusMessage msg = QDBusMessage::createMethodCall(
                    QStringLiteral("org.freedesktop.impl.portal.desktop.sailfish.ui"),
                    QStringLiteral("/org/freedesktop/impl/portal/desktop/sailfish/ui"),
                    QStringLiteral("org.freedesktop.impl.portal.desktop.sailfish.ui"),
                    QStringLiteral("confirmationDialog")
                    );


    QList<QVariant> args;
    args.append(handle.path());
    args.append(title);
    args.append(subtitle);
    args.append(body);
    // just pass the options as-is, we can possibly deal with it in the UI:
    QString jopts = QJsonDocument(QJsonObject::fromVariantMap(options)).toJson();
    args.append(jopts);

    msg.setArguments(args);

    // the call will cause the UI to show, but will not wait for user
    // selection.
    // So we set up a signal handler and continue when we have received the
    // signal.
    QDBusConnection::sessionBus().callWithCallback(
                    msg, this,
                    SLOT(setupDialogResponse()),
                    SLOT(handleDialogError())
                    );

    // busy loop ;)
    waitForDialogResponse();

    if (m_callResponseCode != DialogResponse::Other) {
        qCDebug(XdgDesktopPortalSailfishAccess) << "Success";
    } else {
        qCDebug(XdgDesktopPortalSailfishAccess) << "Dialog failed";
    }

    // Just respond with empty list
    // choices (a(ss)) -> QVariant(QList<QStringaList>)
    // results: a{sv}, QVariantMap -> QMap<QString, QVariant>.
    QList<QStringList> choices = {
        {
            { "", "" },
            { "", "" }
        }
    };
    results.insert("choices", QVariant::fromValue(choices));
    return (uint)m_callResponseCode;
}
/*! \fn void Sailfish::XDP::AccessPortal::handleDialogError()

    Receives the results from the Dialog.

    \sa uidoc
    \internal
*/
void AccessPortal::handleDialogError()
{
    qCDebug(XdgDesktopPortalSailfishAccess) << "Dialog Response Error.";
    m_callResponseCode = DialogResponse::Other;
    m_responseHandled = true;
}
/*! \fn void Sailfish::XDP::AccessPortal::handleDialogResponse( const int &code)

    Receives the results from the Dialog. \a code corresponds to the
    dialog response options.

    \sa uidoc, Sailfish::XDP::AccessPortal::DialogResponse, Nemo.DBus
    \internal
*/
void AccessPortal::handleDialogResponse( const int &code )
{
    qCDebug(XdgDesktopPortalSailfishAccess) << "Dialog Response received:" << code;
    m_callResponseCode = static_cast<DialogResponse>(code);
    m_responseHandled = true;
}

/*! \fn void Sailfish::XDP::AccessPortal::setupDialogResponse()

    After the GUI has been launched, listens for the \c dialogDone signal, and
    calls AccessPortal::handleDialogResponse with the response.

    \sa uidoc, handleDialogResponse
    \internal
*/
void AccessPortal::setupDialogResponse()
{
    if(!QDBusConnection::sessionBus().connect(
                    QStringLiteral("org.freedesktop.impl.portal.desktop.sailfish.ui"),
                    QStringLiteral("/org/freedesktop/impl/portal/desktop/sailfish/ui"),
                    QStringLiteral("org.freedesktop.impl.portal.desktop.sailfish.ui"),
                    QStringLiteral("confirmationDone"),
                    QStringLiteral("i"),
                    this,
                    SLOT(handleDialogResponse(int))
                    ))
    {
        qCDebug(XdgDesktopPortalSailfishAccess) << "Could not set up signal listener";
    } else {
        qCDebug(XdgDesktopPortalSailfishAccess) << "Successfully set up signal listener";
    }
}
/*! \fn void Sailfish::XDP::AccessPortal::waitForDialogResponse()
    Since launching the GUI via D-Bus is asynchronous (or rather, returns
    immediately), we have to wait for a Done signal to arrive from the application.

    \internal
*/
void AccessPortal::waitForDialogResponse()
{
    // loop until we got the signal
    while (!m_responseHandled) {
        QCoreApplication::processEvents();
        QThread::msleep(250);
        qCDebug(XdgDesktopPortalSailfishAccess) << "Waiting for Dialog...";
    }
    qCDebug(XdgDesktopPortalSailfishAccess) << "OK, Dialog done.";
}
} // namespace XDP
} // namespace Sailfish
