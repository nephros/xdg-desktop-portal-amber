## Future Challenges

### Closed-source/proprietary SFOS components


In theory the majority of work for this could be done as a community effort.
However, in areas where proprietary SFOS components need changes, a
community-only implementation would have to resort to hacks and maybe
(patchmanager-)patches in order to provide needed functionality.  

### Upstream Pipewire dependency

Versions newer than 1.16 (which is from 2022) of the Flatpak portals daemon
have a hard dependency on Pipewire. This can be patched-out in e.g. the latest
1.18 version, but that of course means both a maintenance burden, and losing
capabilities depending on PW (which is the cool stuff like screen sharing and
video conferencing).  While I do not want to instigate a discussion about
moving from PA to PW, it should be noted that while PW touts "drop-in
replacement for PA", [this does not apply to PA
plugins](https://docs.pipewire.org/page_pulse_modules.html). Therefore, PW
migration would at the least require a port/rewrite of these plugins.

### Security:

While Flatpak talks about this whole things being "secure", it is unclear (to
me) where that security is controlled, and whether and how that aligns or
contradicts things present in SFOS, like SailJail, polkit, and Policy
Framework.  Investigation needed by experts in these areas.

### Longterm benefits for the SFOS platform

Mozilla > 116 has support for Portals for e.g. camera access. A proper
SFOS-specific implementation of the required portals *could* reduce
browser-specific adaptations, and at the same time provide such capabilities to
all applications.

