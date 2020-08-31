/**
  *  \file transport.c
  *  \brief Starbase Reloaded - Component Transport
  */

#include <string.h>
#include <assert.h>
#include "transport.h"
#include "config.h"
#include "util.h"
#include "message.h"
#include "utildata.h"

/*
 *  Definitions
 */

/* Interestingly, PDK doesn't have this. */
#ifndef SHIPNAME_SIZE
# define SHIPNAME_SIZE 20
#endif

/* If set, we accept multiple part types on a ship.
   The original documentation seems to imply that you can carry only one,
   but this doesn't seem to be the case. */
static const Boolean ACCEPT_MULTIPLE_TYPES = True;

/* Name of state file. */
static const char*const STATE_FILE_NAME = "psbplus.hst";

/* Prefix for ship names. */
static const char*const NAME_PREFIX = "ST: ";

/* Maximum number of items in storage.
   See MAX_BASE_TORPS for reasoning. */
static const Uns16 MAX_BASE_COMPONENTS = 10000;


/*
 *  Generic Utilities
 */

static Uns16 BaseComponents(Uns16 planetId, BaseTech_Def type, Uns16 slot)
{
    Uns16 result = 0;
    switch (type) {
     case ENGINE_TECH: result = (slot > 0 && slot <= ENGINE_NR ? BaseEngines(planetId, slot) : 0); break;
     case BEAM_TECH:   result = (slot > 0 && slot <= BEAM_NR   ? BaseBeams(planetId, slot)   : 0); break;
     case TORP_TECH:   result = (slot > 0 && slot <= TORP_NR   ? BaseTubes(planetId, slot)   : 0); break;
     default:;
    }
    // Info("##  BaseComponents(%d,%d,%d) => %d", planetId, type, slot, result);
    return result;
}

static void PutBaseComponents(Uns16 planetId, BaseTech_Def type, Uns16 slot, Uns16 amount)
{
    // Info("##  PutBaseComponents(%d,%d,%d) <= %d", planetId, type, slot, amount);
    switch (type) {
     case ENGINE_TECH:
        if (slot > 0 && slot <= ENGINE_NR) {
            PutBaseEngines(planetId, slot, amount);
        }
        break;
     case BEAM_TECH:
        if (slot > 0 && slot <= BEAM_NR) {
            PutBaseBeams(planetId, slot, amount);
        }
        break;
     case TORP_TECH:
        if (slot > 0 && slot <= TORP_NR) {
            PutBaseTubes(planetId, slot, amount);
        }
        break;
     default:;
    }
}

static Uns16 BaseReservedComponents(Uns16 planetId, BaseTech_Def type, Uns16 slot)
{
    Uns16 result = 0;
    BuildOrder_Struct order;
    if (BaseBuildOrder(planetId, &order)) {
        switch (type) {
         case ENGINE_TECH:
            // FIXME: MapTruehullByPlayerRace?
            if (slot == order.mEngineType) {
                result = HullEngineNumber(EffTrueHull(BaseOwner(planetId), order.mHull));
            }
            break;
         case BEAM_TECH:
            if (slot == order.mBeamType) {
                result = order.mNumBeams;
            }
            break;
         case TORP_TECH:
            if (slot == order.mTubeType) {
                result = order.mNumTubes;
            }
            break;
         default:;
        }
    }
    // Info("##  BaseComponents(%d,%d,%d) => %d", planetId, type, slot, result);
    return result;
}

static const char* ComponentTypeName(BaseTech_Def type)
{
    switch (type) {
     case ENGINE_TECH: return "engine";
     case BEAM_TECH:   return "beam";
     case TORP_TECH:   return "launcher";
     default:          return "?";
    }
}

static Uns16 ComponentMass(const struct Config* c, BaseTech_Def type, Uns16 slot)
{
    // Components weigh at least CargoSpacePerComp, but can weigh more if configured in the ship list.
    // This function must not return 0.
    Uns16 weight = 1;
    switch (type) {
     case BEAM_TECH:
        if (slot > 0 && slot <= BEAM_NR) {
            weight = BeamMass(slot);
        }
        break;
     case TORP_TECH:
        if (slot > 0 && slot <= TORP_NR) {
            weight = TorpTubeMass(slot);
        }
        break;
     default:;
    }

    return MAX(MAX(c->CargoSpacePerComp, weight), 1);
}

/*
 *  TransportState class
 */

