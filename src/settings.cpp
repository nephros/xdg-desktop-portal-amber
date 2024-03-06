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
const char* SettingsPortal::DCONF_SAILFISHOS_SCHEME_KEY         = "/desktop/jolla/theme/color_scheme";
const char* SettingsPortal::DCONF_SAILFISHOS_HIGHLIGHT_KEY      = "/desktop/jolla/theme/color/highlight";

const char* SettingsPortal::NAMESPACE_FDO     = "org.freedesktop.appearance";

const char* SettingsPortal::CONFIG_FDO_ACCENT_KEY      = "accent-color";
const char* SettingsPortal::CONFIG_FDO_SCHEME_KEY      = "color-scheme";
const char* SettingsPortal::CONFIG_FDO_CONTRAST_KEY    = "contrast";

// TODO: other namespaces, e.g.:
// naming things is hard.
// org.sailfishos.desktop? com.jolla.lipstick? com.jolla.ambienced? org.merproject? com.nokia.meego.harmattan?
const char* SettingsPortal::NAMESPACE_SAILFISHOS = "org.sailfishos.desktop";
const char* SettingsPortal::CONFIG_SAILFISHOS_THEME_SCHEME_KEY      = "color-scheme";
const char* SettingsPortal::CONFIG_SAILFISHOS_THEME_PRIMARY_KEY     = "primary-color";
const char* SettingsPortal::CONFIG_SAILFISHOS_THEME_SECONDARY_KEY   = "secondary-color";
const char* SettingsPortal::CONFIG_SAILFISHOS_THEME_HIGHLIGHT_KEY   = "highlight-color";
const char* SettingsPortal::CONFIG_SAILFISHOS_THEME_SECONDARYHIGHLIGHT_KEY = "secondary-highlight-color";
// perpare to fake KDE Settings
const char* SettingsPortal::NAMESPACE_KDE             = "org.kde.kdeglobals";
const char* SettingsPortal::NAMESPACE_KDE_GENERAL     = "org.kde.kdeglobals.General";
const char* SettingsPortal::CONFIG_KDE_SCHEME_KEY     = "ColorScheme";

SettingsPortal::SettingsPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
    , m_schemeConfig(new MGConfItem(DCONF_SAILFISHOS_SCHEME_KEY, this))
    , m_accentColorConfig(new MGConfItem(DCONF_SAILFISHOS_HIGHLIGHT_KEY, this))
{
    qCDebug(XdgDesktopPortalAmberSettings) << "Desktop portal service: Settings";

    QObject::connect(m_schemeConfig,      SIGNAL(valueChanged()), this, SLOT(valueChanged(SettingsPortal::CONFIG_FDO_SCHEME_KEY)));
    QObject::connect(m_accentColorConfig, SIGNAL(valueChanged()), this, SLOT(valueChanged(SettingsPortal::CONFIG_FDO_ACCENT_KEY)));

}

SettingsPortal::~SettingsPortal()
{
}

void SettingsPortal::ReadAll(const QStringList &nss)
{
    qCDebug(XdgDesktopPortalAmberSettings) << "Settings called with parameters:";
    qCDebug(XdgDesktopPortalAmberSettings) << "    namespaces: " << nss;

    QObject *qo = new QObject(this);
    QDBusMessage reply;
    QDBusMessage message;
    //QDBusVariant result;
    QMap<QString, QMap<QString, QDBusVariant>> result;

    // FIXME: we should support a namespace list:
    if (nss.contains(NAMESPACE_FDO)) {
        // on-the-fly construction of "(a{sa{sv}})"
        result = {
          { NAMESPACE_FDO, {
                  { CONFIG_FDO_SCHEME_KEY,   QDBusVariant(getColorScheme()) },
                  { CONFIG_FDO_ACCENT_KEY,   QDBusVariant(getContrast()) },
                  { CONFIG_FDO_CONTRAST_KEY, QDBusVariant(QVariant(getAccentColor())) }
                }
          }
        };
        reply = message.createReply(QVariant::fromValue(result));
    } else if (nss.contains(NAMESPACE_SAILFISHOS)) {
        qCDebug(XdgDesktopPortalAmberSettings) << "Ahoy Sailor! Namespace" << nss << "is not yet supported";
        reply = message.createErrorReply(QDBusError::UnknownProperty, QStringLiteral("Ahoy Sailor! Namespace is not yet supported"));
    } else {
        qCDebug(XdgDesktopPortalAmberSettings) << "Unknown namespace:" << nss;
        reply = message.createErrorReply(QDBusError::UnknownProperty, QStringLiteral("Namespace is not supported"));
    }
    QDBusConnection::sessionBus().send(reply);
}

QDBusVariant SettingsPortal::Read(const QString &ns,
                              const QString &key)
{
    qCDebug(XdgDesktopPortalAmberSettings) << "Settings called with parameters:";
    qCDebug(XdgDesktopPortalAmberSettings) << "    namespace: " << ns;
    qCDebug(XdgDesktopPortalAmberSettings) << "          key: " << key;

    // TODO
    Q_UNUSED(ns);

    if (key == CONFIG_FDO_SCHEME_KEY) {
        return QDBusVariant(getColorScheme());
    } else if (key == CONFIG_FDO_CONTRAST_KEY) {
        return QDBusVariant(getContrast());
    } else if (key == CONFIG_FDO_ACCENT_KEY) {
        return QDBusVariant(QVariant(getAccentColor()));
    }
    qCDebug(XdgDesktopPortalAmberSettings) << "Unsupported key: " << key;
    return QDBusVariant(QVariant()); // QVariant() constructs an invalid variant
}

void SettingsPortal::valueChanged(const char* &what)
{
    if (what == CONFIG_FDO_SCHEME_KEY) {
        emit SettingsChanged(NAMESPACE_FDO, what, QVariant(getColorScheme()));
    } else if (what == CONFIG_FDO_ACCENT_KEY) {
        emit SettingsChanged(NAMESPACE_FDO, what, QVariant(getAccentColor()));
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
