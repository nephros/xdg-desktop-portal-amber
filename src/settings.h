/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
*/

#ifndef XDG_DESKTOP_PORTAL_AMBER_SETTINGS_H
#define XDG_DESKTOP_PORTAL_AMBER_SETTINGS_H

#include <QDBusAbstractAdaptor>
#include <QDBusVariant>

#include <mlite5/MGConfItem>

namespace Amber {
class SettingsPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Settings")
    Q_PROPERTY(uint version READ version CONSTANT)

    static const char* DCONF_SAILFISHOS_SCHEME_KEY;
    static const char* DCONF_SAILFISHOS_HIGHLIGHT_KEY;
    static const char* NAMESPACE_FDO;
    static const char* CONFIG_FDO_ACCENT_KEY;
    static const char* CONFIG_FDO_SCHEME_KEY;
    static const char* CONFIG_FDO_CONTRAST_KEY;
    // TODO: other namespaces, e.g.:
    static const char* NAMESPACE_SAILFISHOS;
    static const char* CONFIG_SAILFISHOS_THEME_SCHEME_KEY;
    static const char* CONFIG_SAILFISHOS_THEME_PRIMARY_KEY;
    static const char* CONFIG_SAILFISHOS_THEME_SECONDARY_KEY;
    static const char* CONFIG_SAILFISHOS_THEME_HIGHLIGHT_KEY;
    static const char* CONFIG_SAILFISHOS_THEME_SECONDARYHIGHLIGHT_KEY;
    static const char* NAMESPACE_KDE;
    static const char* NAMESPACE_KDE_GENERAL;
    static const char* CONFIG_KDE_SCHEME_KEY;

public:
    explicit SettingsPortal(QObject *parent);
    ~SettingsPortal() override;

    uint version() const { return 1; }

    enum ThemeColorScheme : int {
      LightOnDark = 0,
      DarkOnLight = 1,
    };
    Q_ENUM(ThemeColorScheme)

    enum ColorScheme : int {
      None = -1,
      Dark = ThemeColorScheme::LightOnDark,
      Light = ThemeColorScheme::DarkOnLight
    };
    Q_ENUM(ColorScheme)

public Q_SLOTS:
    void ReadAll(const QStringList &nss);
    QDBusVariant Read(const QString &ns, const QString &key);
    void valueChanged(const char* &what);

signals:
    void SettingsChanged(const QString &ns,
                         const QString &key,
                         const QVariant &value);
private:

    ColorScheme getColorScheme() const;
    QList<QVariant> getAccentColor() const;
    // TODO: We alsways return "normal/no preference" contrast.
    // FIXME: there s something about high contrast in lipstick somewhere...
    uint getContrast() const { return 0; };

    MGConfItem *m_schemeConfig;
    MGConfItem *m_accentColorConfig;

};
}
#endif // XDG_DESKTOP_PORTAL_AMBER_SETTINGS_H
