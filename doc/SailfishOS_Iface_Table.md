## XDG Desktop Portal interface lookup table

Legend:

 - SFOS capability: SFOS has a matching or similar function
 - SFOS interface:  SFOS has an existing interface or API for the capability
 - Impl complexity (0-5): estimated complexity for implementation or adaptation
 - Usefulness (0-5): capability or function makes sense on a mobile OS


### Required

| Backend Name      | Description                     | SFOS capability | SFOS interface | Impl complexity | Usefulness |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: |
|  Request          | shared, internal to portals     |      no         |     no         |     ++          | required   |
|  Session          | shared, internal to portals     |      no         |     no         |     ++          | required   |


### Should be provided:
| Backend Name      | Description                     | SFOS capability | SFOS interface | Impl complexity | Usefulness |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: |
|  Notification     | send and withdraw notifications |  yes            | DBus, Qt/QML   |     +++         |  ++++      |
|  Wallpaper        | set desktop wallpaper           |  yes            | DBus, DConf    |     +           |  +++       |
|  Screenshot       |                                 |  yes            | DBus           |     +           |            |

**Notes:**
Wallpaper and Screenshot will be implemented by Community effort as PoC for XDP.
Notification is probably a good candidate for PoC by SailfishOS upstream.


### May be provided/Undecided:

| Backend Name      | Description                     | SFOS capability | SFOS interface | Impl complexity | Usefulness |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: |
|  Access           | presenting an access dialog     |  yes            | Lipstick, DBus |   ++            |            |
|  Account          | obtaining user information      |  yes            | Settings, Secrets |              |            |
|  App Chooser      | choosing an application         |  yes            |                |                 |            |
|  Background       | apps running in the background  |  somewhat       |                |                 |            |
|  Clipboard        | clipboard access                |  yes            |                |                 |            |
|  Dynamic Launcher | app installation                |  yes            | DBus           |                 |            |
|  Email            | sending an email                |  yes            | App, DBus      |                 |            |
|  File Chooser     | file chooser dialog             |  no             |                |                 |            |
|  Global Shortcuts | ???                             |  no             |                |                 |            |
|  Inhibit          | inhibit suspending, idling      |  yes            | wakelocks, Amber, MCE |          |            |
|  Input Capture    | Capture input                   |  yes            |                |                 |            |
|  Lockdown         | Disable Portals                 |  yes            | MDM, Settings, SJail |           |            |
|  Permission Store | store permissions of apps       |  somewhat       | DConf, SJail   |                 |            |
|  Remote Desktop   | remote controlling desktop      |  no             |                |                 |  ++        |
|  ScreenCast       | stream desktop/app              |  partial        | Lipstick, Mozilla |  ++++        |  ++++      |
|  Secret           | retrieve an app secret          |  yes            | Qt/QML         |                 |            |


### Out-of-Scope/Not Feasible/Not useful
| Backend Name      | Description                     | SFOS capability | SFOS interface | Impl complexity | Usefulness |
| ----------------- | ---------------------------     | :-------------: | -------------- | :-------------: | :--------: |
|  Settings         | read-only access to UI colors   |  yes            | DConf, Qt/QML  |   +             |  +         |
|  Print            |                                 |  no/3rd party   |                |                 |            |

**Notes:**
There is exactly one printing app in SFOS, if a printing interface is needed it may either implement XDP, or its own.

Access to interface colors is possible natively via toolkit. Although this might prove useful for e.g. Kirigami.