void TransportState_Load(struct TransportState* st)
{
    // Zero out state
    memset(st, 0, sizeof(*st));

    // Load the file
    FILE* f = OpenInputFile(STATE_FILE_NAME, GAME_DIR_ONLY | NO_MISSING_ERROR);
    if (f == NULL) {
        Warning("State file (%s) not found, starting with blank slate.", STATE_FILE_NAME);
        return;
    }

    // Version number
    Uns16 version;
    if (!DOSRead16(&version, 1, f) || version != 0) {
        Warning("State file (%s) has unrecognized format, ignoring it.", STATE_FILE_NAME);
        fclose(f);
        return;
    }

    // Read as much as we get to survive a possible Host500 > Host999 transition.
    int shipId = 0;
    struct TransportShip* info;
    while ((info = TransportState_Ship(st, ++shipId)) != 0) {
        if (!DOSRead16(info->Beams, BEAM_NR, f)
            || !DOSRead16(info->Launchers, TORP_NR, f)
            || !DOSRead16(info->Engines, ENGINE_NR, f))
        {
            break;
        }
    }

    fclose(f);
}

void TransportState_Save(struct TransportState* st)
{
    FILE* f = OpenOutputFile(STATE_FILE_NAME, GAME_DIR_ONLY | NO_MISSING_ERROR);
    assert(f);

    // Version number
    Uns16 version = 0;
    Boolean ok = DOSWrite16(&version, 1, f);

    // Content
    int shipId = 0;
    struct TransportShip* info;
    while (ok && (info = TransportState_Ship(st, ++shipId)) != 0) {
        ok = DOSWrite16(info->Beams, BEAM_NR, f)
            && DOSWrite16(info->Launchers, TORP_NR, f)
            && DOSWrite16(info->Engines, ENGINE_NR, f);
    }

    if (!ok) {
        Warning("Error saving state file (%s).", STATE_FILE_NAME);
    }
    fclose(f);
}

struct TransportShip* TransportState_Ship(struct TransportState* st, Uns16 shipId)
{
    if (st != NULL && shipId > 0 && shipId <= SHIP_NR) {
        return &st->Ships[shipId-1];
    } else {
        return 0;
    }
}

/*
 *  TransportShip class
 */

Boolean TransportShip_HasComponents(const struct TransportShip* sh)
{
    return (sh != NULL
            && (AnyNonzero(sh->Engines, ENGINE_NR)
                || AnyNonzero(sh->Beams, BEAM_NR)
                || AnyNonzero(sh->Launchers, TORP_NR)));
}

static void TransportShip_Clear(struct TransportShip* sh)
{
    if (sh != NULL) {
        memset(sh, 0, sizeof(*sh));
    }
}

Uns16 TransportShip_Cargo(const struct TransportShip* sh, BaseTech_Def type, Uns16 slot)
{
    Uns16 result = 0;
    if (sh != NULL) {
        switch (type) {
         case ENGINE_TECH: result = (slot > 0 && slot <= ENGINE_NR ? sh->Engines[slot-1]   : 0); break;
         case BEAM_TECH:   result = (slot > 0 && slot <= BEAM_NR   ? sh->Beams[slot-1]     : 0); break;
         case TORP_TECH:   result = (slot > 0 && slot <= TORP_NR   ? sh->Launchers[slot-1] : 0); break;
         default:;
        }
    }
    // Info("##  TransportShip_Cargo(%p,%d,%d) => %d", (void*) sh, type, slot, result);
    return result;
}

static void TransportShip_PutCargo(struct TransportShip* sh, BaseTech_Def type, Uns16 slot, Uns16 amount)
{
    // Info("##  TransportShip_PutCargo(%p,%d,%d) <= %d", (void*) sh, type, slot, amount);
    if (sh != NULL) {
        switch (type) {
         case ENGINE_TECH:
            if (slot > 0 && slot <= ENGINE_NR) {
                sh->Engines[slot-1] = amount;
            }
            break;
         case BEAM_TECH:
            if (slot > 0 && slot <= BEAM_NR) {
                sh->Beams[slot-1] = amount;
            }
            break;
         case TORP_TECH:
            if (slot > 0 && slot <= TORP_NR) {
                sh->Launchers[slot-1] = amount;
            }
            break;
         default:;
        }
    }
}

