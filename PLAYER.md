Starbase Reloaded - Player Documentation
========================================

Starbase Reloaded is an add-on for VGA Planets, modeled after the
StarbasePlus add-on. It provides new functionalities for starbases,
controlled by friendly codes:

- WIRE TRANSFER of money between bases

- MINEFIELDS can be laid, swept and scooped by bases

- STARSHIP PARTS can be transported between bases using ships

This file describes each feature along with the friendly codes and
configuration options.

Note that all friendly codes are case-sensitive; `RMT` will work,
`rmt` will not.



Wire Transfer
-------------

Many starbases can send cash, one starbase per player can receive.
Transfers happen only between your own bases; you cannot transfer
money to other players this way.

Starbases must have at least 20 tech levels in total (sum of all tech
levels, e.g. four times tech 5, two times tech 10, etc.) to be able to
send or receive money. Friendly codes on bases that do not qualify are
ignored.


### Friendly Codes

* `RMT` (receive money transfer)

  The starbase with this friendly code will receive money from all
  sending bases. Only one starbase per player can receive each turn;
  if multiple bases use `RMT`, only the base with the lowest Id will
  actually receive.


* `TMn` (transfer money)

  `n` is a digit between 1 and 5. The starbase will send up to
  `n`*1000 mc to the receiving starbase. The upper limit can be
  configured. If the planet doesn't have enough cash, fewer will be
  sent. If nobody is receiving, nothing will happen.



### Configuration

The following `psbplus.src` options are relevant for Wire Transfer:

* `StarbaseMCTransfer` (Yes/No, default: Yes)

  Enable this feature.

* `MaxMCTransfer` (number, default: 3000)

  Maxiumm amount of cash a single `TMn` can send.



Minefields
----------

Starbases can lay, sweep and scoop minefields.

To lay, they use torpedoes they have in storage; likewise, scooping
will produce torpedoes for storage. Sweeping uses beams derived from
the base's defense power.

There is no difference between minefields laid by a starbase and
minefields laid by a ship.


### Friendly Codes

* `LMF` (lay minefield; planet friendly code)

  The starbase will lay all its torpedoes as mines. It is not possible
  to lay only a fraction of the stored torpedoes, or lay a minefield
  in another player's identity. Otherwise, rules are similar to
  regular mine laying with starships:

  If the starbase is inside one of your minefields, it will enlarge
  that field; otherwise, it will make a new one.

  Torpedoes are converted into mine units using the formula

       Mine_Units = Num_Torps * (Torp_slot ^ 2)

  The Robotic 4x advantage is not active for starbases!

  If the maximum minefield radius has been reached, no more torpedoes
  will be laid.


