/**
  *  \file utildata.h
  *  \brief Starbase Reloaded - util.data records
  */
#ifndef UTILDATA_H_INCLUDED
#define UTILDATA_H_INCLUDED

#include <phostpdk.h>

enum MineReason {
    MINE_LAID = 0,
    MINE_SWEPT = 1,
    MINE_SCANNED = 2
};

/** Write a "Transport Summary" record.
    One such record is written for every special transport.
    @param to             Receiver
    @param shipId         Ship Id
    @param totalCargo     Total cargo room blocked by starship parts */
void Util_Transport_Summary(RaceType_Def to, Uns16 shipId, Uns16 totalCargo);

/** Write a "Transport Component" record.
    One such record is written for every component type on a special transport (that is, multiple per ship).
    @param to             Receiver
    @param shipId         Ship Id
    @param type           Component type (beam/engine/launcher)
    @param slot           Slot number (beam/engine/torpedo type)
    @param numComponents  Number of components of this type
    @param componentMass  Mass of each of these components */
void Util_Transport_Component(RaceType_Def to, Uns16 shipId, BaseTech_Def type, Uns16 slot, Uns16 numComponents, Uns16 componentMass);

/** Write a "Minefield" record.
    Written on every change of a minefield.
    @param to             Receiver
    @param mineId         Minefield Id
    @param x,y            Center position
    @param owner          Minefield owner
    @param units          Number of minefield units
    @param type           Minefield type (1=web)
    @param scanReason     Reason for scan (MINE_LAID, MINE_SWEPT, MINE_SCANNED) */
void Util_Minefield(RaceType_Def to, Uns16 mineId, Uns16 x, Uns16 y, Uns16 owner, Uns32 units, Uns16 type, enum MineReason scanReason);

#endif
