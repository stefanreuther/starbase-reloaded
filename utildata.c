/**
  *  \file utildata.c
  *  \brief Starbase Reloaded - util.data records
  */

#include "utildata.h"

#define DIM(x) (sizeof(x)/sizeof(x[0]))

/* Minefield update.
   We re-use PHost's regular minefield update.
   We use the extended edition in an attempt to not confuse very old programs
   which may have stricter requirements on records. */
static const Uns16 RECORD_MINE_UPDATE = 46;

/* Special transport reports (custom records). */
static const Uns16 RECORD_TRANSPORT_SUMMARY = 0x4080;
static const Uns16 RECORD_TRANSPORT_COMPONENT = 0x4081;


void Util_Transport_Summary(RaceType_Def to, Uns16 shipId, Uns16 totalCargo)
{
    Uns16 data[2] = {
        shipId,
        totalCargo
    };
    WordSwapShort(data, DIM(data));
    PutUtilRecordSimple(to, RECORD_TRANSPORT_SUMMARY, sizeof(data), &data);
}

void Util_Transport_Component(RaceType_Def to, Uns16 shipId, BaseTech_Def type, Uns16 slot, Uns16 numComponents, Uns16 componentMass)
{
    // Convert internal type (BaseTech_Def) into external type.
    Uns16 externalType;
    switch (type) {
     case ENGINE_TECH:  externalType = 1; break;
     case BEAM_TECH:    externalType = 2; break;
     case TORP_TECH:    externalType = 3; break;
     default: return;
    }

    // Write record
    Uns16 data[5] = {
        shipId,
        externalType,
        slot,
        numComponents,
        componentMass
    };
    WordSwapShort(data, DIM(data));
    PutUtilRecordSimple(to, RECORD_TRANSPORT_COMPONENT, sizeof(data), &data);
}

void Util_Minefield(RaceType_Def to, Uns16 mineId, Uns16 x, Uns16 y, Uns16 owner, Uns32 units, Uns16 type, enum MineReason scanReason)
{
    Uns16 data[] = {
        mineId,
        x,
        y,
        owner,
        units & 0xFFFF,     // splitting the 32-bit field into halves manually for simplicity
        units >> 16,
        type,
        0,                  // controlling planet Id; we don't set that for now.
        scanReason
    };

    WordSwapShort(data, DIM(data));
    PutUtilRecordSimple(to, RECORD_MINE_UPDATE, sizeof(data), &data);
}
