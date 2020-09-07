/**
  *  \file mine.c
  *  \brief Starbase Reloaded - Mine Laying and Sweeping
  */

#include <phostpdk.h>
#include "mine.h"
#include "config.h"
#include "message.h"
#include "util.h"
#include "utildata.h"

/* With this option set, we try to reproduce psbplus behaviour more closely:
   torpedo-to-mine uses tech-squared, not slot-squared.
   Slot-squared is more consistent with the rest of the game, and seems to be
   what the original documentation (STARBASE.TXT) implies. */
static const Boolean USE_TORP_TECH = False;

/* Maximum number of torpedoes in base storage.
   PHost seems to put the limit at 32767, but many others (including HOST, c2ng) put it at 10000.
   This should still be more than enough. */
static const Uns16 MAX_BASE_TORPS = 10000;


/*
 *  Mine Laying
 */

static Uns32 UnitsPerTorpedoRate(const struct Config* c, Uns16 torpNr, Boolean isWeb)
{
    (void) c;
    (void) isWeb;

    if (USE_TORP_TECH) {
        torpNr = TorpTechLevel(torpNr);
    }

    Uns32 rate = torpNr * torpNr;
    return MAX(rate, 1);
}

static Uns32 MaxMinefieldUnits(RaceType_Def owner, Boolean isWeb)
{
    Uns32 maxRadius = isWeb
        ? gPconfigInfo->MaximumWebMinefieldRadius[owner]
        : gPconfigInfo->MaximumMinefieldRadius[owner];

    return maxRadius*maxRadius;
}

static Uns16 FindMinefieldForLaying(Uns16 planetId, RaceType_Def owner, Boolean isWeb)
{
    const Uns16* candidates = EnumerateMinesCovering(PlanetLocationX(planetId), PlanetLocationY(planetId));
    for (size_t i = 0; candidates[i] != 0; ++i) {
        const Uns16 cand = candidates[i];
        if (MinefieldOwner(cand) == owner && IsMinefieldWeb(cand) == isWeb) {
            return cand;
        }
    }
    return 0;
}

static Uns32 UnitsToLay(const struct Config* c, RaceType_Def owner, Boolean isWeb, Uns32 existingUnits, Uns16 torpNr, Uns16 torps)
{
    // Maximum number that can be added without exceeding the maximum size:
    const Uns32 permittedUnits = MaxMinefieldUnits(owner, isWeb);
    const Uns32 addibleUnits = existingUnits < permittedUnits ? permittedUnits - existingUnits : 0;

    // Units we add now
    const Uns32 rate = UnitsPerTorpedoRate(c, torpNr, isWeb);
    return MIN(torps * rate, addibleUnits);
}

static void RemoveTorpedoes(const struct Config* c, Uns16 planetId, Uns16 torpNr, Boolean isWeb, Uns32 unitsNow)
{
    // If a fractional torpedo was laid, remove it entirely.
    const Uns32 rate = UnitsPerTorpedoRate(c, torpNr, isWeb);
    const Uns16 torps = BaseTorps(planetId, torpNr) - (unitsNow + (rate-1)) / rate;

    PutBaseTorps(planetId, torpNr, torps);
}

static void LayMinefield(const struct Config* c, Uns16 planetId, RaceType_Def owner, Boolean isWeb)
{
    Uns16 mineId = 0;
    Uns32 unitsLaid = 0;
    for (Uns16 torpNr = TORP_NR; torpNr >= 1; --torpNr) {
        Uns16 torps = BaseTorps(planetId, torpNr);
        if (torps > 0) {
            // Locate minefield
            if (mineId == 0) {
                mineId = FindMinefieldForLaying(planetId, owner, isWeb);
            }
            if (mineId != 0) {
                // We have an existing minefield. Enlarge it if possible.
                const Uns32 existingUnits = MinefieldUnits(mineId);
                const Uns32 unitsNow = UnitsToLay(c, owner, isWeb, existingUnits, torpNr, torps);

                // Enlarge minefield
                PutMinefieldUnits(mineId, existingUnits + unitsNow);
                unitsLaid += unitsNow;

                RemoveTorpedoes(c, planetId, torpNr, isWeb, unitsNow);
            } else {
                // No minefield there. Create one.
                const Uns32 unitsNow = UnitsToLay(c, owner, isWeb, 0, torpNr, torps);

                mineId = CreateMinefield(PlanetLocationX(planetId), PlanetLocationY(planetId), owner, unitsNow, isWeb);
                if (mineId == 0) {
                    Info("\t(-) Base %d, player %d: failure to lay minefield", planetId, owner);
                    break;
                }

                unitsLaid += unitsNow;
                RemoveTorpedoes(c, planetId, torpNr, isWeb, unitsNow);
            }
        }
    }

    // Send message
    if (unitsLaid > 0 && mineId > 0) {
        Info("\t(+) Base %d, player %d, minefield %d: laid %d units", planetId, owner, mineId, (int) unitsLaid);
        Message_MinefieldLaid(owner, planetId, mineId, MinefieldPositionX(mineId), MinefieldPositionY(mineId), unitsLaid, MinefieldUnits(mineId), MinefieldRadius(mineId), isWeb);
        Util_Minefield(owner, mineId, MinefieldPositionX(mineId), MinefieldPositionY(mineId), MinefieldOwner(mineId), MinefieldUnits(mineId), IsMinefieldWeb(mineId), MINE_LAID);
    }
}

