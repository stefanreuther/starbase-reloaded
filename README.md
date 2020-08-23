Starbase Reloaded
=================

Starbase Reloaded is an add-on for VGA Planets, modeled after the
StarbasePlus add-on. See PLAYER.md for player documentation.


Compiling
---------

You need the PHost development kit (PDK) that is available here:
<http://phost-contrib.sourceforge.net/>

**If you just want it compiled,** and that's it, use the enclosed
Makefile. Adjust the variables at the top and type `make`.

**If you want to seriously modify it,** use the Makefile generator
from <https://github.com/stefanreuther/accidental-build>. Create a
build directory, and type

     /path/to/Make.pl IN=/path/to/sbreload PDK_DIR=/path/to/pdk
     make

This will create a Makefile suited for development, which does
automatic dependencies, automatic rebuild on rule change, and
out-of-tree build.

In any case, the build result will be a binary `sbreload` that is the
entire add-on.

So far, Starbase Reloaded has been tested only on Linux with PHost.


Installing and Configuring
--------------------------

Starbase Reloaded needs to be invoked from AUXHOST1.INI and
AUXHOST2.INI. Invoke as

    sbreload 1 path/to/game
    sbreload 2 path/to/game

from the respective location.

Starbase Reloaded will take a configuration file `psbplus.src` from
the game directory. You can use `sbreload -dc` on an empty directory
to print a list of configuration options with defaults. See PLAYER.md
for descriptions of the options.

Starbase Reloaded will store state in a file `psbplus.hst` in the game
directory.


Colophon
--------

This add-on re-implements ideas from the classic StarbasePlus add-on,
and took inspiration from the earlier (but incomplete) implementation
`pstarbase`. It uses the same file formats as the latter.

Written in 2020 by Stefan Reuther <streu@gmx.de> for PlanetsCentral
<https://planetscentral.com/>.

This source code is licensed under a permissive BSD license. Note that
linking against the PDK will produce a binary that is covered by the
GNU GPL, as per the PDK's license.
