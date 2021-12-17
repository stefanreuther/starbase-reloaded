/**
  *  \file sendconf.c
  *  \brief Starbase Reloaded - Send Configuration to Players
  */

#include "sendconf.h"
#include "config.h"
#include "language.h"
#include "message.h"
#include "util.h"

struct State {
    struct Message m;
    RaceType_Def player;
};

static void State_SendOption(void* state, const char* name, const char* value)
{
    struct State* st = state;
    if (st->m.Lines >= MAX_MESSAGE_LINES) {
        const struct Language* lang = GetLanguageForPlayer(st->player);
        Message_Add(&st->m, lang->Continuation);
        Message_Send(&st->m, st->player);
        Message_Init(&st->m);
        Message_Add(&st->m, lang->SendConfig_Continuation);
    }
    Message_Add(&st->m, "  ");
    Message_Add(&st->m, name);
    Message_Add(&st->m, " = ");
    Message_Add(&st->m, value);
    Message_Add(&st->m, "\n");
}

static void SendConfig(const struct Config* c, RaceType_Def player)
{
    struct State st;
    const struct Language* lang = GetLanguageForPlayer(player);
    st.player = player;
    Message_Init(&st.m);
    Message_Add(&st.m, lang->SendConfig_Header);
    Config_Format(c, State_SendOption, &st);
    Message_Send(&st.m, st.player);
}


void DoSendConfig(const struct Config* c)
{
    Info("    Sending configuration...");

    Uns32 gotConfig = 0;
    for (Uns16 planetId = 1; planetId <= PLANET_NR; ++planetId) {
        if (IsPlanetExist(planetId)) {
            RaceType_Def owner = PlanetOwner(planetId);
            if ((owner != 0) && (owner <= RACE_NR) && (gotConfig & (1 << owner)) == 0) {
                if (PlanetHasFCode(planetId, "con")) {
                    gotConfig |= 1 << owner;
                    Info("\t(+) Player %d: requested configuration", owner);
                    SendConfig(c, owner);
                }
            }
        }
    }
}
