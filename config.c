/**
  *  \file config.c
  *  \brief Starbase Reloaded - Configuration Handling
  */

#include <stddef.h>
#include <string.h>
#include <strings.h>        // strcasecmp according to SuS
#include "config.h"

/*
 *  Definition of config layout
 */

const char*const CONFIG_FILE_NAME = "psbplus.src";
const char*const CONFIG_FILE_SECTION = "PSTARBASE";

enum Type {
    tBoolean,
    tUns16
};

struct Definition {
    const char* Name;
    enum Type   Type : 16;
    size_t      Offset : 16;
};

#define CONFIG(type, x) { #x, t##type, offsetof(struct Config, x) }

static const struct Definition CONFIG_DEFINITION[] = {
    CONFIG(Boolean, BeamSweepMines),
    CONFIG(Uns16,   BeamSweepRange),
    CONFIG(Uns16,   BeamSweepRate),
    CONFIG(Uns16,   BeamWebSweepRate),

    CONFIG(Boolean, FighterSweepMines),
    CONFIG(Boolean, ColonialFighterOnlySweepMines),
    CONFIG(Uns16,   FtrSweepRate),
    CONFIG(Uns16,   FtrWebSweepRate),

    CONFIG(Boolean, StarbaseMCTransfer),
    CONFIG(Uns16,   MaxMCTransfer),

    CONFIG(Boolean, LayMinefields),
    CONFIG(Boolean, LayWebMinefields),

    CONFIG(Boolean, ScoopMinefields),

    CONFIG(Boolean, TransportComp),
    CONFIG(Boolean, FreighterCarryOnly),
    CONFIG(Boolean, NonCloakerCarryOnly),
    CONFIG(Uns16,   CargoSpacePerComp),
    CONFIG(Boolean, TagSpecialTransport),
};

/*
 *  Internal
 */

static struct Config* gConfig;

static Boolean AssignBoolean(Boolean* p, const char* value)
{
    if (strcasecmp(value, "yes") == 0 || strcasecmp(value, "true") == 0) {
        *p = True;
        return True;
    } else if (strcasecmp(value, "no") == 0 || strcasecmp(value, "false") == 0) {
        *p = False;
        return True;
    } else {
        return False;
    }
}

static Boolean AssignUns16(Uns16* p, const char* value)
{
    unsigned long parsed;
    char* rem;

    parsed = strtoul(value, &rem, 10);
    if ((Uns16) parsed == parsed && rem != value && rem[strspn(rem, " \t")] == '\0') {
        *p = (Uns16) parsed;
        return True;
    } else {
        return False;
    }
}

static Boolean AssignGlobalConfig(const char* lhs, char* rhs, const char* line)
{
    size_t i;
    (void) line;
    if (!lhs || !rhs) {
        return True;
    }
    for (i = 0; i < sizeof(CONFIG_DEFINITION)/sizeof(CONFIG_DEFINITION[0]); ++i) {
        const struct Definition* def = &CONFIG_DEFINITION[i];
        if (strcasecmp(lhs, def->Name) == 0) {
            switch (def->Type) {
             case tBoolean:
                return AssignBoolean((Boolean*) ((char*)gConfig + def->Offset), rhs);
             case tUns16:
                return AssignUns16((Uns16*) ((char*)gConfig + def->Offset), rhs);
            }
        }
    }
    return False;
}


/*
 *  Public Interface
 */

void Config_Init(struct Config* p)
{
    p->BeamSweepMines = True;
    p->BeamSweepRange = 5;
    p->BeamSweepRate = 4;
    p->BeamWebSweepRate = 3;

    p->FighterSweepMines = True;
    p->ColonialFighterOnlySweepMines = False;
    p->FtrSweepRate = 20;
    p->FtrWebSweepRate = 0;

    p->StarbaseMCTransfer = True;
    p->MaxMCTransfer = 3000;

    p->LayMinefields = True;
    p->LayWebMinefields = True;

    p->ScoopMinefields = True;

    p->TransportComp = True;
    p->FreighterCarryOnly = True;
    p->NonCloakerCarryOnly = True;
    p->CargoSpacePerComp = 40;
    p->TagSpecialTransport = True;
}

void Config_Load(struct Config* p)
{
    FILE* f;
    struct Config* prev;

    Config_Init(p);

    f = OpenInputFile(CONFIG_FILE_NAME, GAME_DIR_ONLY | TEXT_MODE | NO_MISSING_ERROR);
    if (f == NULL) {
        Warning("Configuration file (%s) not found, using defaults.", CONFIG_FILE_NAME);
        return;
    }

    // PDK is not reentrant, but we pretend to be.
    prev = gConfig;
    gConfig = p;
    ConfigFileReader(f, CONFIG_FILE_NAME, CONFIG_FILE_SECTION, True, AssignGlobalConfig);
    gConfig = prev;

    fclose(f);
}

void Config_Format(const struct Config* p, void func(void* state, const char* name, const char* value), void* state)
{
    size_t i;
    for (i = 0; i < sizeof(CONFIG_DEFINITION)/sizeof(CONFIG_DEFINITION[0]); ++i) {
        const struct Definition* def = &CONFIG_DEFINITION[i];
        switch (def->Type) {
         case tBoolean: {
            Boolean value = *(Boolean*) ((char*)p + def->Offset);
            func(state, def->Name, value ? "Yes" : "No");
            break;
         }
         case tUns16: {
            Uns16 value = *(Uns16*) ((char*)p + def->Offset);
            char tmp[20];
            snprintf(tmp, sizeof(tmp), "%d", value);
            func(state, def->Name, tmp);
            break;
         }
        }
    }
}
