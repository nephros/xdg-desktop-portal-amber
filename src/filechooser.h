/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 */

#ifndef XDG_DESKTOP_PORTAL_AMBER_FILECHOOSER_H
#define XDG_DESKTOP_PORTAL_AMBER_FILECHOOSER_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

namespace Amber {
class FileChooserPortal : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.FileChooser")
    Q_PROPERTY(uint version READ version CONSTANT)

public:
    explicit FileChooserPortal(QObject *parent);
    ~FileChooserPortal() override;

    enum PickerResponse : uint
    {
        Accepted = 0,
        Cancelled = 1,
        Other = 2
    };
    Q_ENUM(PickerResponse)

public Q_SLOTS:
    // TODO:
    //version 3 must support opening directories instead of files through options(["directory=true"])
    uint version() const { return 2; }
    uint OpenFile(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QString &title,
                      const QVariantMap &options,
                      QVariantMap &results);
    uint SaveFile(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QString &title,
                      const QVariantMap &options,
                      QVariantMap &results);
    uint SaveFiles(const QDBusObjectPath &handle,
                      const QString &app_id,
                      const QString &parent_window,
                      const QString &title,
                      const QVariantMap &options,
                      QVariantMap &results);
    void handlePickerError();
    void handlePickerResponse(
         const uint &code,
         const QString &result
         //const QVariantMap &results
     );
    void waitForPickerResponse();
    void setupPickerResponse();

private:
    bool m_responseHandled = false;
    PickerResponse m_callResponseCode;
    QVariantMap m_callResult;

};
}
#endif // XDG_DESKTOP_PORTAL_AMBER_FILECHOOSER_H