static Uns16 TransportShip_CargoMass(const struct TransportShip* sh, const struct Config* c)
{
    Uns16 total = 0;
    for (int i = 1; i <= ENGINE_NR; ++i) {
        total += ComponentMass(c, ENGINE_TECH, i) * TransportShip_Cargo(sh, ENGINE_TECH, i);
    }
    for (int i = 1; i <= BEAM_NR; ++i) {
        total += ComponentMass(c, BEAM_TECH, i) * TransportShip_Cargo(sh, BEAM_TECH, i);
    }
    for (int i = 1; i <= TORP_NR; ++i) {
        total += ComponentMass(c, TORP_TECH, i) * TransportShip_Cargo(sh, TORP_TECH, i);
    }
    return total;
}

/*
 *  Rule Configuration
 */

static Boolean ShipCanLoadComponents(Uns16 shipId, const struct Config* c)
{
    if (c->FreighterCarryOnly && (ShipBeamNumber(shipId) > 0 || ShipTubeNumber(shipId) > 0 || ShipBays(shipId) > 0)) {
        return False;
    }

    if (c->NonCloakerCarryOnly && ShipCanCloak(shipId)) {
        return False;
    }

    return True;
}

static Uns16 ShipCanAcceptComponent(const struct TransportShip* sh, const struct Config* c, BaseTech_Def type, Uns16 slot)
{
    (void) c;

    return ACCEPT_MULTIPLE_TYPES
        || !TransportShip_HasComponents(sh)
        || TransportShip_Cargo(sh, type, slot) != 0;
}


/*
 *  Action
 */

static void GetComponent(struct TransportShip* sh, const struct Config* c, Uns16 shipId, Uns16 planetId, BaseTech_Def type, Uns16 slot)
{
    // Ship must be allowed to load components
    if (!ShipCanLoadComponents(shipId, c)) {
        Info("\t(-) Ship %d: not allowed to load components", shipId);
        Message_Transport_LoadNotPermitted(ShipOwner(shipId), shipId);
        return;
    }

    // Base must have components
    const Uns16 baseComponents = BaseComponents(planetId, type, slot);
    const Uns16 reservedComponents = BaseReservedComponents(planetId, type, slot);
    if (baseComponents <= reservedComponents) {
        Info("\t(-) Ship %d, base %d: load: no matching component on base", shipId, planetId);
        Message_Transport_LoadNoParts(ShipOwner(shipId), shipId, planetId);
        return;
    }

    // Ship must be able to accept components of this type
    if (!ShipCanAcceptComponent(sh, c, type, slot)) {
        Info("\t(-) Ship %d, base %d: load: conflicting component on ship", shipId, planetId);
        Message_Transport_LoadConflictingParts(ShipOwner(shipId), shipId);
        return;
    }

    // Determine mass of component
    const Uns16 compMass = ComponentMass(c, type, slot);

    // Determine mass already on ship
    const Uns16 shipCargo = ShipCargoMass(shipId) + TransportShip_CargoMass(sh, c);
    const Uns16 maxCargo = HullCargoCapacity(ShipHull(shipId));

    // Determine maximum number of components
    // Careful in case ship is already overloaded.
    const Uns16 maxComponents = (shipCargo >= maxCargo ? 0 : (maxCargo - shipCargo) / compMass);
    if (maxComponents == 0) {
        Info("\t(-) Ship %d, base %d: load: out of space on ship", shipId, planetId);
        Message_Transport_LoadNoSpace(ShipOwner(shipId), shipId);
        return;
    }

    // OK, do it
    const Uns16 numComponents = MIN(baseComponents - reservedComponents, maxComponents);
    TransportShip_PutCargo(sh, type, slot, TransportShip_Cargo(sh, type, slot) + numComponents);
    PutBaseComponents(planetId, type, slot, baseComponents - numComponents);
    Info("\t(+) Ship %d, base %d: loaded %d %s-%d", shipId, planetId, numComponents, ComponentTypeName(type), slot);
    Message_Transport_LoadSuccess(ShipOwner(shipId), shipId, planetId, numComponents);
}

static Uns16 UnloadSingleComponent(struct TransportShip* sh, Uns16 planetId, BaseTech_Def type, Uns16 slot, Uns16 shipComponents)
{
    // Determine number of components on base
    const Uns16 baseComponents = BaseComponents(planetId, type, slot);

    // Determine how many we can add without overflowing
    const Uns16 maxComponents = (baseComponents >= MAX_BASE_COMPONENTS ? 0 : MAX_BASE_COMPONENTS - baseComponents);

    // Determine how many we want to add
    const Uns16 numComponents = MIN(shipComponents, maxComponents);

    // Move them
    TransportShip_PutCargo(sh, type, slot, TransportShip_Cargo(sh, type, slot) - numComponents);
    PutBaseComponents(planetId, type, slot, baseComponents + numComponents);
    return numComponents;
}

