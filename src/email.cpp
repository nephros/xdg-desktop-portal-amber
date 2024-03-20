/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 */

#include "email.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(XDPortalSailfishEmail, "xdp-sailfish-email")

namespace Sailfish {
namespace XDP {
EmailPortal::EmailPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XDPortalSailfishEmail) << "Desktop portal service: Email";
}

EmailPortal::~EmailPortal()
{
}

/*! \fn uint Sailfish::XDP::EmailPortal::ComposeEmail(const QDBusObjectPath &handle, const QString &app_id, const QString &parent_window, const QVariantMap &options, QVariantMap &results)

     Opens the EMail application to create a new email.

     See the \l{XDG Desktop Portal Specification} for possible values of \a options.
     See the \l{XDG Desktop Portal Backend Specification} for the meaning of \a handle, \a app_id, \a parent_window, \a results.

     \note Due to a limitation of Sailfish OS, attachments are currently not supported.
*/
uint EmailPortal::ComposeEmail(const QDBusObjectPath &handle,
                                  const QString &app_id,
                                  const QString &parent_window,
                                  const QVariantMap &options,
                                  QVariantMap &results)
{
    qCDebug(XDPortalSailfishEmail) << "Email called with parameters:";
    qCDebug(XDPortalSailfishEmail) << "    handle: " << handle.path();
    qCDebug(XDPortalSailfishEmail) << "    app_id: " << app_id;
    qCDebug(XDPortalSailfishEmail) << "    parent_window: " << parent_window;
    qCDebug(XDPortalSailfishEmail) << "    options: " << options;

    qCDebug(XDPortalSailfishEmail) << "Opening Email composer";
    QDBusMessage msg = QDBusMessage::createMethodCall(
                    QStringLiteral("com.jolla.email.ui"),
                    QStringLiteral("/com/jolla/email/ui"),
                    QStringLiteral("com.jolla.email.ui"),
                    QStringLiteral("compose")
                    );
                    //QStringLiteral("mailto"),
    /*
     * .compose    method    sssss
     * to, subj, cc, bcc, body
     * .mailto     method    as
     *
     * <interface name="com.jolla.email.ui">
     * <method name="compose">
     * <arg name="emailSubject" type="s" direction="in" />
     * <arg name="emailTo" type="s" direction="in" />
     * <arg name="emailCc" type="s" direction="in" />
     * <arg name="emailBcc" type="s" direction="in" />
     * <arg name="emailBody" type="s" direction="in" />
     * </method>
     *
     *
     * service: "com.jolla.email.ui"
     * path: "/share"
     * iface: "org.sailfishos.share"
     */
    QList<QVariant> args;
    QString subject;
    QString to;
    QString cc;
    QString bcc;
    QString body;
    if (options.contains(QStringLiteral("address"))) { // single recipient, s
        to = options.value(QStringLiteral("address")).toString();
    } else if (options.contains(QStringLiteral("addresses"))) { // recipients, as
        to = options.value(QStringLiteral("address")).toString();
    }
    if (options.contains(QStringLiteral("cc"))) { // recipients, as
        cc = options.value(QStringLiteral("cc")).toString();
    }
    if (options.contains(QStringLiteral("bcc"))) { // recipients, as
        bcc = options.value(QStringLiteral("bcc")).toString();
    }
    if (options.contains(QStringLiteral("subject"))) {
        subject = options.value(QStringLiteral("subject")).toString();
    }
    if (options.contains(QStringLiteral("body"))) {
        body = options.value(QStringLiteral("body")).toString();
    }
    if (options.contains(QStringLiteral("attachments"))) { // as array of urls
        // probably needs ShareAction or so.
        qCDebug(XDPortalSailfishEmail) << "The attachment option is not supported";
    }
    args.append(subject);
    args.append(to);
    args.append(cc);
    args.append(bcc);
    args.append(body);
    msg.setArguments(args);

    QDBusPendingReply<QString> pcall = QDBusConnection::sessionBus().call(msg);
    pcall.waitForFinished();
    if (pcall.isValid()) {
        qCDebug(XDPortalSailfishEmail) << "Success";
        // return something in results just so signature is correct
        results.insert("success", QVariant(true));
        return 0;
    }
    qCDebug(XDPortalSailfishEmail) << "Email failed:" << pcall.error().name() << pcall.error().message();
    // return something in results just so signature is correct
    results.insert("success", QVariant(false));
    return 1;
}
} // namespace XDP
} // namespace Sailfish
