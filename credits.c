/**
  *  \file credits.c
  *  \brief Starbase Reloaded - Credit Transfer
  */

#include <phostpdk.h>
#include "credits.h"
#include "config.h"
#include "util.h"
#include "message.h"

const int MIN_TOTAL_TECH = 20; /* FIXME: put in config */

struct State {
    Uns16 ReceivingBase[RACE_NR+1];
};

static Boolean BaseCanTransfer(const struct Config* c, Uns16 baseId)
{
    (void) c;

    return ((BaseTech(baseId, HULL_TECH) + BaseTech(baseId, ENGINE_TECH) + BaseTech(baseId, BEAM_TECH) + BaseTech(baseId, TORP_TECH))
            >= MIN_TOTAL_TECH);
}

static void Credit_Init(struct State* p)
{
    int i;
    for (i = 0; i < RACE_NR+1; ++i) {
        p->ReceivingBase[i] = 0;
    }
}

static void Credit_FindReceivers(struct State* p, const struct Config* c)
{
    int i;
    for (i = 1; i <= PLANET_NR; ++i) {
        if (IsBaseExist(i) && PlanetHasFCode(i, "RMT")) {
            RaceType_Def owner = BaseOwner(i);
            if (owner > 0 && owner <= RACE_NR) {
                if (BaseCanTransfer(c, i)) {
                    if (p->ReceivingBase[owner] == 0) {
                        Info("\t(+) Base %d, player %d: receives", i, owner);
                        p->ReceivingBase[owner] = i;
                    } else {
                        Info("\t(-) Base %d, player %d: duplicate receiver", i, owner);
                    }
                } else {
                    Info("\t(-) Base %d, player %d: insufficient tech to receive", i, owner);
                    Message_Credits_InsufficentTechToReceive(owner, i, MIN_TOTAL_TECH);
                }
            }
        }
    }
}

static void Credit_ProcessSenders(struct State* p, const struct Config* c)
{
    int i, amount;
    for (i = 1; i <= PLANET_NR; ++i) {
        if (IsBaseExist(i) && (amount = PlanetMatchFCode(i, "TM", 5)) != 0) {
            RaceType_Def owner = BaseOwner(i);
            if (owner > 0 && owner <= RACE_NR) {
                if (BaseCanTransfer(c, i)) {
                    Uns16 to = p->ReceivingBase[owner];
                    if (to == 0) {
                        Info("\t(-) Base %d, player %d: no receiver for transmission", i, owner);
                    } else {
                        // Determine amount to transfer
                        Uns32 want = 1000 * amount;
                        Uns32 allowed = c->MaxMCTransfer;
                        Uns32 have = PlanetCargo(i, CREDITS);
                        Uns32 toTransfer = MIN(have, MIN(want, allowed));
                        Info("\t(+) Base %d, player %d: transfers %d mc to %d", i, owner, (int) toTransfer, to);

                        PutPlanetCargo(to, CREDITS, PlanetCargo(to, CREDITS) + toTransfer);
                        PutPlanetCargo(i,  CREDITS, PlanetCargo(i,  CREDITS) - toTransfer);

                        Message_Credits_Transferred(owner, i, to, toTransfer);
                    }
                } else {
                    Info("\t(-) Base %d, player %d: insufficient tech to transfer", i, owner);
                    Message_Credits_InsufficentTechToSend(owner, i, MIN_TOTAL_TECH);
                }
            }
        }
    }
}

void DoCreditTransfer(const struct Config* c)
{
    if (!c->StarbaseMCTransfer || c->MaxMCTransfer == 0) {
        Info("    Credit transfers disabled.");
    } else {
        struct State st;
        Info("    Credit transfers...");
        Credit_Init(&st);
        Credit_FindReceivers(&st, c);
        Credit_ProcessSenders(&st, c);
    }
}
