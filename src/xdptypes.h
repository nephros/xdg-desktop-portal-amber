/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 */


#pragma once

#include <QDBusArgument>
#include <QMap>
#include <QString>

namespace Amber {

// a{sa{sv}}
using XDPResultMap =  QMap<QString, QMap<QString, QDBusVariant>>;
// sa{sv}
// using Shortcut = QPair<QString, QVariantMap>;
// a(sa{sv})
//using Shortcuts = QList<Shortcut>;
}

Q_DECLARE_METATYPE(Amber::XDPResultMap);