static void UnloadComponent(struct TransportShip* sh, Uns16 shipId, Uns16 planetId, BaseTech_Def type, Uns16 slot)
{
    // Determine number of components on ship
    Uns16 shipComponents = TransportShip_Cargo(sh, type, slot);
    if (shipComponents == 0) {
        Info("\t(-) Ship %d, base %d: unload: no matching component on ship", shipId, planetId);
        Message_Transport_UnloadNoParts(ShipOwner(shipId), shipId);
        return;
    }

    // Unload and generate messages
    // For now, do not special-case "no space on base".
    Uns16 numComponents = UnloadSingleComponent(sh, planetId, type, slot, shipComponents);
    Info("\t(+) Ship %d, base %d: unloaded %d %s-%d", shipId, planetId, numComponents, ComponentTypeName(type), slot);
    Message_Transport_UnloadSuccess(ShipOwner(shipId), shipId, planetId, numComponents);
}

static void UnloadAll(struct TransportShip* sh, Uns16 shipId, Uns16 planetId)
{
    // Unload everything
    Uns32 total = 0;
    for (Uns16 i = 1; i <= ENGINE_NR; ++i) {
        total += UnloadSingleComponent(sh, planetId, ENGINE_TECH, i, TransportShip_Cargo(sh, ENGINE_TECH, i));
    }
    for (Uns16 i = 1; i <= BEAM_NR; ++i) {
        total += UnloadSingleComponent(sh, planetId, BEAM_TECH, i, TransportShip_Cargo(sh, BEAM_TECH, i));
    }
    for (Uns16 i = 1; i <= TORP_NR; ++i) {
        total += UnloadSingleComponent(sh, planetId, TORP_TECH, i, TransportShip_Cargo(sh, TORP_TECH, i));
    }

    // Generate messages
    // For now, do not distinguish between "nothing aboard" and "no space on base" which both end up with total=0.
    Info("\t(+) Ship %d, base %d: unloaded %d components", shipId, planetId, total);
    if (total == 0) {
        Message_Transport_UnloadNoParts(ShipOwner(shipId), shipId);
    } else {
        Message_Transport_UnloadSuccess(ShipOwner(shipId), shipId, planetId, total);
    }
}

static void UntagShips()
{
    for (Uns16 shipId = 1; shipId <= SHIP_NR; ++shipId) {
        char buf[SHIPNAME_SIZE+1];
        if (IsShipExist(shipId)) {
            ShipName(shipId, buf);
            if (memcmp(buf, NAME_PREFIX, strlen(NAME_PREFIX)) == 0) {
                PutShipName(shipId, &buf[strlen(NAME_PREFIX)]);
            }
        }
    }
}

static void TagShips(struct TransportState* st)
{
    for (Uns16 shipId = 1; shipId <= SHIP_NR; ++shipId) {
        if (IsShipExist(shipId) && TransportShip_HasComponents(TransportState_Ship(st, shipId))) {
            // Rename it; build new name in-place.
            char buf[SHIPNAME_SIZE + 10];
            strcpy(buf, NAME_PREFIX);
            ShipName(shipId, &buf[strlen(NAME_PREFIX)]);
            PutShipName(shipId, buf);
        }
    }
}

/*
 *  Ship Report Generation
 */

struct ReportShip_State {
    Uns32 args[2];
    struct Message m;
    const struct Config* config;
};

static void ReportShip_Add(struct ReportShip_State* st, const struct TransportShip* sh, BaseTech_Def type, Uns16 slot, const char* name, const char* fcPrefix)
{
    const Uns16 amount = TransportShip_Cargo(sh, type, slot);
    if (amount != 0) {
        const Uns16 shipId = st->args[0];
        if (st->m.Lines >= MAX_MESSAGE_LINES) {
            Message_Add(&st->m, "(continued on next page)\n");
            Message_Send(&st->m, ShipOwner(shipId));
            Message_Init(&st->m);
            Message_Format(&st->m,
                           "(-f%0I)<<< Special Transport >>>\n"
                           "\n"
                           "(continued inventory)\n",
                           st->args, 2);
        }

        char line[50];
        snprintf(line, sizeof(line), "%3d x %-20s [%s%d]\n", amount, name, fcPrefix, slot % 10);
        Message_Add(&st->m, line);
        Util_Transport_Component(ShipOwner(shipId), shipId, type, slot, amount, ComponentMass(st->config, type, slot));
    }
}