void DoMineLaying(const struct Config* c)
{
    if (c->LayMinefields || c->LayWebMinefields) {
        Info("    Laying minefields...");
        for (Uns16 i = 1; i <= PLANET_NR; ++i) {
            if (IsBaseExist(i)) {
                RaceType_Def owner = PlanetOwner(i);
                if (c->LayMinefields && PlanetHasFCode(i, "LMF")) {
                    LayMinefield(c, i, owner, False);
                }
                if (c->LayWebMinefields && gPconfigInfo->PlayerSpecialMission[owner] == 7 && PlanetHasFCode(i, "LWF")) {
                    LayMinefield(c, i, owner, True);
                }
            }
        }
        if (c->LayMinefields) {
            DefineSpecialFCode("LMF");
        }
        if (c->LayWebMinefields) {
            DefineSpecialFCode("LWF");
        }
    } else {
        Info("    Laying minefields disabled.");
    }
}

/*
 *  Sweeping/Scooping
 */

static Uns32 BeamSweepCapacity(const struct Config* c, Uns16 planetId, Boolean isWeb)
{
    // CHANGE: pstarbase divide by 20 last, but STARBASE.TXT says we do it here
    Uns16 numBeams = BaseDefense(planetId) / 20;
    Uns16 beamTech = BaseTech(planetId, BEAM_TECH);
    Uns16 rate = isWeb ? c->BeamWebSweepRate : c->BeamSweepRate;

    return (Uns32) rate * beamTech *  beamTech * numBeams;
}

static Boolean PlanetSweepsMine(Uns16 planetId, Uns16 mineId)
{
    RaceType_Def planetOwner = PlanetOwner(planetId);
    RaceType_Def mineOwner = MinefieldOwner(mineId);

    if (planetOwner == mineOwner) {
        return False;
    }

    if (PlayersAreAllies(planetOwner, mineOwner)
        && PlayerAllowsAlly(planetOwner, mineOwner, ALLY_MINES)
        && PlayerAllowsAlly(mineOwner, planetOwner, ALLY_MINES))
    {
        return False;
    }

    return True;
}

static void SweepFromPlanet(Uns16 planetId, Uns32 mineCapacity, Uns32 webCapacity, Uns16 range, Boolean withFighters)
{
    // No need to gather mines if rate is 0 (by base having too little defense or disabled)
    if (mineCapacity == 0 && webCapacity == 0) {
        return;
    }

    // Check candidates
    const Uns16* candidates = EnumerateMinesWithinRadius(PlanetLocationX(planetId), PlanetLocationY(planetId), range);
    for (size_t i = 0; candidates[i] != 0; ++i) {
        const Uns16 mineId = candidates[i];
        if (PlanetSweepsMine(planetId, mineId)) {
            // Capture old minefield state
            const RaceType_Def oldOwner = MinefieldOwner(mineId);
            const Uns16 oldX = MinefieldPositionX(mineId);
            const Uns16 oldY = MinefieldPositionY(mineId);
            const Uns16 oldRadius = MinefieldRadius(mineId);
            const Boolean oldWeb = IsMinefieldWeb(mineId);

            // Compute loss
            const Uns32 existingUnits = MinefieldUnits(mineId);
            const Uns32 capacity = oldWeb ? webCapacity : mineCapacity;
            const Uns32 sweptUnits = MIN(existingUnits, capacity);
            const Uns32 remainingUnits = existingUnits - sweptUnits;

            PutMinefieldUnits(mineId, remainingUnits);

            Info("\t(+) Base %d, minefield %d: sweep %d units using %s", planetId, mineId, sweptUnits, withFighters ? "fighters" : "beams");
            Message_MinefieldSwept(PlanetOwner(planetId), planetId, mineId, oldX, oldY, oldOwner, oldRadius, sweptUnits, remainingUnits, oldWeb, withFighters);
            Util_Minefield(PlanetOwner(planetId), mineId, oldX, oldY, oldOwner, remainingUnits, oldWeb, MINE_SWEPT);
        }
    }
}

static void SweepUsingBeams(const struct Config* c, Uns16 planetId)
{
    SweepFromPlanet(planetId, BeamSweepCapacity(c, planetId, False), BeamSweepCapacity(c, planetId, True), c->BeamSweepRate, False);
}

