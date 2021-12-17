/**
  *  \file language.c
  *  \brief Starbase Reloaded - Language
  */

#include "language.h"

#include "lang_en.inc"
#include "lang_de.inc"

const struct Language* GetLanguageForPlayer(RaceType_Def player)
{
    if (player > 0 && player <= RACE_NR) {
        Language_Def lang = gPconfigInfo->Language[player];
        if (lang == LANG_German) {
            return &GERMAN;
        }
    }
    return &ENGLISH;
}
