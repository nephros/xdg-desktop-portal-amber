## XDG Desktop Portal interface lookup table

Legend:

 - SFOS capability: SFOS has a matching or similar function
 - SFOS interface:  SFOS has an existing interface or API for the capability
 - est. complexity (0-5): estimated complexity for implementation or adaptation
 - Usefulness (0-5): capability or function makes sense on a mobile OS


### Required or builtin

| Backend Name      | Description                     | SFOS capability | SFOS interface | est. complexity | Usefulness |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: |
|  Request          | shared, internal to portals     |      no         |     no         |     ++          | required   |
|  Session          | shared, internal to portals     |      no         |     no         |     ++          | required   |
|  Permission Store | store permissions of apps       |  somewhat       | DConf, SJail   |                 |            |

**Notes:**
Permission store is similar to SailfishOS MDM policy framework.

### Should be provided:
| Backend Name      | Description                     | SFOS capability | SFOS interface | est. complexity | Usefulness |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: |
|  Access           | presenting an access dialog     |  yes            | Lipstick, DBus |   ++            |            |
|  Notification     | send and withdraw notifications |  yes            | DBus, Qt/QML   |   +++           |  ++++      |
|  Wallpaper        | set desktop wallpaper           |  yes            | DBus, DConf    |   +             |  +++       |
|  Screenshot       |                                 |  yes            | DBus           |   +             |            |
|  Email            | sending an email                |  yes            | App, DBus      |                 |            |

**Notes:**
Email, Wallpaper and Screenshot will be implemented by Community effort as PoC for XDP.
Notification is probably a good candidate for PoC by SailfishOS upstream (Jolla).
Apparently Access is required for the Screenshot interface to be registered correctly?

#### Email:
Implemented, works.
Unfortunately, the Email app lacks an interface to specify *both* a list of
attachments *and* recipients, subject, body.
Attachments can be done via Share API, email via Email app dbus interface, but not both.

This was noted in another context here:
https://forum.sailfishos.org/t/13762

A patch by nephros exists imlementing the interface mentioned above.

Hackishly, one can construct a multipart MIME message and give it as the body, but this is not
possible to do fully compliant with the standard, and is unreliable.  
Also i needs a lot of email-specific processing in the sending application.
Recerse [Zawinskis Law](https://www.catb.org/jargon/html/Z/Zawinskis-Law.html).

#### Access:
Is required by Screenshot and uses/updates/reads PermissionStore.

Can be implemented using windowprompt.

This works as a PoC using the showInfoWindow method. However, that window does
not have yes/no choices, and no response. Access dialogs ask for permission,
and want to present choices.
newPermissionDialog could be used if adapted but is sailjail-specific at the moment.

Maybe security prompt would be better. Or polkit rules, which show confirmation prompts already.

Also, windowprompt wants the calling app to be privileged or it will not show dialogs.

#### Wallpaper
Setting a wallpaper image works in the PoC non-interativey (via ambienced), and
interactively (by opening in Gallery)

### May be provided/Undecided:

| Backend Name      | Description                     | SFOS capability | SFOS interface | est. complexity | Usefulness |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: |
|  Account          | obtaining user information      |  yes            | Settings, Secrets |              |            |
|  App Chooser      | choosing an application         |  yes            |                |                 |            |
|  Background       | apps running in the background  |  somewhat       |                |                 |            |
|  Dynamic Launcher | app installation                |  yes            | DBus           |                 |            |
|  File Chooser     | file chooser dialog             |  no             | (not standalone) |               |            |
|  Inhibit          | inhibit suspending, idling, ... |  yes            | wakelocks, Amber, MCE |          |            |
|  Input Capture    | Capture input                   |  ???            |                |                 |            |
|  Lockdown         | Disable Portals                 |  yes            | MDM, Settings, SJail | +++       |            |
|  Secret           | retrieve an app secret          |  yes            | Qt/QML         |                 |            |


### Out-of-Scope/Not Feasible/Not useful

| Backend Name      | Description                     | SFOS capability | SFOS interface | est. complexity | Usefulness |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: |
|  Settings         | read-only access to UI colors   |  yes            | DConf, Qt/QML  |   +             |  +         |
|  Print            |                                 |  no/3rd party   |                |                 |            |
|  Global Shortcuts | ??? hotkeys ???                 |  no             |                |                 |            |

**Notes:**
There is exactly one printing app in SFOS, if a printing interface is needed it may either implement XDP, or its own.

Access to interface colors is possible natively via toolkit. Although this might prove useful for e.g. Kirigami.

----

These depend on Pipewire to be functional:

| Backend Name      | Description                     | SFOS capability | SFOS interface | est. complexity | Usefulness |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: |
|  Clipboard        | clipboard access                |  yes            |                |                 |            |
|  Remote Desktop   | remote controlling desktop      |  no             |                |                 |  ++        |
|  ScreenCast       | stream desktop/app              |  partial        | Lipstick, Mozilla |  ++++        |  ++++      |