static void ReportShip(struct TransportShip* sh, const struct Config* c, Uns16 shipId)
{
    char name[40];

    const Uns16 totalCargo = TransportShip_CargoMass(sh, c);
    struct ReportShip_State st;
    st.args[0] = shipId;
    st.args[1] = totalCargo;
    st.config = c;
    Message_Init(&st.m);
    Message_Format(&st.m,
                   "(-f%0I)<<< Special Transport >>>\n"
                   "\n"
                   "FROM: %0S\n"
                   "  Ship %0d\n"
                   "\n"
                   "We are currently carrying components\n"
                   "with a total weight of %1d kt:\n",
                   st.args, 2);
    Util_Transport_Summary(ShipOwner(shipId), shipId, totalCargo);

    for (Uns16 i = 1; i <= ENGINE_NR; ++i) {
        ReportShip_Add(&st, sh, ENGINE_TECH, i, EngineName(i, name), "UE");
    }
    for (Uns16 i = 1; i <= BEAM_NR; ++i) {
        ReportShip_Add(&st, sh, BEAM_TECH, i, BeamName(i, name), "UB");
    }
    for (Uns16 i = 1; i <= TORP_NR; ++i) {
        ReportShip_Add(&st, sh, TORP_TECH, i, TorpName(i, name), "UT");
    }

    Message_Send(&st.m, ShipOwner(shipId));
}

static void ReportShips(struct TransportState* st, const struct Config* c)
{
    for (Uns16 shipId = 1; shipId <= SHIP_NR; ++shipId) {
        if (IsShipExist(shipId) && TransportShip_HasComponents(TransportState_Ship(st, shipId))) {
            ReportShip(TransportState_Ship(st, shipId), c, shipId);
        }
    }
}

/*
 *  Cargo Trimming
 */

static Boolean RemoveComponent(struct TransportShip* sh, BaseTech_Def type, Uns16 limit)
{
    for (Uns16 slot = 1; slot <= limit; ++slot) {
        Uns16 have = TransportShip_Cargo(sh, type, slot);
        if (have > 0) {
            TransportShip_PutCargo(sh, type, slot, have-1);
            return True;
        }
    }
    return False;
}

static Boolean RemoveCargo(Uns16 shipId, CargoType_Def what, Uns16* acc, Uns16 limit)
{
    Uns16 have = ShipCargo(shipId, what);
    Uns16 drop = MIN(have, MIN(*acc, limit));
    if (drop != 0) {
        PutShipCargo(shipId, what, have - drop);
        *acc -= drop;
        return True;
    } else {
        return False;
    }
}

static Boolean RemoveAmmo(Uns16 shipId, Uns16* acc, Uns16 limit)
{
    Uns16 have = ShipAmmunition(shipId);
    Uns16 drop = MIN(have, MIN(*acc, limit));
    if (drop != 0) {
        PutShipAmmunition(shipId, have - drop);
        *acc -= drop;
        return True;
    } else {
        return False;
    }
}

