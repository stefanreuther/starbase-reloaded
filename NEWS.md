v0.44 (30/Jan/2021)
-------------------

Messages now appear in the correct order with respect to
host-generated messages. In particular, mine laying from starbases
appears before mine sweeping from ships that happened later, so
programs that parse messages see the correct final size. In addition,
fix a wrong message template.


v0.43 (12/Sep/2020)
-------------------

Handle UnitsPerTorpRate (robot 4X bonus) and UnitsPerWebRate for
minefields. This option needs to be handled in the same way as for
ships. Otherwise, Robots could produce unlimited torpedoes by laying
mines with ships (using 4X bonus) and scooping them with bases (not
honoring the 4X bonus, therefore producing 4X the torpedoes used for
laying the field).


v0.42 (02/Sep/2020)
-------------------

Initial public release.
