// SPDX-FileCopyrightText: 2018 Red Hat Inc
// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
// SPDX-FileCopyrightText: 2024 Sailfish OS Community
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "screenshot.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QUrl>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QDateTime>
#include <QColor>

Q_LOGGING_CATEGORY(XDPortalSailfishScreenshot, "xdp-sailfish-screenshot")
Q_DECLARE_METATYPE(Sailfish::XDP::ScreenshotPortal::ColorRGB)

/*! \property Sailfish::XDP::ScreenshotPortal::version
    \brief Contains the backend implementation version
*/

QDBusArgument &operator<<(QDBusArgument &arg, const Sailfish::XDP::ScreenshotPortal::ColorRGB &color)
{
    arg.beginStructure();
    arg << color.red << color.green << color.blue;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, Sailfish::XDP::ScreenshotPortal::ColorRGB &color)
{
    double red, green, blue;
    arg.beginStructure();
    arg >> red >> green >> blue;
    color.red = red;
    color.green = green;
    color.blue = blue;
    arg.endStructure();

    return arg;
}

QDBusArgument &operator<<(QDBusArgument &argument, const QColor &color)
{
    argument.beginStructure();
    argument << color.rgba();
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QColor &color)
{
    argument.beginStructure();
    QRgb rgba;
    argument >> rgba;
    argument.endStructure();
    color = QColor::fromRgba(rgba);
    return argument;
}

namespace Sailfish {
namespace XDP {
ScreenshotPortal::ScreenshotPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qDBusRegisterMetaType<QColor>();
    qDBusRegisterMetaType<ColorRGB>();
    qCDebug(XDPortalSailfishScreenshot) << "Desktop portal service: Screenshot";
    qCDebug(XDPortalSailfishScreenshot) << "Desktop portal service: ColorPicker";
}

/*! \fn uint Sailfish::XDP::ScreenshotPortal::PickColor(const QDBusObjectPath &handle, const QString &app_id, const QString &parent_window, const QVariantMap &options, QVariantMap &results)
     See the \l{XDG Desktop Portal Backend Specification} for the meaning of \a handle, \a app_id, \a parent_window, and \a results.
     See the \l{XDG Desktop Portal Specification} for possible \a options.

     \warning This method currently will always return the same color, white.
*/

uint ScreenshotPortal::PickColor(const QDBusObjectPath &handle,
                                 const QString &app_id,
                                 const QString &parent_window,
                                 const QVariantMap &options,
                                 QVariantMap &results)
{
    qCDebug(XDPortalSailfishScreenshot) << "Start ColorPicker";
    // Fixme: we do have a color Picker in Silica, but no standalone app to show it.
    /*
    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.KWin"),
                                                      QStringLiteral("/ColorPicker"),
                                                      QStringLiteral("org.kde.kwin.ColorPicker"),
                                                      QStringLiteral("pick"));
    QDBusPendingReply<QColor> pcall = QDBusConnection::sessionBus().call(msg);
    if (pcall.isValid()) {
        QColor selectedColor = pcall.value();
        ColorRGB color;
        color.red = selectedColor.redF();
        color.green = selectedColor.greenF();
        color.blue = selectedColor.blueF();
        results.insert(QStringLiteral("color"), QVariant::fromValue<ScreenshotPortal::ColorRGB>(color));
        return 0;
    }
    qCDebug(XDPortalSailfishScreenshot) << "ColorPicker Failed";
    */
    qCDebug(XDPortalSailfishScreenshot) << "ColorPicker not implemented. Returning white.";
    ColorRGB color;
    color.red = 1.0;
    color.green = 1.0;
    color.blue = 1.0;
    results.insert(QStringLiteral("color"), QVariant::fromValue<ScreenshotPortal::ColorRGB>(color));
    return 0;
}

/*! \fn uint Sailfish::XDP::ScreenshotPortal::Screenshot(const QDBusObjectPath &handle, const QString &app_id, const QString &parent_window, const QVariantMap &options, QVariantMap &results)
    Takes a screenshot and returns its file location. The output location will be QStandardPaths::PicturesLocation, and the screenshot is saved in PNG format.
     See the \l{XDG Desktop Portal Backend Specification} for the meaning of \a handle, \a app_id, \a parent_window, and \a results.
     See the \l{XDG Desktop Portal Specification} for possible \a options.

    To take the screenshot, \c org.nemomobile.lipstick.saveScreenshot is called via D-Bus.
*/
uint ScreenshotPortal::Screenshot(const QDBusObjectPath &handle,
                                  const QString &app_id,
                                  const QString &parent_window,
                                  const QVariantMap &options,
                                  QVariantMap &results)
{
    qCDebug(XDPortalSailfishScreenshot) << "Screenshot called with parameters:";
    qCDebug(XDPortalSailfishScreenshot) << "    handle: " << handle.path();
    qCDebug(XDPortalSailfishScreenshot) << "    app_id: " << app_id;
    qCDebug(XDPortalSailfishScreenshot) << "    parent_window: " << parent_window;
    qCDebug(XDPortalSailfishScreenshot) << "    options: " << options;

    if (!options.isEmpty()) {
        qCDebug(XDPortalSailfishScreenshot) << "Screenshot options not supported.";
    }

    qCDebug(XDPortalSailfishScreenshot) << "Asking Lipstick for a screenshot";
    //QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("com.jolla.lipstick"),
    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.nemomobile.lipstick"),
                                                      QStringLiteral("/org/nemomobile/lipstick/screenshot"),
                                                      QStringLiteral("org.nemomobile.lipstick"),
                                                      QStringLiteral("saveScreenshot"));
    QList<QVariant> args;
    // TODO: L10N (?)
    QString filepath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
            + QStringLiteral("/Screenshot-")
            + QDateTime::currentDateTime().toString(Qt::ISODate)
            + QStringLiteral(".png");
    args.append(filepath);
    msg.setArguments(args);

    qCDebug(XDPortalSailfishScreenshot) << "Proposed path is"<< filepath;
    QDBusPendingReply<QString> pcall = QDBusConnection::sessionBus().call(msg);
    pcall.waitForFinished();
    if (pcall.isValid()) {
        qCDebug(XDPortalSailfishScreenshot) << "Success:" << QString("Filepath is %1").arg(filepath);
        results.insert(QStringLiteral("uri"), QUrl::fromLocalFile(filepath).toString(QUrl::FullyEncoded));
        return 0;
    }
    qCDebug(XDPortalSailfishScreenshot) << "Screenshot failed:" << pcall.error().name() << pcall.error().message();
    return 1;
}
} // namespace XDP
} // namespace Sailfish