+ `LWF` (lay web field; planet friendly code)

  This friendly code can only be used by Crystals (as determined by
  PHost's `PlayerSpecialMission` configuration). It works like `LMF`
  but will lay a web minefield.


+ `SMF` (sweep minefield; planet friendly code)

  This friendly code causes the starbase to use its beams and fighters
  to sweep mines. Beams use the following rules:

      Num_Beams = Trunc(Base_Defense / 20)

      Mine_Units = (Beam_Tech ^ 2) * Num_Beams * Rate

  The `Rate` is configured as `BeamSweepRate`, `BeamWebSweepRate`
  depending on the mine field type. The minefield must be within a
  range of `BeamSweepRange`.

  Fighters differ between Colonies (as determined by PHost's
  `PlayerRace` option) and others:

  * Colonial fighters will sweep 25 mine units each, within a range of
    100 ly. If PHost's `AllowColoniesSweepWebs` is enabled, they will
    also sweep `FtrSweepRate` webs.

  * others' fighters will sweep `FtrSweepRate` each. The range is
    determined by the base's tech levels

        Trunc((Hull_Tech + Engine_Tech + Beam_Tech) / 3) * 10

    Non-Colonial fighters cannot sweep webs.

  You will sweep all enemy minefields (that is, not yours, and not
  owned by someone you have a bidirectional mine-level alliance with).


+ `MSC` (mine scoop; planet friendly code)

  This friendly code causes the starbase to gather up the mines from
  an own minefield and convert it into torpedoes. The starbase will
  gather up all applicable minefields in `BeamSweepRange` and produce
  torpedoes of the best-possible type the starbase can currently build
  (that is, if you have torpedo tech 6, it will produce the best
  torpedo type that can be built with tech 6).

  Mines are converted to torpedoes at the same rate as for laying.


### Configuration

The following `psbplus.src` options are relevant for Minefields:

+ `LayMinefields`, `LayWebMinefields` (Yes/No; default: Yes)

  Enable laying of minefields using `LMF`, `LWF`.


+ `BeamSweepMines` (Yes/No; default: Yes)

  Enable mine sweeping using beams with `SMF`.


+ `BeamSweepRange` (number; default: 5)

  Defines the maximum distance for mine sweeping.


+ `BeamSweepRate`, `BeamWebSweepRate` (number; default: 4, 3)

  Mine sweeping rate for mines and web mines.


+ `FighterSweepMines` (Yes/No; default: Yes)

  Enable mine sweeping using fighters with `SMF`.


+ `ColonialFighterOnlySweepMines` (Yes/No; default: No)

  If set, only Colonies can use their fighters to sweep mines. By
  default, all players can use fighters. Note that Colonies can only
  sweep web mines with fighters if the PCONFIG setting
  `AllowColoniesSweepWebs` is enabled; other players can never sweep
  webs with fighters.


+ `FtrSweepRate`, `FtrWebSweepRate` (number; default: 20, 0)

  Mine sweeping rate for fighters.


+ `ScoopMinefields` (Yes/No; default: Yes)

  Enable mine scooping using `MSC`.


The following `pconfig.src` options are relevant for Minefields:

+ `MaximumMinefieldRadius`, `MaximumWebMinefieldRadius`

  These options have the same meaning as for ships laying minefields.
  A minefield reaching that radius can no longer grow.


+ `PlayerRace`, `AllowColoniesSweepWebs`

  This option is used to determine Colony players for fighter
  sweeping, and whether they can sweep webs.


+ `PlayerSpecialMission`

  This option is used to determine Crystal players for laying web
  mines.



Starship Parts
--------------

You can transport starship parts (engines, beams, torpedo launchers)
by loading them onto ships. By default, all freighters can load parts.

Each part/component takes 40 kt cargo space (`CargoSpacePerComp`), or
its regular weight if that is higher. Ships must have sufficient free
cargo space to load parts. That cargo space needs to remain "free" as
long as the components are aboard the ship. Beware, standard clients
do not show that you have starship parts loaded.

To notify you of loaded parts, the ship's name is changed to have the
prefix "ST:" (for "special transport") when you load parts.

Note that all load/unload actions happen **after movement** and
therefore **after ship building**.

If a ship happens to become overloaded (for example, by you
accidentally loading more cargo), excess cargo will be destroyed;
loaded starship parts will have priority.


### Friendly Codes

+ `GEn` (get engine)

  `n` is a digit between 1 and 9. This will cause the ship to load up
  as many engines of the specified type as possible (as many as the
  starbase has, as many as will fit into the ship).

  There is no way to load only a part of the starbase's stock.

  The ship needs to end its turn at a starbase owned by the same
  player to load parts from it; it is not possible to load from allied
  or even enemy bases.

  You can only load one part type at a time; loading multiple parts
  needs multiple turns.


+ `GBn` (get beam), `GTn` (get torpedo launcher)

  `n` is a digit between 0 and 9, where 0 stands for type 10. This
  will cause the ship to load up as many beams or torpedo launchers of
  the specified type as possible. Otherwise, exactly like `GEn`.


+ `UEn`, `UBn`, `UTn` (unload engine/beam/torpedo launcher)

  `n` is a digit like for the corresponding `Gxn` friendly code. The
  ship will unload all parts of the specified type onto the starbase
  it ends its turn at.

  There is no way to unload only a part of the ship's freight. The
  starbase can be owned by any player; you can "gift" other players
  parts.


+ `UAP` (unload all parts)

  Unload all parts at once.



### Configuration (`psbplus.src`)

The following `psbplus.src` options are relevant for Starship Parts:

+ `TransportComp` (Yes/No; default: Yes)

  Enable the component transport feature.

  Note that if this feature is disabled in mid-game, the "unload"
  friendly codes can still be used to empty possible remaining
  transports.

+ `FreighterCarryOnly` (Yes/No; default: Yes)

  If enabled, only freighters can carry parts. A freighter is any ship
  that has no weapon.

  If a ship becomes ineligible (e.g. by being refitted), it can no
  longer load parts, but can still unload parts it is possibly
  carrying.


+ `NonCloakerCarryOnly` (Yes/No; default: Yes)

  If enabled, only ships that cannot cloak can carry parts.

  If a ship becomes ineligible (e.g. by leveling up), it can no longer
  load parts, but can still unload parts it is possibly carrying.


+ `CargoSpacePerComp` (number; default: 40)

  The minimum cargo space allocated for each part aboard a ship.


+ `TagSpecialTransport` (Yes/No; default: Yes)

  If enabled, ships that carry parts will receive a "ST:" prefix for
  their ship name.

  Unless ship names are filtered at the host side (pconfig.src
  `AllowShipNames=No`), that prefix will be visible to all players.



Others
------

+ `con` (configuration; planet friendly code)

  Starbase Reloaded reacts to the `con` friendly code by sending you
  the current configuration settings as a set of subspace messages.



Host Order
----------

### AUXHOST1 (before movement)

+ Mine sweeping and scooping in order of starbase Id
+ Mine laying in order of starbase Id


### AUXHOST2 (after movement)

+ Component loading and unloading in order of ship Id
+ Credit transfers
+ Config transmission
