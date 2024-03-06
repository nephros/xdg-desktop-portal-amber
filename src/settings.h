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

public:
    explicit SettingsPortal(QObject *parent);
    ~SettingsPortal() override;

    uint version() const { return 1; }

    enum ThemeColorScheme : uint {
      LightOnDark = 1,
      DarkOnLight = 2,
    };
    Q_ENUM(ThemeColorScheme)

    enum ColorScheme : uint {
      None = 0,
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
    QMap<QString, QMap<QString, QVariant>> ReadAll(const QStringList &nss);
    QVariant Read(const QString &ns, const QString &key);
/*
    void ReadAll(const QStringList &nss,
                      QVariantMap &value);

    void Read(const QString &ns,
                    const QString &key,
                    QVariant &value);
*/
signals:
    void SettingsChanged(const QString &ns,
                    const QString &key,
                    QVariant &value);
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
