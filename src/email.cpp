// SPDX-FileCopyrightText: 2024 Sailfish OS Community
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "email.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(XdgDesktopPortalAmberEmail, "xdp-amber-email")

namespace Amber {
EmailPortal::EmailPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qCDebug(XdgDesktopPortalAmberEmail) << "Desktop portal service: Email";
}

EmailPortal::~EmailPortal()
{
}

uint EmailPortal::ComposeEmail(const QDBusObjectPath &handle,
                                  const QString &app_id,
                                  const QString &parent_window,
                                  const QVariantMap &options,
                                  QVariantMap &results)
{
    qCDebug(XdgDesktopPortalAmberEmail) << "Email called with parameters:";
    qCDebug(XdgDesktopPortalAmberEmail) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalAmberEmail) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalAmberEmail) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalAmberEmail) << "    options: " << options;

    qCDebug(XdgDesktopPortalAmberEmail) << "Opening Email composer";
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
        qCDebug(XdgDesktopPortalAmberEmail) << "The attachment option is not supported";
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
        qCDebug(XdgDesktopPortalAmberEmail) << "Success";
        return 0;
    }
    qCDebug(XdgDesktopPortalAmberEmail) << "Email failed:" << pcall.error().name() << pcall.error().message();
    return 1;
}
} // namespace Amber
