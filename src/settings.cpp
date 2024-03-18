/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
*/

#include "settings.h"
#include "xdptypes.h"

#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusContext>
#include <QDBusMessage>
#include <QDBusVariant>
#include <QColor>
#include <QLoggingCategory>

#include <mlite5/MGConfItem>
#include <mlite5/MDConfGroup>

Q_LOGGING_CATEGORY(XDPortalSailfishSettings, "xdp-sailfish-settings")

/*! \property Sailfish::XDP::SettingsPortal::version
    \brief Contains the backend implementation version
*/
/*! \enum Sailfish::XDP::SettingsPortal::FDOColorScheme

   Possible values for \c org.freedesktop.appearance \c color-scheme
    \value None
        No preference
    \value Dark
        Prefer dark appearance
    \value Light
        Prefer light appearance
*/

static const QMap<QString, QString> SailfishDconf = {
  { QStringLiteral("scheme_key"),    QStringLiteral("/desktop/jolla/theme/color_scheme") },
  { QStringLiteral("highlight_key"), QStringLiteral("/desktop/jolla/theme/color/highlight") },
  { QStringLiteral("theme_group"),   QStringLiteral("/desktop/jolla/theme/color") },
};

static const char* NAMESPACE_FDO     = "org.freedesktop.appearance";

// Custom namespace support.
//  -- naming things is hard.
//  -- org.sailfishos.desktop? com.jolla.lipstick? com.jolla.ambienced? "Mozilla/5.0 (org.sailfishos.desktop) com.jolla.lipsick/1.0 (like com.nokia.meego-harmattan)"?
static const char* NAMESPACE_SAILFISHOS = "org.sailfishos.desktop";

struct SailfishConfKeys {
    const char* scheme;
    const char* primary;
    const char* secondary;
    const char* highlight;
    const char* secondaryHighlight;
};
static const SailfishConfKeys SailfishConfKey  = {
    "color-scheme",
    "primary-color",
    "secondary-color",
    "highlight-color",
    "secondary-highlight-color",
};

// TODO: other namespaces, e.g.:
// prepare to fake KDE Settings
static const char* NAMESPACE_KDE             = "org.kde.kdeglobals";
static const char* NAMESPACE_KDE_GENERAL     = "org.kde.kdeglobals.General";
static const char* CONFIG_KDE_SCHEME_KEY     = "ColorScheme";


