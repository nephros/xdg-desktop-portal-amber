/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt-project.org
 *
 * Copyright: 2016 Red Hat Inc
 * Copyright: 2016 Jan Grulich <jgrulich@redhat.com>
 * Copyright: 2021~ LXQt team
 * Authors:
 *   Palo Kisa <palo.kisa@gmail.com>
 * Copyright: 2024~ Sailfish OS Community
 * Authors:
 *   Peter G. <sailfish@nephros.org>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "desktopportal.h"
#include "access.h"
#include "email.h"
#include "filechooser.h"
#include "settings.h"
#include "screenshot.h"
#include "wallpaper.h"

namespace Amber
{
    DesktopPortal::DesktopPortal(QObject *parent)
        : QObject(parent)
        , m_access(new AccessPortal(this))
        , m_email(new EmailPortal(this))
        , m_filechooser(new FileChooserPortal(this))
        , m_screenshot(new ScreenshotPortal(this))
        , m_settings(new SettingsPortal(this))
        , m_wallpaper(new WallpaperPortal(this))
    {
        //m_access = new AccessPortal(this);
        //m_screenshot = new ScreenshotPortal(this);
        //m_wallpaper = new WallpaperPortal(this);

    }
    DesktopPortal::~DesktopPortal()
    {
    }
}

