// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
// SPDX-FileCopyrightText: 2024 Sailfish OS Community
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "settings.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusVariant>
#include <QColor>
#include <QLoggingCategory>

#include <mlite5/MGConfItem>

Q_LOGGING_CATEGORY(XdgDesktopPortalAmberSettings, "xdp-amber-settings")

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

QMap<QString, QMap<QString, QDBusVariant>> SettingsPortal::ReadAll(const QStringList &nss)
{
    qCDebug(XdgDesktopPortalAmberSettings) << "Settings called with parameters:";
    qCDebug(XdgDesktopPortalAmberSettings) << "    namespaces: " << nss;

    // TODO
    Q_UNUSED(nss);

    // on-the-fly construction of "(a{sa{sv}})"
    return {
        { NAMESPACE_OFDA_KEY, {
            { CONFIG_SCHEME_KEY,   QDBusVariant(getColorScheme()) },
            { CONFIG_ACCENT_KEY,   QDBusVariant(getContrast()) },
            { CONFIG_CONTRAST_KEY, QDBusVariant(QVariant(getAccentColor())) }
        }
        }
    };
}

QDBusVariant SettingsPortal::Read(const QString &ns,
                              const QString &key)
{
    qCDebug(XdgDesktopPortalAmberSettings) << "Settings called with parameters:";
    qCDebug(XdgDesktopPortalAmberSettings) << "    namespace: " << ns;
    qCDebug(XdgDesktopPortalAmberSettings) << "          key: " << key;

    // TODO
    Q_UNUSED(ns);

    if (key == CONFIG_SCHEME_KEY) {
        return QDBusVariant(getColorScheme());
    } else if (key == CONFIG_CONTRAST_KEY) {
        return QDBusVariant(getContrast());
    } else if (key == CONFIG_ACCENT_KEY) {
        return QDBusVariant(QVariant(getAccentColor()));
    }
    qCDebug(XdgDesktopPortalAmberSettings) << "Unsupported key: " << key;
    return QDBusVariant(QVariant()); // QVariant() constructs an invalid variant
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

QList<QVariant> SettingsPortal::getAccentColor() const
{
    QColor set;
    set.setNamedColor(m_accentColorConfig->value().toString());
    if (set.isValid()) {
        return { QVariant(set.redF()), QVariant(set.greenF()), QVariant(set.blueF()) };
    }
}
} // namespace Amber