namespace Sailfish {
namespace XDP {
enum SailfishColorScheme : uint {
      LightOnDark = 0, // dark wallpaper, light fonts
      DarkOnLight = 1, // light wallpaper, dark fonts
};

static const QMap<SailfishColorScheme, SettingsPortal::FDOColorScheme> ColorSchemeMap = {
  { SailfishColorScheme::LightOnDark, SettingsPortal::FDOColorScheme::Light },
  { SailfishColorScheme::DarkOnLight, SettingsPortal::FDOColorScheme::Dark }
};

const QStringList SettingsPortal::SupportedNameSpaces = { NAMESPACE_FDO, NAMESPACE_SAILFISHOS, NAMESPACE_KDE_GENERAL };

SettingsPortal::SettingsPortal(QObject *parent)
    : QDBusAbstractAdaptor(parent)
    , m_schemeConfig(new MGConfItem(SailfishDconf.value("scheme_key"), this))
    , m_accentColorConfig(new MGConfItem(SailfishDconf.value("highlight_key"), this))
    , m_sailfishThemeConfigGroup(new MDConfGroup(SailfishDconf.value("theme_group"), this))
{
    qCDebug(XDPortalSailfishSettings) << "Desktop portal service: Settings";

    qDBusRegisterMetaType<XDPResultMap>();
    qDBusRegisterMetaType<XDPResultPart>();
    qDBusRegisterMetaType<QMap<QString, QDBusVariant>>();

    /* TODO
    QObject::connect(m_schemeConfig,      SIGNAL(valueChanged()), SLOT(valueChanged(const char* what=SettingsPortal::CONFIG_FDO_SCHEME_KEY)));
    QObject::connect(m_accentColorConfig, SIGNAL(valueChanged()), SLOT(valueChanged(const char* what=SettingsPortal::CONFIG_FDO_ACCENT_KEY)));
    */
    // listen for dconf changes:
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

    m_fdoValue = { -1, {0.0,0.0,0.0}, 0, };

}

SettingsPortal::~SettingsPortal()
{
}
/*! \fn void Sailfish::XDP::SettingsPortal::ReadAll(const QStringList &nss, const QDBusMessage &message)

    Implements \c org.freedesktop.impl.portal.Settings.ReadAll

    \a nss specifies the a list of namespaces. See the class description for supported namespaces.
*/
void SettingsPortal::ReadAll(const QStringList &nss, const QDBusMessage &message)
{
    qCDebug(XDPortalSailfishSettings) << "Settings called with parameters:";
    qCDebug(XDPortalSailfishSettings) << "    namespaces: " << nss;

    QDBusMessage reply;
    XDPResultMap result;

    for (auto i = nss.begin(), end = nss.end(); i != end; ++i) {
        if (!SupportedNameSpaces.contains(*i)) // skip unknown
            continue;

        if (*i == NAMESPACE_FDO) {
            // on-the-fly construction of "(a{sa{sv}})"
            result.insert(NAMESPACE_FDO, {
                {
                   { FDOSettingsKey.scheme,   QDBusVariant(getColorScheme()) },
                   { FDOSettingsKey.contrast, QDBusVariant(getContrast()) },
                   { FDOSettingsKey.accent,   QDBusVariant(getAccentColor()) }
                }
            }
            );
        } else if (*i == NAMESPACE_SAILFISHOS) {
            qCDebug(XDPortalSailfishSettings) << "Ahoy Sailor!";
            result.insert(NAMESPACE_SAILFISHOS, {
                {
                        { SailfishConfKey.scheme, QDBusVariant(getColorScheme()) },
                        { SailfishConfKey.primary,
                            QDBusVariant(m_sailfishThemeConfigGroup->value(QStringLiteral("primary"),"#ffffffff", QMetaType::QString)) },
                        { SailfishConfKey.secondary,
                            QDBusVariant(m_sailfishThemeConfigGroup->value(QStringLiteral("secondary"),"#b0ffffff", QMetaType::QString)) },
                        { SailfishConfKey.secondaryHighlight,
                            // F76039 ia approx the color of the original J1 orange ToH
                            QDBusVariant(m_sailfishThemeConfigGroup->value(QStringLiteral("highlight"),"#F76039", QMetaType::QString)) },
                        { SailfishConfKey.highlight,
                            QDBusVariant(m_sailfishThemeConfigGroup->value(QStringLiteral("secondaryHighlight"),"#943922", QMetaType::QString)) }
                }
            }
            );
            //reply = message.createReply(QVariant::fromValue(result));
        }
    }
    if (!result.isEmpty()) {
        reply = message.createReply(QVariant::fromValue(result));
    } else {
        reply = message.createErrorReply(QDBusError::InvalidArgs, QStringLiteral("Unknown Namespace."));
    }
//    qCDebug(XDPortalSailfishSettings) << "Sending:" << result;
    QDBusConnection::sessionBus().send(reply);
}

/*! \fn QDBusVariant Sailfish::XDP::SettingsPortal::Read(const QString &ns, const QString &key)
    Reads and returns a single value, specified by \a key, from namespace \a ns
*/
QDBusVariant SettingsPortal::Read(const QString &ns,
                              const QString &key)
{
    qCDebug(XDPortalSailfishSettings) << "Settings called with parameters:";
    qCDebug(XDPortalSailfishSettings) << "    namespace: " << ns;
    qCDebug(XDPortalSailfishSettings) << "          key: " << key;

    // TODO
    Q_UNUSED(ns);

    if (key == FDOSettingsKey.scheme) {
        return QDBusVariant(getColorScheme());
    } else if (key == FDOSettingsKey.contrast) {
        return QDBusVariant(getContrast());
    } else if (key == FDOSettingsKey.accent) {
        return QDBusVariant(getAccentColor());
    }
    qCDebug(XDPortalSailfishSettings) << "Unsupported key: " << key;
    return QDBusVariant(QVariant()); // QVariant() constructs an invalid variant
}

/*! \fn void Sailfish::XDP::SettingsPortal::SettingChanged(const QString &ns, const QString &key, const QVariant &value)

    Emitted when any of the supported values have changed.
    \a ns, \a key, \a value are namespace, key within that namespace, and value that has changed.

    \note emitting this signal is mandaroty for the XDP namespace. It may or
    may not be emitted in the same way for other namespaces.

    \sa SettingsPortalNamespaces
*/
/*! \fn void Sailfish::XDP::SettingsPortal::valueChanged(const QString &what)
    Slot to receive change signals, and do something about that.

    \sa SettingsPortal::SettingChanged
    \internal
*/
void SettingsPortal::valueChanged(const QString &key)
{
    if (key == FDOSettingsKey.scheme) {
        emit SettingChanged(NAMESPACE_FDO, key, QVariant(getColorScheme()));
    } else if (key == FDOSettingsKey.accent) {
        emit SettingChanged(NAMESPACE_FDO, key, getAccentColor());
    }
}

void SettingsPortal::readColorScheme()
{

  SailfishColorScheme scheme = static_cast<SailfishColorScheme>(m_schemeConfig->value().toInt());
  m_fdoValue.scheme = (int) ColorSchemeMap.value(
      scheme,
      FDOColorScheme::None
  );
}

void SettingsPortal::readAccentColor()
{
    QColor accent;
    accent.setNamedColor(m_accentColorConfig->value().toString());
    if (accent.isValid()) {
        m_fdoValue.accent[0] = accent.redF();
        m_fdoValue.accent[1] = accent.greenF();
        m_fdoValue.accent[2] = accent.blueF();
    }
}

/*! \fn void Sailfish::XDP::SettingsPortal::ambienceChanged(const int &i)
    Slot to receive change signals from ambienced, and do something about that.

    \sa com.jolla.ambienced.contentChanged
    \internal
*/
void SettingsPortal::ambienceChanged(const int &i)
{
    qCDebug(XDPortalSailfishSettings) << "Ambience change signalled, reloading";
    emit SettingChanged(NAMESPACE_SAILFISHOS, QStringLiteral("ambience"), QVariant(i));
    update();
    emit SettingChanged(NAMESPACE_FDO, FDOSettingsKey.accent,   getAccentColor());
    emit SettingChanged(NAMESPACE_FDO, FDOSettingsKey.scheme,   QVariant(getColorScheme()));
    emit SettingChanged(NAMESPACE_FDO, FDOSettingsKey.contrast, QVariant(getContrast()));
}
} // namespace XDP
} // namespace Sailfish
