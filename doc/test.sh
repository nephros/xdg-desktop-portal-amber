#!/bin/sh
case $1 in

1)
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Screenshot.Screenshot "0" '{}'
;;
2)
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Wallpaper.SetWallpaperURI "0" "file://${2}" '{"show-preview":<"true">}'
;;
3)
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Wallpaper.SetWallpaperURI "0" "file://${2}" '{"show-preview":<"false">}'
;;
4)
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Email.ComposeEmail "0" '{"address": <"test@example.org">,"subject":<"test email">}'
;;
5)
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.FileChooser.OpenFile "0" "" '{}'
;;
6)
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.FileChooser.OpenFile "0" "Choose a file (this is a test)" '{ "directory": <false>, "modal": <false>, "multiple": <false> }'
;;
7)
gdbus call --session --dest org.freedesktop.impl.portal.desktop.amber.ui --object-path /org/freedesktop/impl/portal/desktop/amber/ui --method org.freedesktop.impl.portal.desktop.amber.ui.openFilePicker
;;
8)
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Settings.ReadAll "['org.freedesktop.appearance']"
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Settings.ReadAll "['org.sailfishos.desktop']"
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Settings.ReadAll "['org.kde.kdeglobals']"
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Settings.ReadAll "['org.kde.kdeglobals.*']"
;;
9)
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Settings.Read "org.freedesktop.appearance" "accent-color"
;;

*)
busctl --user  introspect org.freedesktop.portal.Desktop /org/freedesktop/portal/desktop
echo give a number a d parameters to call.
cat $0
;;

esac
