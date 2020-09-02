/**
  *  \file util.h
  *  \brief Starbase Reloaded - Assorted Utilities
  */
#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <phostpdk.h>

/** Check for planet friendly code.
    Code is checked case-sensitively.

    @param [in] planetId Planet Id
    @param [in] expected Expected friendly code (3-character string)

    @return true on match */
Boolean PlanetHasFCode(Uns16 planetId, const char* expected);

/** Check for variable planet friendly code.
    The first two characters must match the given prefix,
    the third must be a digit (with 0 representing 10).

    @param [in] planetId Planet Id
    @param [in] prefix   Code prefix (2 characters)
    @param [in] limit    Only accept results up to this value
    @return value matching the last character of the code; 0 if no match or limit exceeded */
int PlanetMatchFCode(Uns16 planetId, const char* prefix, int limit);

/** Check for ship friendly code.
    Code is checked case-sensitively.

    @param [in] shipId   Ship Id
    @param [in] expected Expected friendly code (3-character string)
    @return true on match */
Boolean ShipHasFCode(Uns16 shipId, const char* expected);

/** Check for variable ship friendly code.
    The first two characters must match the given prefix,
    the third must be a digit (with 0 representing 10).

    @param [in] shipId   Ship Id
    @param [in] prefix   Code prefix (2 characters)
    @param [in] limit    Only accept results up to this value
    @return value matching the last character of the code; 0 if no match or limit exceeded */
int ShipMatchFCode(Uns16 shipId, const char* prefix, int limit);

/** Define a series of special friendly codes.
    This defines the friendly codes special that ShipMatchFCode(..., prefix, limit) or PlanetMatchFCode(..., prefix, limit) will match.
    @param [in] prefix   Code prefix (2 characters)
    @param [in] limit    Limit */
void DefineSpecialFCodeSeries(const char* prefix, int limit);

/** Check for nonzero value.
    @param [in] array Array to check
    @param [in] count Number of elements in array
    @return True if array contains any nonzero value */
Boolean AnyNonzero(const Uns16* array, size_t count);

/** Get effective TrueHull value.
    If MapTruehullByPlayerRace is set, we need to map race indexes through EffRace.
    (Unfortunately, PDK doesn't do that for us.)
    @param [in] player Player
    @param [in] index Index
    @return Hull number */
Uns16 EffTrueHull(RaceType_Def player, Uns16 index);

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#endif
