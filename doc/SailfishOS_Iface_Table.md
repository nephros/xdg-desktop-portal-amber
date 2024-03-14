## XDG Desktop Portal interface lookup table

### XDP frontend services

This tble lists the interfaces provided by xdg-desktop-portal without any backend implementations

Not strictly relevant to the Amber backend implementation, but given here for completeness.

| Frontend interface                         | Description                   | tested | working | SFOS Capability |
| ------------------------------------------ | ----------------------------  | :----: | :-----: | :-------------: |
| org.freedesktop.portal.Device              | Asks for access to cam/mic    | no     |         | yes             |
| org.freedesktop.portal.GameMode            | com.feralinteractive.GameMode | no     |         | no              |
| org.freedesktop.portal.MemoryMonitor       | low system memory             | no     |         | yes             |
| org.freedesktop.portal.NetworkMonitor      | network status information    | no     |         | yes             |
| org.freedesktop.portal.PowerProfileMonitor | lookup power saving mode      | yes    |   no    | yes             |
| org.freedesktop.portal.ProxyResolver       | Looks up http proxy           | no     |         | yes             |
| org.freedesktop.portal.Realtime            | org.freedesktop.RealtimeKit1  | yes    |   no    | no              |
| org.freedesktop.portal.Trash               | send files to the trashcan    | no     |         | no              |

**Notes:**
The spec sais something like this for most of these:
> It is not a portal in the strict sense, since it does not involve user
> interaction. Applications are expected to use this interface indirectly, via
> a library API such as the GLib XXXMonitor interface.


Legend:

 - SFOS capability: SFOS has a matching or similar function
 - SFOS interface:  SFOS has an existing interface or API for the capability
 - est. complexity (0-5): estimated complexity for implementation or adaptation
 - Usefulness (0-5): capability or function makes sense on a mobile OS


### Required or builtin

| Backend Name      | Description                     | SFOS capability | SFOS interface | est. complexity | Usefulness | Amber implementation |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: | :------------------: |
|  Request          | shared, internal to portals     |      no         |     no         |     ++          | required   |                      |
|  Session          | shared, internal to portals     |      no         |     no         |     ++          | required   |                      |
|  Permission Store | store permissions of apps       |  somewhat       | DConf, SJail   |                 |            |                      |

**Notes:**
Permission store is similar to SailfishOS MDM policy framework.
There is a GVDB based implementation available from the Flatpak portal.
Eventually a native variant should be designed.

### Should be provided:
| Backend Name      | Description                     | SFOS capability | SFOS interface | est. complexity | Usefulness | Amber implementation status    |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: | :----------------------------: |
|  Access           | presenting an access dialog     |  yes            | Lipstick, DBus |   ++            |  required  |  partial, Jolla changes needed |
|  Notification     | send and withdraw notifications |  yes            | DBus, Qt/QML   |   +++           |  ++++      |  not started                   |
|  Wallpaper        | set desktop wallpaper           |  yes            | DBus, DConf    |   +             |  +++       |  experimental                  |
|  Screenshot       |                                 |  yes            | DBus           |   +             |            |  experimental                  |
|  Email            | sending an email                |  yes            | App, DBus      |                 |            |  partial                       |
|  File Chooser     | file chooser dialog             |  no             | (not standalone) | ++            |  +++       |  experimental                  |

**Notes:**
*Email*, *Wallpaper* and *Screenshot* will be implemented by Community effort as PoC for XDP.
*Notification* is probably a good candidate for PoC by SailfishOS upstream (Jolla).
Apparently *Access* is required for the *Screenshot* interface to be registered correctly?

#### Email:
Implemented, works.
Unfortunately, the Email app lacks an interface to specify *both* a list of
attachments *and* recipients, subject, body.
Attachments can be done via Share API, email via Email app dbus interface, but not both.

This was noted in another context here:
https://forum.sailfishos.org/t/13762

A patch by nephros exists implementing the interface mentioned above.

