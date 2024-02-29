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

Q_LOGGING_CATEGORY(XdgDesktopPortalAmberScreenshot, "xdp-amber-screenshot")
Q_DECLARE_METATYPE(Amber::ScreenshotPortal::ColorRGB)

QDBusArgument &operator<<(QDBusArgument &arg, const Amber::ScreenshotPortal::ColorRGB &color)
{
    arg.beginStructure();
    arg << color.red << color.green << color.blue;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, Amber::ScreenshotPortal::ColorRGB &color)
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

namespace Amber {
ScreenshotPortal::ScreenshotPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qDBusRegisterMetaType<QColor>();
    qDBusRegisterMetaType<ColorRGB>();
    qCDebug(XdgDesktopPortalAmberScreenshot) << "Desktop portal service: Screenshot";
    qCDebug(XdgDesktopPortalAmberScreenshot) << "Desktop portal service: ColorPicker";
}

uint ScreenshotPortal::PickColor(const QDBusObjectPath &handle,
                                 const QString &app_id,
                                 const QString &parent_window,
                                 const QVariantMap &options,
                                 QVariantMap &results)
{
    qCDebug(XdgDesktopPortalAmberScreenshot) << "Start ColorPicker";
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
    qCDebug(XdgDesktopPortalAmberScreenshot) << "ColorPicker Failed";
    */
    qCDebug(XdgDesktopPortalAmberScreenshot) << "ColorPicker not implemented. Returning white.";
    ColorRGB color;
    color.red = 1.0;
    color.green = 1.0;
    color.blue = 1.0;
    results.insert(QStringLiteral("color"), QVariant::fromValue<ScreenshotPortal::ColorRGB>(color));
    return 0;
}

uint ScreenshotPortal::Screenshot(const QDBusObjectPath &handle,
                                  const QString &app_id,
                                  const QString &parent_window,
                                  const QVariantMap &options,
                                  QVariantMap &results)
{
    qCDebug(XdgDesktopPortalAmberScreenshot) << "Screenshot called with parameters:";
    qCDebug(XdgDesktopPortalAmberScreenshot) << "    handle: " << handle.path();
    qCDebug(XdgDesktopPortalAmberScreenshot) << "    app_id: " << app_id;
    qCDebug(XdgDesktopPortalAmberScreenshot) << "    parent_window: " << parent_window;
    qCDebug(XdgDesktopPortalAmberScreenshot) << "    options: " << options;

    if (!options.isEmpty()) {
        qCDebug(XdgDesktopPortalAmberScreenshot) << "Screenshot options not supported.";
    }

    qCDebug(XdgDesktopPortalAmberScreenshot) << "Asking Lipstick for a screenshot";
    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("com.jolla.lipstick"),
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

    QDBusPendingReply<QString> pcall = QDBusConnection::sessionBus().call(msg);
    pcall.waitForFinished();
    if (pcall.isValid()) {
        qCDebug(XdgDesktopPortalAmberScreenshot) << "Success:" << QString("Filepath is %1").arg(filepath);
        results.insert(QStringLiteral("uri"), QUrl::fromLocalFile(filepath).toString(QUrl::FullyEncoded));
        return 0;
    }
    qCDebug(XdgDesktopPortalAmberScreenshot) << "Screenshot failed";
    return 1;
}
} // namespace Amber
