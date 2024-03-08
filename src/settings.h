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
#include <QStringList>

#include <mlite5/MGConfItem>
#include <mlite5/MDConfGroup>

namespace Amber {
class SettingsPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Settings")
    Q_PROPERTY(uint version READ version CONSTANT)

public:
    explicit SettingsPortal(QObject *parent);
    ~SettingsPortal() override;

    uint version() const { return 1; }

    enum FDOColorScheme : int {
      None = -1,
      Dark = 0,
      Light = 1,
    };
    Q_ENUM(FDOColorScheme)

    struct FDOConfKeys {
        const char* accent;
        const char* scheme;
        const char* contrast;
    };
    const FDOConfKeys FDOSettingsKey = {"accent-color", "color-scheme", "contrast"};

public Q_SLOTS:
    void ReadAll(const QStringList &nss);
    QDBusVariant Read(const QString &ns, const QString &key);
    void valueChanged(const QString &key);
    void ambienceChanged(const int &i);

signals:
    void SettingChanged(const QString &ns,
                         const QString &key,
                         const QVariant &value);
protected:
    static const QStringList SupportedNameSpaces;

private:
    struct FDOValues {
        int scheme;
        double accent[3];
        uint contrast;
    };
    FDOValues m_fdoValue;

    int getColorScheme() const      { return m_fdoValue.scheme; };
    QVariant getAccentColor() const { return QVariant( m_fdoValue.accent ); };
    uint getContrast() const        { return m_fdoValue.contrast; };

    void readColorScheme();
    void readAccentColor();
    void readContrast() { m_fdoValue.contrast = 0; };
    void update() { readColorScheme(); readAccentColor(); readContrast(); }

    MGConfItem  *m_schemeConfig;
    MGConfItem  *m_accentColorConfig;
    MDConfGroup *m_sailfishThemeConfigGroup;

};
}
#endif // XDG_DESKTOP_PORTAL_AMBER_SETTINGS_H
