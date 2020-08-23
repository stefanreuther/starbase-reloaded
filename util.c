/**
  *  \file util.c
  *  \brief Starbase Reloaded - Assorted Utilities
  */

#include <string.h>
#include "util.h"

static int MatchFCode(const char* fc, const char* prefix, int limit)
{
    if (memcmp(fc, prefix, 2) == 0) {
        if (fc[2] == '0' && limit >= 10) {
            return 10;
        }
        if (fc[2] >= '1' && fc[2] <= '0' + limit) {
            return fc[2] - '0';
        }
    }
    return 0;
}

Boolean PlanetHasFCode(Uns16 planetId, const char* expected)
{
    char fc[3];
    PlanetFCode(planetId, fc);
    return memcmp(fc, expected, 3) == 0;
}

int PlanetMatchFCode(Uns16 planetId, const char* prefix, int limit)
{
    char fc[3];
    PlanetFCode(planetId, fc);
    return MatchFCode(fc, prefix, limit);
}

Boolean ShipHasFCode(Uns16 shipId, const char* expected)
{
    char fc[3];
    ShipFCode(shipId, fc);
    return memcmp(fc, expected, 3) == 0;
}

int ShipMatchFCode(Uns16 shipId, const char* prefix, int limit)
{
    char fc[3];
    ShipFCode(shipId, fc);
    return MatchFCode(fc, prefix, limit);
}

Boolean AnyNonzero(const Uns16* array, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        if (array[i] != 0) {
            return True;
        }
    }
    return False;
}