static void SweepUsingFighters(const struct Config* c, Uns16 planetId)
{
    Uns32 mineCapacity, webCapacity;
    Uns16 range;
    if (gPconfigInfo->PlayerRace[PlanetOwner(planetId)] == Colonies) {
        // Colonies: can always sweep mines with fighters; can sweep webs if configured in PCONFIG; always 100 ly range.
        mineCapacity = c->FtrSweepRate;
        webCapacity  = gPconfigInfo->ColSweepWebs ? c->FtrWebSweepRate : 0;
        range = 100;
    } else {
        // Others: can sweep mines only if configured; can never sweep webs; dynamic range.
        mineCapacity = c->ColonialFighterOnlySweepMines ? 0 : c->FtrSweepRate;
        webCapacity = 0;
        range = 10 * ((BaseTech(planetId, HULL_TECH) + BaseTech(planetId, ENGINE_TECH) + BaseTech(planetId, BEAM_TECH)) / 3);
    }

    mineCapacity *= BaseFighters(planetId);
    webCapacity *= BaseFighters(planetId);

    SweepFromPlanet(planetId, mineCapacity, webCapacity, range, True);
}

static Boolean PlanetScoopsMine(Uns16 planetId, Uns16 mineId)
{
    RaceType_Def planetOwner = PlanetOwner(planetId);
    RaceType_Def mineOwner = MinefieldOwner(mineId);

    return (planetOwner == mineOwner);
}

static Uns16 TorpNrForScooping(Uns16 planetId)
{
    // We always scoop into the best torpedo slot the base can build.
    Uns16 torpTech = BaseTech(planetId, TORP_TECH);
    Uns16 torpNr = TORP_NR;
    while (torpNr > 1 && TorpTechLevel(torpNr) > torpTech) {
        --torpNr;
    }
    return torpNr;
}

static void ScoopFromPlanet(const struct Config* c, Uns16 planetId)
{
    Uns16* candidates = EnumerateMinesWithinRadius(PlanetLocationX(planetId), PlanetLocationY(planetId), c->BeamSweepRange);
    for (size_t i = 0; candidates[i] != 0; ++i) {
        Uns16 mineId = candidates[i];
        if (PlanetScoopsMine(planetId, mineId)) {
            const Uns16 oldRadius = MinefieldRadius(mineId);
            const Uns32 existingUnits = MinefieldUnits(mineId);

            // Determine scooping rate. Scooping rate is same as laying rate.
            const Uns32 torpNr = TorpNrForScooping(planetId);
            const Uns32 torpRate = UnitsPerTorpedoRate(c, torpNr, IsMinefieldWeb(mineId));

            // Determine number of torpedoes we get by sweeping the entire field.
            // A fractional torpedo is discarded.
            const Uns16 existingTorps = BaseTorps(planetId, torpNr);
            Uns32 newTorps = existingTorps + (existingUnits / torpRate);

            // Check whether torpedoes fit into the base.
            Uns32 remainingUnits;
            if (newTorps <= MAX_BASE_TORPS) {
                // Yes, torpedoes fit. Minefield is gone.
                remainingUnits = 0;
            } else if (existingTorps <= MAX_BASE_TORPS) {
                // Base has some room, but not for everything.
                // Scoop what we can (plus the fractional torpedo which does not end up in storage).
                remainingUnits = (newTorps - MAX_BASE_TORPS) * torpRate;
                newTorps = MAX_BASE_TORPS;
            } else {
                // Base was already overloaded before, don't change anything.
                remainingUnits = existingUnits;
                newTorps = existingTorps;
            }
            PutMinefieldUnits(mineId, remainingUnits);
            PutBaseTorps(planetId, torpNr, newTorps);

            Info("\t(+) Base %d, minefield %d: scooping miness", planetId, mineId);
            Message_MinefieldScooped(PlanetOwner(planetId), planetId, mineId, MinefieldPositionX(mineId), MinefieldPositionY(mineId), oldRadius, newTorps - existingTorps, IsMinefieldWeb(mineId));
            Util_Minefield(PlanetOwner(planetId), mineId, MinefieldPositionX(mineId), MinefieldPositionY(mineId), MinefieldOwner(mineId), remainingUnits, IsMinefieldWeb(mineId), MINE_SWEPT);
        }
    }
}

void DoMineSweeping(const struct Config* c)
{
    if (c->BeamSweepMines || c->FighterSweepMines || c->ScoopMinefields) {
        Info("    Sweeping/scooping minefields...");
        for (Uns16 i = 1; i <= PLANET_NR; ++i) {
            if (IsBaseExist(i)) {
                if (PlanetHasFCode(i, "SMF")) {
                    if (c->BeamSweepMines) {
                        SweepUsingBeams(c, i);
                    }
                    if (c->FighterSweepMines) {
                        SweepUsingFighters(c, i);
                    }
                }
                if (c->ScoopMinefields && PlanetHasFCode(i, "MSC")) {
                    ScoopFromPlanet(c, i);
                }
            }
        }
        if (c->BeamSweepMines || c->FighterSweepMines) {
            DefineSpecialFCode("SMF");
        }
        if (c->ScoopMinefields) {
            DefineSpecialFCode("MSC");
        }
    } else {
        Info("    Sweeping/scooping minefields disabled.");
    }
}
