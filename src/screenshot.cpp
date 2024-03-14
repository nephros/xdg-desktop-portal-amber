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

Q_LOGGING_CATEGORY(XdgDesktopPortalSailfishScreenshot, "xdp-sailfish-screenshot")
Q_DECLARE_METATYPE(Sailfish::ScreenshotPortal::ColorRGB)

/*! \property Sailfish::ScreenshotPortal::version
    \brief Contains the backend implementation version
*/

QDBusArgument &operator<<(QDBusArgument &arg, const Sailfish::ScreenshotPortal::ColorRGB &color)
{
    arg.beginStructure();
    arg << color.red << color.green << color.blue;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, Sailfish::ScreenshotPortal::ColorRGB &color)
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
ScreenshotPortal::ScreenshotPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qDBusRegisterMetaType<QColor>();
    qDBusRegisterMetaType<ColorRGB>();
    qCDebug(XdgDesktopPortalSailfishScreenshot) << "Desktop portal service: Screenshot";
    qCDebug(XdgDesktopPortalSailfishScreenshot) << "Desktop portal service: ColorPicker";
}

/*! \fn uint Amber::ScreenshotPortal::PickColor(const QDBusObjectPath &handle, const QString &app_id, const QString &parent_window, const QVariantMap &options, QVariantMap &results)
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
    qCDebug(XdgDesktopPortalSailfishScreenshot) << "Start ColorPicker";
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
    qCDebug(XdgDesktopPortalSailfishScreenshot) << "ColorPicker Failed";
    */
    qCDebug(XdgDesktopPortalSailfishScreenshot) << "ColorPicker not implemented. Returning white.";
    ColorRGB color;
    color.red = 1.0;
    color.green = 1.0;
    color.blue = 1.0;
    results.insert(QStringLiteral("color"), QVariant::fromValue<ScreenshotPortal::ColorRGB>(color));
    return 0;
}

/*! \fn uint Amber::ScreenshotPortal::Screenshot(const QDBusObjectPath &handle, const QString &app_id, const QString &parent_window, const QVariantMap &options, QVariantMap &results)
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
    qCDebug(XdgDesktopPortalSailfishScreenshot) << "Screenshot called with parameters:";
    qCDebug(XdgDesktopPortalSailfishScreenshot) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalSailfishScreenshot) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalSailfishScreenshot) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalSailfishScreenshot) << "    options: " << options;

    if (!options.isEmpty()) {
        qCDebug(XdgDesktopPortalSailfishScreenshot) << "Screenshot options not supported.";
    }

    qCDebug(XdgDesktopPortalSailfishScreenshot) << "Asking Lipstick for a screenshot";
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

    qCDebug(XdgDesktopPortalSailfishScreenshot) << "Proposed path is"<< filepath;
    QDBusPendingReply<QString> pcall = QDBusConnection::sessionBus().call(msg);
    pcall.waitForFinished();
    if (pcall.isValid()) {
        qCDebug(XdgDesktopPortalSailfishScreenshot) << "Success:" << QString("Filepath is %1").arg(filepath);
        results.insert(QStringLiteral("uri"), QUrl::fromLocalFile(filepath).toString(QUrl::FullyEncoded));
        return 0;
    }
    qCDebug(XdgDesktopPortalSailfishScreenshot) << "Screenshot failed:" << pcall.error().name() << pcall.error().message();
    return 1;
}
} // namespace Sailfish
