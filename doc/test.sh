#!/bin/sh
function bing() {
#--hint="sound-name dialog-warning" \
notificationtool -o add -T 1500 --application=PortalTest --urgency=2 \
	--hint="transient true" \
	--hint="x-nemo-feedback general_warning" \
	"$1" #"$1" "$1" "$1"
}
case $1 in

0) # introspect impl
busctl --user introspect org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop
;;
1) # Screenshot
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Screenshot.Screenshot "0" '{}'
;;
2) # Set Wallpaper w/ preview
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Wallpaper.SetWallpaperURI "0" "file://${2}" '{"show-preview":<"true">}'
;;
3) # Set Wallpaper no preview
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Wallpaper.SetWallpaperURI "0" "file://${2}" '{"show-preview":<"false">}'
;;
4) # EMail
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Email.ComposeEmail "0" '{"address": <"test@example.org">,"subject":<"test email">}'
;;
5) # OpenFile
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.FileChooser.OpenFile "0" "" '{}'
;;
6) # OpenFile
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.FileChooser.OpenFile "0" "Choose a file (this is a test)" '{ "directory": <false>, "modal": <false>, "multiple": <false> }'
;;
7) # UI: just the file dialog
gdbus call --session --dest org.freedesktop.impl.portal.desktop.sailfish.ui --object-path /org/freedesktop/impl/portal/desktop/sailfish/ui --method org.freedesktop.impl.portal.desktop.sailfish.ui.openFilePicker
;;
8) # UI: confirmation dialog
busctl --user call org.freedesktop.impl.portal.desktop.sailfish.ui /org/freedesktop/impl/portal/desktop/sailfish/ui org.freedesktop.impl.portal.desktop.sailfish.ui confirmationDialog sssss "/org/freedesktop/impl/portal/handle/foo/1" "My Title" "My Subtitle" "My dialog body" '{ "deny_label": "Deny", "grant_label": "Allow" }'
;;
9) # Settings portal
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Settings.ReadAll "['org.freedesktop.appearance']"
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Settings.ReadAll "['org.sailfishos.desktop']"
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Settings.ReadAll "['org.kde.kdeglobals']"
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Settings.ReadAll "['org.kde.kdeglobals.*']"
;;
10) # Settings portal single
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Settings.Read "org.freedesktop.appearance" "accent-color"
;;
11) # Lockdown read
busctl --user  get-property org.freedesktop.portal.Desktop /org/freedesktop/portal/desktop org.freedesktop.portal.Lockdown disable-location
busctl --user  get-property org.freedesktop.portal.Desktop /org/freedesktop/portal/desktop org.freedesktop.portal.Lockdown disable_location
busctl --user  get-property org.freedesktop.portal.Desktop /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_location
busctl --user call org.freedesktop.portal.Desktop /org/freedesktop/portal/desktop org.freedesktop.DBus.Properties GetAll s "org.freedesktop.impl.portal.Lockdown"
;;
12) # read camera MDM policy, bing, mute, bing
echo camera
busctl --user get-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_camera
echo printing
busctl --user get-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_printing
busctl --user set-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_printing b true
busctl --user get-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_printing
echo location
busctl --user get-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_location
echo location on
busctl --user set-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_location b false
busctl --user get-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_location
sleep 1
echo location off
busctl --user set-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_location b true

busctl --user get-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_microphone
echo micmute on
busctl --user set-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_microphone b true
busctl --user get-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_microphone
echo micmute off
busctl --user set-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_microphone b true
busctl --user get-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_microphone

bing "test bing"
echo mute sound
busctl --user set-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_sound_output b true
busctl --user get-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_sound_output
bing "silent bing"
sleep 5
echo unmute sound
busctl --user set-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_sound_output b false
busctl --user get-property  org.freedesktop.impl.portal.desktop.sailfish /org/freedesktop/portal/desktop org.freedesktop.impl.portal.Lockdown disable_sound_output
sleep 3
bing "unmuted bing"
;;
13) # Account read
gdbus call --session  --dest org.freedesktop.portal.Desktop --object-path /org/freedesktop/portal/desktop --method org.freedesktop.portal.Account.GetUserInformation  "0" '{}'
;;
*) # just introspect Portals
busctl --user  introspect org.freedesktop.portal.Desktop /org/freedesktop/portal/desktop
echo give a number a d parameters to call.
less $0
;;

esac

