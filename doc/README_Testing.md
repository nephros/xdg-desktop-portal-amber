### Installation

Curently, these steps are necessary for testing:

1. Installing `xdg-desktop-portals` version **1.16** [link](https://build.sailfishos.org/package/show/home:nephros:devel:portals/xdg-desktop-portal-116)
    - Note that there are subpackages of this called e.g. `permission-store`, or `rewrite-launchers`. You do NOT need to install those.
2. Install `xdg-desktop-portal-amber` [link](https://build.sailfishos.org/package/show/home:nephros:devel:portals/xdg-desktop-portal-amber), including the subpackages:
	- xdg-desktop-portal-amber
	- xdg-desktop-portal-amber-qml
	- xdg-desktop-portal-amber-sailfishos-config

These packages provide:

	- xdg-desktop-portal: the Portals frontend services by Flatpak
	- xdg-desktop-portal-amber: Portal backend implementation services
	- xdg-desktop-portal-amber-qml: UI component for e.g. the FileChooser dialogs
	- xdg-desktop-portal-amber-sailfishos-config: Any config files needed to glue everything together.

### Running

Note that currently, the systemd/dbus services and environment are not "polished" to start reliably and in the correct order.
For testing, it is recommended to stop the systemd services and run the binaries manually.

  - Service: `xdg-desktop-portal`: the Flatpak main Portals daemon
    - to run manually: `/usr/libexec/xdg-desktop-portal -rv`
  - Service: `xdg-desktop-portal-amber`: the SailfishOS implementation
    - to run manually: `/usr/libexec/xdg-desktop-portal-amber`
  - Helper UI application: `/usr/bin/xdg-desktop-portal-amber-ui`
    - do not run manually (this is launched vis DBus), but in case something does not work, kill a running process of this and try again.

There are several important environment variables in play. The most important
is `XDG_CURRENT_DESKTOP=Lipstick`. This must be set to "Lipstick" and must be
set in the env of `xdg-desktop-portal` (so the main service).
The package `xdg-desktop-portal-amber-sailfishos-config` does ship a config
file adding this variable, but this does not yet function reliably.

To test interaction of applications coming from KDE/Plasma, like Kirigami apps
available from Chum, such applications must have the variable
`PLASMA_INTEGRATION_USE_PORTAL=1` set, which is usually done in qt-runner.

### Tests

**NOTE**: Everything runs in *user/session* bus. No system bus things are involved.

1. run `busctl --user  introspect org.freedesktop.portal.Desktop /org/freedesktop/portal/desktop` (or equivalent).
You should see several services available on the bus: 
These come from the Flatpak service and not from Amber, and seeing them means the main portal service is running correctly.

Example output snippet:

```
org.freedesktop.portal.GameMode            interface -         -            -
.QueryStatus                               method    i         i            -
.QueryStatusByPIDFd                        method    hh        i            -
.QueryStatusByPid                          method    ii        i            -
.RegisterGame                              method    i         i            -
.RegisterGameByPIDFd                       method    hh        i            -
.RegisterGameByPid                         method    ii        i            -
.UnregisterGame                            method    i         i            -
.UnregisterGameByPIDFd                     method    hh        i            -
.UnregisterGameByPid                       method    ii        i            -
.Active                                    property  b         false        emits-change
.version                                   property  u         4            emits-change
org.freedesktop.portal.MemoryMonitor       interface -         -            -
.version                                   property  u         1            emits-change
.LowMemoryWarning                          signal    y         -            -
org.freedesktop.portal.NetworkMonitor      interface -         -            -
.CanReach                                  method    su        b            -
.GetAvailable                              method    -         b            -
.GetConnectivity                           method    -         u            -
.GetMetered                                method    -         b            -
.GetStatus                                 method    -         a{sv}        -
.version                                   property  u         3            emits-change
.changed                                   signal    -         -            -
org.freedesktop.portal.PowerProfileMonitor interface -         -            -
.power-saver-enabled                       property  b         false        emits-change
.version                                   property  u         1            emits-change
org.freedesktop.portal.ProxyResolver       interface -         -            -
```

1. If amber is configured, running and recognized correctly, you should see at least the following services in addition:
   - `org.freedesktop.portal.FileChooser`
   - `org.freedesktop.portal.Email`
   - `org.freedesktop.portal.Wallpaper`

If not, check that: `XDG_CURRENT_DESKTOP` variable is set and available to the
xdg-desktop-portal service. Also, that xdg-desktop-portal-amber service/binary
is running.


#### Simple test: Email portal
Run:
```
gdbus call --session  --dest org.freedesktop.portal.Desktop \
    --object-path /org/freedesktop/portal/desktop \
    --method org.freedesktop.portal.Email.ComposeEmail \
    "0" '{"    address": <"test@example.org">,"subject":<"test email">}'
```

The Sailfish OS EMail client should come up in compose mode.

#### Intermediate Tests: Wallpaper and Screenshot

Place a Wallpaper-suitable test image at e.g. `$HOME/Pictures/TestWallpaper.jpg"
```
gdbus call --session  --dest org.freedesktop.portal.Desktop \
     --object-path /org/freedesktop/portal/desktop \
     --method org.freedesktop.portal.Wallpaper.SetWallpaperURI \
     "0" "file://$HOME/Pictures/TestWallpaper.jpg" '{"show-preview":<"true">}'
```

The Sailfish OS Gallery app should come up in edit mode (because of `show-preview = true`

```
gdbus call --session  --dest org.freedesktop.portal.Desktop \
     --object-path /org/freedesktop/portal/desktop \
     --method org.freedesktop.portal.Wallpaper.SetWallpaperURI \
     "0" "file://$HOME/Pictures/TestWallpaper.jpg" '{"show-preview":<"false">}'
```

Nothing apparent happens, but there should now be a new Ambience made from
TestWallpaper.jpg available in the Ambience Settings.

```
gdbus call --session  --dest org.freedesktop.portal.Desktop \
    --object-path /org/freedesktop/portal/desktop \
    --method org.freedesktop.portal.Screenshot.Screenshot "0" '{}'
```

This should ask for permission to take a screenshot. Possibly even do that and place it in `~/Pictures/Screenshots`
However, sometimes this fails with a message that screenshots are not allowed by Device Policy. This is a known, yet unsolved, issue.

#### Complex test: FileChooser

The canonical test for this is to try to post an image using the Kirigami Mastodon client Tokodon.
Make sure the `PLASMA_INTEGRATION_USE_PORTAL=1` is set in the qt-runner environment, and launch Tokodon.

Create a new toot, and select to add a new image.

Instead of the usual Kirigami/KDE dialog, Silica/Sailfish-styled picker dialog
should come up, allow to select an image, and have that image appear in the new
toot.


For command-line debugging, you can to something like these:

Launch the picker UI only:
 ```
 gdbus call --session --dest org.freedesktop.impl.portal.desktop.amber.ui \
     --object-path /org/freedesktop/impl/portal/desktop/amber/ui \
	 --method org.freedesktop.impl.portal.desktop.amber.ui.openFilePicker
``` 


To simulate an app requesting a file open dialog:
```
 
gdbus call --session  --dest org.freedesktop.portal.Desktop \
    --object-path /org/freedesktop/portal/desktop \
	--method org.freedesktop.portal.FileChooser.OpenFile \
	"0" "Choose a file (this is a test)" '{ "directory": <false>, "modal": <false>, "multiple": <false> }'
```