Hackishly, one can construct a multipart MIME message and give it as the body, but this is not
possible to do fully compliant with the standard, and is unreliable.  
Also i needs a lot of email-specific processing in the sending application.
Reverse [Zawinskis Law](https://www.catb.org/jargon/html/Z/Zawinskis-Law.html).

#### Access:
Is required by *Screenshot* and uses/updates/reads *PermissionStore*.

Can be implemented using `windowprompt`.

This works as a PoC using the `showInfoWindow` method. However, that window does
not have yes/no choices, and no response. Access dialogs ask for permission,
and want to present choices.
`newPermissionDialog` could be used if adapted but is sailjail-specific at the moment.

Maybe security prompt would be better. Or polkit rules, which show confirmation prompts already.

Also, `windowprompt` wants the calling app to be privileged or it will not show dialogs.

#### Wallpaper
Setting a wallpaper image works in the PoC non-interativey (via ambienced), and
interactively (by opening in Gallery)

#### File Chooser
While it may not immediately seem obvious, providing this Portal is useful for
"ported" apps like those using Kirigami or KDE/Plasma in general.

These apps can be configured to use a Portal for file choosers instead of their
native picker dialogs, which often are not usable under SFOS.

The problem is that there is no functionality to present the existing
`Sailfish.Pickers` components as standalone dialogs.  
Therefore, a helper app was implemented to do that. It is based on
`SystemDialog` and existing dialogs like Network or Bluetooth device selection.

Side note: Mozilla browsers can also use Portals for file pickers. Currently,
the Sailfish Browser app uses its own purpose-built UI Dialog for Save actions.  
Maybe the Save action dialog can be split out into a Portal application,
letting both the Browser, and other users of *FileChooser* use it.

### May be provided/Undecided:

| Backend Name      | Description                          | SFOS capability | SFOS interface        | est. complexity | Usefulness |
| ----------------- | ------------------------------------ | :-------------: | --------------------- | :-------------: | :--------: |
|  Account          | basic user information               |  yes            | SailfishUserManager   | +               | +          |
|  Dynamic Launcher | .desktop file installation           |  yes            |                       |                 | +          |
|  Inhibit          | inhibit suspending, idling, ...      |  yes            | wakelocks, Amber, MCE |                 | +++        |
|  Input Capture    | Capture input                        |  ???            |                       |                 |            |
|  Lockdown         | Disable Portals, mic, gps, sound...  |  yes            | MDM, Settings, SJail  | +++             | +++        |
|  Secret           | retrieve an app secret               |  yes            | Qt/QML                |                 | +          |
|  Settings         | read-only access to UI colors        |  yes            | DConf, Qt/QML         | ++              | +          |

**Notes:**
Settings: While this is not very useful through the default
"org.freedesktop.appearance" namespace, it supports requesting custom
namespaces.  
This means we could implement e.g. the namespace from KDE/Plasma/Kirigami, allowing such apps to access SFOS Theme colors etc.
Doing that could avoid or assist having to implement a custom theme for Kirigami or QQuickControls.

DynamicLauncher: This describes "installing" (i.e. creating) local .desktop
files, e.g. to launch a web address. Implemented in Sailfish Browser. May be
useful.

### Out-of-Scope/Not Feasible/Not useful

| Backend Name      | Description                     | SFOS capability | SFOS interface | est. complexity | Usefulness |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: |
|  App Chooser      | choosing an application         |  yes            |                |                 |            |
|  Background       | apps running in the background  |  somewhat       |                |                 |            |
|  Print            |                                 |  no/3rd party   |                |                 |            |
|  Global Shortcuts | ??? hotkeys ???                 |  no             |                |                 |            |

**Notes:**
The only two printing apps in SFOS are SeaPrint, and Sailfish Browser.


----

These depend on Pipewire to be functional:

| Backend Name      | Description                     | SFOS capability | SFOS interface | est. complexity | Usefulness |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: |
|  Clipboard        | clipboard access                |  yes            |                |                 |            |
|  Remote Desktop   | remote controlling desktop      |  no             |                |                 |  ++        |
|  ScreenCast       | stream desktop/app              |  partial        | Lipstick, Mozilla |  ++++        |  ++++      |