static void TrimSingleShipCargo(struct TransportShip* sh, const struct Config* c, Uns16 shipId)
{
    /*
       Things we do not do and why:

       Do not fix ships overloaded with regular cargo ONLY; PHost does that.
       (This is the TransportShip_HasComponents check in TrimCargo().)
       However, we need to deal with possibly overloaded ships at every place.

       Do not drop cargo from ships that cannot carry components.
       Ships can become ineligible to carry components by refit or by advancing an experience level
       (or by configuration change).

       Do not try to unload onto a planet.
       This would allow people to unload components without spending a friendly code.
       It is not guaranteed to work (overflow).
     */

    // Determine maximum mass for components.
    const Uns16 maxTotalCargo = HullCargoCapacity(ShipHull(shipId));

    // Pass 1: drop components that exceed the ship's cargo room
    // (e.g. a ship with 200 kt cargo room but 20 components)
    Uns16 droppedComponents = 0;
    Uns16 componentMass = TransportShip_CargoMass(sh, c);
    const Uns16 originalMass = componentMass;
    while (componentMass > maxTotalCargo) {
        Boolean ok = False;
        if (RemoveComponent(sh, BEAM_TECH, BEAM_NR)) {
            ++droppedComponents;
            componentMass = TransportShip_CargoMass(sh, c);
            if (componentMass <= maxTotalCargo) {
                break;
            }
            ok = True;
        }
        if (RemoveComponent(sh, TORP_TECH, TORP_NR)) {
            ++droppedComponents;
            componentMass = TransportShip_CargoMass(sh, c);
            if (componentMass <= maxTotalCargo) {
                break;
            }
            ok = True;
        }
        if (RemoveComponent(sh, ENGINE_TECH, ENGINE_NR)) {
            ++droppedComponents;
            componentMass = TransportShip_CargoMass(sh, c);
            if (componentMass <= maxTotalCargo) {
                break;
            }
            ok = True;
        }
        if (!ok) {
            // Unable to trim more
            break;
        }
    }

    // Report message
    if (droppedComponents != 0) {
        const Uns16 droppedMass = originalMass - componentMass;
        Info("\t(+) Ship %d: trimmed cargo: %d components, %d kt", shipId, droppedComponents, droppedMass);
        Message_Transport_TrimmedComponents(ShipOwner(shipId), shipId, droppedComponents, droppedMass);
    }

    // Pass 2: trim excess cargo
    const Uns16 maxCargoMass = maxTotalCargo - componentMass;
    const Uns16 cargoMass = ShipCargoMass(shipId);
    if (cargoMass > maxCargoMass) {
        Uns16 toDrop = cargoMass - maxCargoMass;
        while (toDrop > 0) {
            // Remove from all types of cargo in equal amounts.
            // To speed things up, remove more than just one during first iterations.
            // If we got, say, 100 kt excess, we need to remove 100/6 = 16 from each type at least.
            // If cargo is already equally distributed, this takes only few iterations.
            // If only one type remains, this will eventually remove stuff in 1 kt increments,
            // like the naive algorithm would do anyway.
            Uns16 toDropNow = MAX(toDrop / 6, 1);
            Boolean ok =
                  RemoveCargo(shipId, TRITANIUM, &toDrop, toDropNow);
            ok |= RemoveCargo(shipId, DURANIUM, &toDrop, toDropNow);
            ok |= RemoveCargo(shipId, MOLYBDENUM, &toDrop, toDropNow);
            ok |= RemoveCargo(shipId, SUPPLIES, &toDrop, toDropNow);
            ok |= RemoveCargo(shipId, COLONISTS, &toDrop, toDropNow);
            ok |= RemoveAmmo(shipId, &toDrop, toDropNow);
            if (!ok) {
                break;
            }
        }

        const Uns16 droppedMass = cargoMass - ShipCargoMass(shipId);
        Info("\t(+) Ship %d: trimmed regular cargo: %d kt", shipId, droppedMass);
        Message_Transport_TrimmedCargo(ShipOwner(shipId), shipId, droppedMass);
    }
}

static void TrimCargo(struct TransportState* st, const struct Config* c)
{
    for (Uns16 shipId = 1; shipId <= SHIP_NR; ++shipId) {
        struct TransportShip* sh = TransportState_Ship(st, shipId);
        if (IsShipExist(shipId)) {
            // Trim single ship cargo
            if (TransportShip_HasComponents(sh)) {
                TrimSingleShipCargo(sh, c, shipId);
            }
        } else {
            // Ship does not exist; just discard all the stuff
            TransportShip_Clear(sh);
        }
    }
}

