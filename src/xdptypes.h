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

using XDPResultPart = QMap<QString, QDBusVariant>;
// a{sa{sv}}
using XDPResultMap =  QMap<QString, XDPResultPart>;
// sa{sv}
// using Shortcut = QPair<QString, QVariantMap>;
// a(sa{sv})
//using Shortcuts = QList<Shortcut>;
}

Q_DECLARE_METATYPE(Amber::XDPResultPart);
Q_DECLARE_METATYPE(Amber::XDPResultMap);
