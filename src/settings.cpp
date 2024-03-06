// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
// SPDX-FileCopyrightText: 2024 Sailfish OS Community
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "settings.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QColor>
#include <QLoggingCategory>

#include <mlite5/MGConfItem>

Q_LOGGING_CATEGORY(XdgDesktopPortalAmberSettings, "xdp-amber-settings")
Q_DECLARE_METATYPE(Amber::SettingsPortal::ColorRGB)


namespace Amber {
const char* SettingsPortal::THEME_DCONF_SCHEME_KEY         = "/desktop/jolla/theme/colorScheme";
const char* SettingsPortal::THEME_DCONF_HIGHLIGHT_KEY      = "/desktop/jolla/theme/color/highlight";

SettingsPortal::SettingsPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
    , m_schemeConfig(new MGConfItem(THEME_DCONF_SCHEME_KEY, this))
    , m_accentColorConfig(new MGConfItem(THEME_DCONF_HIGHLIGHT_KEY, this))
{
    qCDebug(XdgDesktopPortalAmberSettings) << "Desktop portal service: Settings";
    //QObject::connect(m_schemeConfig,      SIGNAL(valueChanged()), this, SIGNAL(SettingsChanged()));
    //QObject::connect(m_accentColorConfig, SIGNAL(valueChanged()), this, SIGNAL(SettingsChanged()));
}

SettingsPortal::~SettingsPortal()
{
}

QMap<QString, QMap<QString, QDBusVariant>> SettingsPortal::ReadAll(const QStringList &nss)
{
    qCDebug(XdgDesktopPortalAmberSettings) << "Settings called with parameters:";
    qCDebug(XdgDesktopPortalAmberSettings) << "    namespaces: " << nss;

    // TODO
    Q_UNUSED(nss);

    // “(a{sa{sv}})”
    QMap<QString, QMap<QString, QDBusVariant>> value;
    QVariantMap ofa;
    ofa.insert(QStringLiteral("color-scheme"), QVariant(getColorScheme()));
    ColorRGB accent =  getAccentColor();
    ofa.insert(QStringLiteral("accent-color"), QVariantList( { accent.red, accent.green, accent.blue }));
    ofa.insert(QStringLiteral("contrast"), QVariant(getContrast()));

    value.insert(QStringLiteral("org.freedesktop.appearance"), ofa);

    qCDebug(XdgDesktopPortalAmberSettings) << "Returning:" << value;
    return value;
}

QDBusVariant SettingsPortal::Read(const QString &ns,
                              const QString &key)
{
    qCDebug(XdgDesktopPortalAmberSettings) << "Settings called with parameters:";
    qCDebug(XdgDesktopPortalAmberSettings) << "    namespace: " << ns;
    qCDebug(XdgDesktopPortalAmberSettings) << "          key: " << key;
    QVariant value;

    // TODO
    Q_UNUSED(ns);

    if (key == QStringLiteral("color-scheme")) {
        value = QVariant(getColorScheme());
    } else if (key == QStringLiteral("contrast")) {
        value = QVariant(getContrast());
    } else if (key == QStringLiteral("accent-color")) {
        ColorRGB accent =  getAccentColor();
        value = QVariant::fromValue<SettingsPortal::ColorRGB>(accent);
    } else {
        qCDebug(XdgDesktopPortalAmberSettings) << "Unsupported key: " << key;
        value = QVariant();
    }
    return value;
}

SettingsPortal::ColorScheme SettingsPortal::getColorScheme() const
{

  ColorScheme ret = ColorScheme::None;
  uint set = m_schemeConfig->value().toUInt();

  if ( set == (uint) ThemeColorScheme::DarkOnLight )
    ret = ColorScheme::Dark;
  if ( set == (uint) ThemeColorScheme::LightOnDark )
    ret = ColorScheme::Light;

  return ret;
}
SettingsPortal::ColorRGB SettingsPortal::getAccentColor() const
{
  ColorRGB ret = { 0.0, 0.0, 0.0 };
  QColor set;
  set.setNamedColor(m_accentColorConfig->value().toString());
  if (set.isValid()) {
    ret.red = set.redF();
    ret.green = set.greenF();
    ret.blue = set.blueF();
  }
  return ret;
}
} // namespace Amber
