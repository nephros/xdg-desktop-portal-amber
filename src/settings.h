/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
*/

#ifndef XDG_DESKTOP_PORTAL_AMBER_SETTINGS_H
#define XDG_DESKTOP_PORTAL_AMBER_SETTINGS_H

#include <QDBusAbstractAdaptor>

#include <mlite5/MGConfItem>

namespace Amber {
class SettingsPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Settings")
    Q_PROPERTY(uint version READ version CONSTANT)

    static const char* THEME_DCONF_SCHEME_KEY;
    static const char* THEME_DCONF_HIGHLIGHT_KEY;
    static const char* NAMESPACE_OFDA_KEY;
    static const char* CONFIG_ACCENT_KEY;
    static const char* CONFIG_SCHEME_KEY;
    static const char* CONFIG_CONTRAST_KEY;

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
    struct ColorRGB {
            double red;
            double green;
            double blue;
    };

public Q_SLOTS:
    void ReadAll(const QStringList &nss,
                       QVariantMap &value);

    void Read(const QString &ns,
              const QString &key,
                    QVariant &value);
    void valueChanged(const QString &what);

signals:
    void SettingsChanged(const QString &ns,
                         const QString &key,
                         const QVariant &value);
private:

    ColorScheme getColorScheme() const;
    ColorRGB getAccentColor() const;
    // TODO: We alsways return "normal/no preference" contrast.
    uint getContrast() const { return 0; };

    MGConfItem *m_schemeConfig;
    MGConfItem *m_accentColorConfig;

};
}
#endif // XDG_DESKTOP_PORTAL_AMBER_SETTINGS_H