static void HandleNewShips(struct TransportState* st)
{
    /*
     *  A ship may be destroyed and rebuilt the same turn.
     *  If it was carrying components, we must reset its cargo.
     *  Unfortunately, there is no easy way to detect that.
     *
     *  The approach I'm taking here is to scan UTIL.TMP fo "ship built" records,
     *  and reset all ships seen.
     *
     *  An alternative approach would be to hook into a pcontrol
     *  stage shortly after combat, which requires extra setup for hosts.
     */

    // Open file
    FILE* fp = OpenInputFile("util.tmp", GAME_DIR_ONLY | NO_MISSING_ERROR);
    if (fp == NULL) {
        Warning("Unable to open util.tmp; newly-built ships will not be cleaned.");
        return;
    }

    // File format definitions
    enum { PlayerSlot, TypeSlot, SizeSlot, HEADER_SIZE };
    Uns16 header[HEADER_SIZE];

    const Uns16 Type_ShipBuilt = 20;

    enum { ShipSlot, BaseSlot, BODY_SIZE };
    Uns16 body[BODY_SIZE];

    // Read file
    while (DOSRead16(header, HEADER_SIZE, fp)) {
        if (header[TypeSlot] == Type_ShipBuilt && header[SizeSlot] >= sizeof(body)) {
            // Found an applicable record; read it
            if (!DOSRead16(body, BODY_SIZE, fp)) {
                Warning("Unable to read util.tmp; aborting mid-way.");
                break;
            }

            // Reset the mentioned ship
            const Uns16 shipId = body[ShipSlot];
            struct TransportShip* sh = TransportState_Ship(st, shipId);
            if (TransportShip_HasComponents(sh)) {
                Info("\t(!) Ship %d: was rebuilt, reset cargo", shipId);
                TransportShip_Clear(sh);
            }

            // Skip fewer bytes
            header[SizeSlot] -= sizeof(body);
        }

        // Skip record content
        fseek(fp, header[SizeSlot], SEEK_CUR);
    }
    fclose(fp);
}


/*
 *  Public Entry Points
 */

void DoTrimCargo(const struct Config* c)
{
    struct TransportState st;

    Info("    Trimming cargo...");
    TransportState_Load(&st);
    TrimCargo(&st, c);
    TransportState_Save(&st);
}

void DoComponentTransport(const struct Config* c)
{
    struct TransportState st;

    Info("    Component transports...");

    // Load state
    TransportState_Load(&st);

    // Untag all ships to to avoid players doing fun things
    if (c->TagSpecialTransport) {
        UntagShips();
    }

    // Scan for newly-built ships and remove their components
    HandleNewShips(&st);

    // Trim overloaded ships
    TrimCargo(&st, c);

    // Unload all ships
    for (Uns16 shipId = 1; shipId <= SHIP_NR; ++shipId) {
        int slot;
        Uns16 planetId;
        struct TransportShip* sh;
        if (IsShipExist(shipId)
            && (sh = TransportState_Ship(&st, shipId))
            && (planetId = FindPlanetAtShip(shipId)) != 0
            && IsBaseExist(planetId))
        {
            // Unloading works at any base and regardless of configuration.
            // Players need to be able to get rid of components after transport has been turned off.
            // Players can gift components to others.
            if (ShipHasFCode(shipId, "UAP")) {
                UnloadAll(sh, shipId, planetId);
            } else if ((slot = ShipMatchFCode(shipId, "UE", ENGINE_NR)) != 0) {
                UnloadComponent(sh, shipId, planetId, ENGINE_TECH, slot);
            } else if ((slot = ShipMatchFCode(shipId, "UB", BEAM_NR)) != 0) {
                UnloadComponent(sh, shipId, planetId, BEAM_TECH, slot);
            } else if ((slot = ShipMatchFCode(shipId, "UT", TORP_NR)) != 0) {
                UnloadComponent(sh, shipId, planetId, TORP_TECH, slot);
            }
        }
    }

    // Load all ships
    if (c->TransportComp) {
        for (Uns16 shipId = 1; shipId <= SHIP_NR; ++shipId) {
            int slot;
            Uns16 planetId;
            struct TransportShip* sh;
            if (IsShipExist(shipId)
                && (sh = TransportState_Ship(&st, shipId))
                && (planetId = FindPlanetAtShip(shipId)) != 0
                && IsBaseExist(planetId)
                && ShipOwner(shipId) == PlanetOwner(planetId))
            {
                // Loading only works at own bases, and only when configured.
                if ((slot = ShipMatchFCode(shipId, "GE", ENGINE_NR)) != 0) {
                    GetComponent(sh, c, shipId, planetId, ENGINE_TECH, slot);
                } else if ((slot = ShipMatchFCode(shipId, "GB", BEAM_NR)) != 0) {
                    GetComponent(sh, c, shipId, planetId, BEAM_TECH, slot);
                } else if ((slot = ShipMatchFCode(shipId, "GT", TORP_NR)) != 0) {
                    GetComponent(sh, c, shipId, planetId, TORP_TECH, slot);
                }
            }
        }
    }

    // Send all reports
    ReportShips(&st, c);

    // Tag all ships that carry components
    if (c->TagSpecialTransport) {
        TagShips(&st);
    }

    // Save state
    TransportState_Save(&st);
}
