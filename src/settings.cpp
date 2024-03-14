// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
// SPDX-FileCopyrightText: 2024 Sailfish OS Community
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "settings.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusContext>
#include <QDBusVariant>
#include <QColor>
#include <QLoggingCategory>

#include <mlite5/MGConfItem>
#include <mlite5/MDConfGroup>

Q_LOGGING_CATEGORY(XdgDesktopPortalSailfishSettings, "xdp-sailfish-settings")

namespace Sailfish {
const char* SettingsPortal::DCONF_SAILFISHOS_SCHEME_KEY         = "/desktop/jolla/theme/color_scheme";
const char* SettingsPortal::DCONF_SAILFISHOS_HIGHLIGHT_KEY      = "/desktop/jolla/theme/color/highlight";
const char* SettingsPortal::DCONF_SAILFISHOS_THEME_GROUP        = "/desktop/jolla/theme/color";

const char* SettingsPortal::NAMESPACE_FDO     = "org.freedesktop.appearance";

const char* SettingsPortal::CONFIG_FDO_ACCENT_KEY      = "accent-color";
const char* SettingsPortal::CONFIG_FDO_SCHEME_KEY      = "color-scheme";
const char* SettingsPortal::CONFIG_FDO_CONTRAST_KEY    = "contrast";

// TODO: other namespaces, e.g.:
// naming things is hard.
// org.sailfishos.desktop? com.jolla.lipstick? com.jolla.ambienced? org.merproject? com.nokia.meego-harmattan?
const char* SettingsPortal::NAMESPACE_SAILFISHOS                    = "org.sailfishos.desktop";
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
    , m_sailfishThemeConfigGroup(new MDConfGroup(DCONF_SAILFISHOS_THEME_GROUP, this))
{
    qCDebug(XdgDesktopPortalSailfishSettings) << "Desktop portal service: Settings";

    qDBusRegisterMetaType<QMap<QString, QMap<QString, QDBusVariant>>>();
    qDBusRegisterMetaType<QMap<QString,QDBusVariant>>();

    /* TODO
    QObject::connect(m_schemeConfig,      SIGNAL(valueChanged()), SLOT(valueChanged(const char* what=SettingsPortal::CONFIG_FDO_SCHEME_KEY)));
    QObject::connect(m_accentColorConfig, SIGNAL(valueChanged()), SLOT(valueChanged(const char* what=SettingsPortal::CONFIG_FDO_ACCENT_KEY)));
    */
    QObject::connect(m_sailfishThemeConfigGroup, SIGNAL(valueChanged(QString)), SLOT(valueChanged(QString)));
    // listen for ambience changes:
    QDBusConnection::sessionBus().connect(
                    QStringLiteral("com.jolla.ambienced"),
                    QStringLiteral("/com/jolla/ambienced"),
                    QStringLiteral("com.jolla.ambienced"),
                    QStringLiteral("contentChanged"),
                    QStringLiteral("i"),
                    this,
                    SLOT(ambienceChanged(int))
                    );
}

SettingsPortal::~SettingsPortal()
{
}

void SettingsPortal::ReadAll(const QStringList &nss)
{
    qCDebug(XdgDesktopPortalSailfishSettings) << "Settings called with parameters:";
    qCDebug(XdgDesktopPortalSailfishSettings) << "    namespaces: " << nss;

    // ugly hack copied over from KDE:
    QObject *obj = QObject::parent();
    if (!obj) {
        qCWarning(XdgDesktopPortalSailfishSettings) << "Failed to get dbus context for reply";
        return;
    }
    void *ptr = obj->qt_metacast("QDBusContext");
    QDBusContext *q_ptr = reinterpret_cast<QDBusContext *>(ptr);
    if (!q_ptr) {
        qCWarning(XdgDesktopPortalSailfishSettings) << "Failed to get dbus context for reply";
        return;
    }
    QDBusMessage reply;
    QDBusMessage message = q_ptr->message();
    QMap<QString, QMap<QString, QDBusVariant>> result;

    // FIXME: we should support a namespace list:
    if (nss.contains(NAMESPACE_FDO)) {
        // on-the-fly construction of "(a{sa{sv}})"
        result = {
          { NAMESPACE_FDO, {
                  { CONFIG_FDO_SCHEME_KEY,   QDBusVariant(getColorScheme()) },
                  { CONFIG_FDO_CONTRAST_KEY, QDBusVariant(getContrast()) },
                  { CONFIG_FDO_ACCENT_KEY,   QDBusVariant(QVariant(getAccentColor())) }
                }
          }
        };
        reply = message.createReply(QVariant::fromValue(result));
    } else if (nss.contains(NAMESPACE_SAILFISHOS)) {
        qCDebug(XdgDesktopPortalSailfishSettings) << "Ahoy Sailor!";
        result = {
          { NAMESPACE_SAILFISHOS, {
                  { CONFIG_SAILFISHOS_THEME_SCHEME_KEY, QDBusVariant(getColorScheme()) },
                  { CONFIG_SAILFISHOS_THEME_PRIMARY_KEY,
                         QDBusVariant(m_sailfishThemeConfigGroup->value(QStringLiteral("primary"),"#ffffffff", QMetaType::QString)) },
                  { CONFIG_SAILFISHOS_THEME_SECONDARY_KEY,
                         QDBusVariant(m_sailfishThemeConfigGroup->value(QStringLiteral("secondary"),"#b0ffffff", QMetaType::QString)) },
                  { CONFIG_SAILFISHOS_THEME_SECONDARYHIGHLIGHT_KEY,
                        // F76039 ia approx the color of the original J1 orange ToH
                         QDBusVariant(m_sailfishThemeConfigGroup->value(QStringLiteral("highlight"),"#F76039", QMetaType::QString)) },
                  { CONFIG_SAILFISHOS_THEME_HIGHLIGHT_KEY,
                         QDBusVariant(m_sailfishThemeConfigGroup->value(QStringLiteral("secondaryHighlight"),"#943922", QMetaType::QString)) }
                }
          }
        };
        reply = message.createReply(QVariant::fromValue(result));
    } else {
        qCDebug(XdgDesktopPortalSailfishSettings) << "Unknown namespace:" << nss;
        reply = message.createErrorReply(QDBusError::UnknownProperty, QStringLiteral("Namespace is not supported"));
    }
    QDBusConnection::sessionBus().send(reply);
}

QDBusVariant SettingsPortal::Read(const QString &ns,
                              const QString &key)
{
    qCDebug(XdgDesktopPortalSailfishSettings) << "Settings called with parameters:";
    qCDebug(XdgDesktopPortalSailfishSettings) << "    namespace: " << ns;
    qCDebug(XdgDesktopPortalSailfishSettings) << "          key: " << key;

    // TODO
    Q_UNUSED(ns);

    if (key == CONFIG_FDO_SCHEME_KEY) {
        return QDBusVariant(getColorScheme());
    } else if (key == CONFIG_FDO_CONTRAST_KEY) {
        return QDBusVariant(getContrast());
    } else if (key == CONFIG_FDO_ACCENT_KEY) {
        return QDBusVariant(QVariant(getAccentColor()));
    }
    qCDebug(XdgDesktopPortalSailfishSettings) << "Unsupported key: " << key;
    return QDBusVariant(QVariant()); // QVariant() constructs an invalid variant
}

void SettingsPortal::valueChanged(const QString &what)
{
    if (what == CONFIG_FDO_SCHEME_KEY) {
        emit SettingChanged(NAMESPACE_FDO, what, QVariant(getColorScheme()));
    } else if (what == CONFIG_FDO_ACCENT_KEY) {
        emit SettingChanged(NAMESPACE_FDO, what, QVariant(getAccentColor()));
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
    return QList<QVariant>();
}

void SettingsPortal::ambienceChanged(const int &i)
{
    emit SettingChanged(NAMESPACE_SAILFISHOS, QStringLiteral("ambience"), QVariant(i));
}
} // namespace Sailfish
