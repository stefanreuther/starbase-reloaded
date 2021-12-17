/**
  *  \file language.h
  *  \brief Starbase Reloaded - Language
  */
#ifndef LANGUAGE_H_INCLUDED
#define LANGUAGE_H_INCLUDED

#include <phostpdk.h>

/** Definition for a single language.
    Each string is (part of) a message template. */
struct Language {
    // Credits actions
    const char* Message_Credits_InsufficentTechToReceive;
    const char* Message_Credits_InsufficentTechToSend;
    const char* Message_Credits_Transferred;

    // Minefield laid
    const char* Message_MinefieldLaid_Prefix;
    const char* Message_MinefieldLaid_Web;
    const char* Message_MinefieldLaid_Normal;
    const char* Message_MinefieldLaid_Suffix;

    // Minefield swept
    const char* Message_MinefieldSwept_Web;
    const char* Message_MinefieldSwept_Normal;
    const char* Message_MinefieldSwept_Fighters;
    const char* Message_MinefieldSwept_Beams;

    // Minefield scooped
    const char* Message_MinefieldScooped_Web;
    const char* Message_MinefieldScooped_Normal;
    const char* Message_MinefieldScooped_Action;

    // Load parts
    const char* Message_Transport_LoadNotPermitted;
    const char* Message_Transport_LoadNoParts;
    const char* Message_Transport_LoadConflictingParts;
    const char* Message_Transport_LoadNoSpace;
    const char* Message_Transport_LoadSuccess;
    const char* Message_Transport_UnloadNoParts;
    const char* Message_Transport_UnloadSuccess;
    const char* Message_Transport_TrimmedComponents;
    const char* Message_Transport_TrimmedCargo;

    // Configuration
    const char* SendConfig_Header;
    const char* SendConfig_Continuation;

    // Ship cargo
    const char* ReportShip_Header;
    const char* ReportShip_Continuation;

    const char* Continuation;
};

/** Get language for a player.
    Will never return null; if player has no (recognized) language, returns English.
    \param player Player */
const struct Language* GetLanguageForPlayer(RaceType_Def player);

#endif
