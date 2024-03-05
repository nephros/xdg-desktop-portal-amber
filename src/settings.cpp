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
const char* SettingsPortal::THEME_DCONF_SCHEME_KEY         = "/desktop/jolla/theme/color_scheme";
const char* SettingsPortal::THEME_DCONF_HIGHLIGHT_KEY      = "/desktop/jolla/theme/color/highlight";

const char* SettingsPortal::NAMESPACE_OFDA_KEY     = "org.freedesktop.appearance";

const char* SettingsPortal::CONFIG_ACCENT_KEY      = "accent-color";
const char* SettingsPortal::CONFIG_SCHEME_KEY      = "color-scheme";
const char* SettingsPortal::CONFIG_CONTRAST_KEY    = "contrast";

SettingsPortal::SettingsPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
    , m_schemeConfig(new MGConfItem(THEME_DCONF_SCHEME_KEY, this))
    , m_accentColorConfig(new MGConfItem(THEME_DCONF_HIGHLIGHT_KEY, this))
{
    qCDebug(XdgDesktopPortalAmberSettings) << "Desktop portal service: Settings";
    QObject::connect(m_schemeConfig,      SIGNAL(valueChanged()), this, SLOT(valueChanged(CONFIG_SCHEME_KEY)));
    QObject::connect(m_accentColorConfig, SIGNAL(valueChanged()), this, SLOT(valueChanged(CONFIG_ACCENT_KEY)));
}

SettingsPortal::~SettingsPortal()
{
}

void SettingsPortal::ReadAll(const QStringList &nss,
                                   QVariantMap &value)
{
    qCDebug(XdgDesktopPortalAmberSettings) << "Settings called with parameters:";
    qCDebug(XdgDesktopPortalAmberSettings) << "    namespaces: " << nss;

    // TODO
    Q_UNUSED(nss);

    QVariantMap ofda;
    ColorRGB accent =  getAccentColor();
    ofda.insert(CONFIG_SCHEME_KEY, QVariant(getColorScheme()));
    ofda.insert(CONFIG_ACCENT_KEY, QVariantList( { accent.red, accent.green, accent.blue }));
    ofda.insert(CONFIG_CONTRAST_KEY, QVariant(getContrast()));

    value.insert(NAMESPACE_OFDA_KEY, ofda);

    return;
}

void SettingsPortal::Read(const QString &ns,
                          const QString &key,
                                QVariant &value)
{
    qCDebug(XdgDesktopPortalAmberSettings) << "Settings called with parameters:";
    qCDebug(XdgDesktopPortalAmberSettings) << "    namespace: " << ns;
    qCDebug(XdgDesktopPortalAmberSettings) << "          key: " << key;

    // TODO
    Q_UNUSED(ns);

    if (key == CONFIG_SCHEME_KEY) {
        value = QVariant(getColorScheme());
    } else if (key == CONFIG_CONTRAST_KEY) {
        value = QVariant(getContrast());
    } else if (key == CONFIG_ACCENT_KEY) {
        ColorRGB accent =  getAccentColor();
        value = QVariant::fromValue<SettingsPortal::ColorRGB>(accent);
    } else {
        qCDebug(XdgDesktopPortalAmberSettings) << "Unsupported key: " << key;
        value = QVariant();
    }
    return;
}

void SettingsPortal::valueChanged(const QString &what)
{
    if (what == CONFIG_SCHEME_KEY) {
        emit SettingsChanged(NAMESPACE_OFDA_KEY, what, QVariant(getColorScheme()));
    } else if (what == CONFIG_ACCENT_KEY) {
        emit SettingsChanged(NAMESPACE_OFDA_KEY, what, QVariant(getAccentColor()));
    }
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
