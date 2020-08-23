/**
  *  \file transport.h
  *  \brief Starbase Reloaded - Component Transport
  */
#ifndef TRANSPORT_H_INCLUDED
#define TRANSPORT_H_INCLUDED

#include <phostpdk.h>

struct Config;

/*
 *  Classes
 */

/** State for a single ship. */
struct TransportShip {
    Uns16 Engines[ENGINE_NR];     /**< Loaded engines. Indexed by Id-1. */
    Uns16 Beams[BEAM_NR];         /**< Loaded beams. Indexed by Id-1. */
    Uns16 Launchers[TORP_NR];     /**< Loaded torpedo launchers. Indexed by Id-1. */
};

/** State for all ships. */
struct TransportState {
    struct TransportShip Ships[SHIP_NR];   /**< For each ship, its carried cargo. Indexed by Id-1. */
};

/** Load state.
    If state file does not exist, state is initialized to empty.
    @param [out] st State
    @pre PDK initialized (gGameDirectory set) */
void TransportState_Load(struct TransportState* st);

/** Save state.
    @param [in] st State
    @pre PDK initialized (gGameDirectory set) */
void TransportState_Save(struct TransportState* st);

/** Access state for one ship.
    @param [in] st     State
    @param [in] shipId Ship Id
    @return Ship state; NULL if st is NULL or shipId is out of range. The return value can therefore also serve as a range check. */
struct TransportShip* TransportState_Ship(struct TransportState* st, Uns16 shipId);

/** Check for components on ship.
    @param [in] sh Ship state
    @return True if ship carries any component */
Boolean TransportShip_HasComponents(const struct TransportShip* sh);

/** Get components on ship.
    @param [in] sh   Ship state
    @param [in] type Component type (ENGINE_TECH, BEAM_TECH, TORP_TECH)
    @param [in] slot Component slot (1-based)
    @return Number of components; 0 if parameters invalid or out of range */
Uns16 TransportShip_Cargo(const struct TransportShip* sh, BaseTech_Def type, Uns16 slot);


/*
 *  Main entry points
 */

/** Cargo trimming.
    This will load state, perform cargo trimming, and save again.
    For use during auxhost1.
    @param [in] c Configuration */
void DoTrimCargo(const struct Config* c);

/** Component transport stage.
    This will load state, perform all actions (including cargo trimming), and save again.
    For use during auxhost2.
    @param [in] c Configuration */
void DoComponentTransport(const struct Config* c);

#endif
