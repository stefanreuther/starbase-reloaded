/**
  *  \file main.c
  *  \brief Starbase Reloaded - Main Entry Point
  *
  *  \mainpage
  *
  *  Starbase Reloaded is an add-on for VGA Planets, modeled after the
  *  StarbasePlus add-on. It provides new functionalities for starbases,
  *  controlled by friendly codes:
  *
  *  - WIRE TRANSFER of money between bases
  *
  *  - MINEFIELDS can be laid, swept and scooped by bases
  *
  *  - STARSHIP PARTS can be transported between bases using ships
  */

#include <phostpdk.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "credits.h"
#include "mine.h"
#include "sendconf.h"
#include "transport.h"

static const char*const BANNER = "Starbase Reloaded - A StarbasePlus Variant";
static const char*const LOG_FILE = "psbplus.log";

enum Mode {
    BeforeMovement,
    AfterMovement,
    DumpShips,
    DumpConfig,
    Help
};

static void PrintUsage(FILE* stream, const char* name)
{
    fprintf(stream, "%s\n\n"
            "Usage: %s MODE [GAMEDIR [ROOTDIR]]\n\n"
            "MODE is:\n"
            "  1       auxhost1\n"
            "  2       auxhost2\n"
            "  -dc     dump config\n"
            "  -ds     dump ship storage\n"
            "  --help  this message\n\n"
            "Written in 2020 by Stefan Reuther <streu@gmx.de> for PlanetsCentral\n",
            BANNER, name);
}

static int ParseMode(enum Mode* pMode, const char* name)
{
    // skip leading dashes
    while (name[0] == '-') {
        ++name;
    }

    if (strcmp(name, "1") == 0) {
        *pMode = BeforeMovement;
        return 1;
    } else if (strcmp(name, "2") == 0) {
        *pMode = AfterMovement;
        return 1;
    } else if (strcmp(name, "dc") == 0) {
        *pMode = DumpConfig;
        return 1;
    } else if (strcmp(name, "ds") == 0) {
        *pMode = DumpShips;
        return 1;
    } else if (strcmp(name, "help") == 0 || strcmp(name, "h") == 0) {
        *pMode = Help;
        return 1;
    } else {
        return 0;
    }
}

static void InitHostAction(Boolean beforeMovement, struct Config* c)
{
    InitPHOSTLib();
    gLogFile = OpenOutputFile(LOG_FILE, GAME_DIR_ONLY | TEXT_MODE | (beforeMovement ? 0 : APPEND_MODE));
    Info("Loading...");
    if (!ReadGlobalData()) {
        FreePHOSTLib();
        ErrorExit("Unable to read global data");
    }
    if (!ReadHostData()) {
        FreePHOSTLib();
        ErrorExit("Unable to read host data");
    }
    Config_Load(c);
}

static void DoneHostAction()
{
    Info("Saving...");
    if (!WriteHostData()) {
        FreePHOSTLib();
        ErrorExit("Unable to write host data");
    }
    FreePHOSTLib();
}

/*
 *  BeforeMovement mode
 */

static void DoBeforeMovement()
{
    struct Config c;
    InitHostAction(True, &c);

    Info("Starbase Reloaded - Before Movement...");
    DoMineSweeping(&c);
    DoMineLaying(&c);
    DoTrimCargo(&c);

    DoneHostAction();
}

/*
 *  AfterMovement mode
 */

static void DoAfterMovement()
{
    struct Config c;
    InitHostAction(False, &c);

    Info("Starbase Reloaded - After Movement...");
    DoComponentTransport(&c);
    DoCreditTransfer(&c);
    DoSendConfig(&c);

    DoneHostAction();
}

/*
 *  DumpConfig Mode
 */

static void DumpConfig_Show(void* state, const char* name, const char* value)
{
    (void) state;
    Info("%s = %s", name, value);
}

static void DoDumpConfig()
{
    struct Config c;
    InitPHOSTLib();
    Config_Load(&c);
    Config_Format(&c, DumpConfig_Show, NULL);
    FreePHOSTLib();
}

/*
 *  DumpShips mode
 */

static void ShowComponents(const struct TransportShip* sh, const char* pfx, BaseTech_Def type, Uns16 count)
{
    for (Uns16 i = 1; i <= count; ++i) {
        printf("%s%5d", pfx, TransportShip_Cargo(sh, type, i));
        pfx = ", ";
    }
    printf("\n");
}

static void DoDumpShips()
{
    struct TransportState st;
    TransportState_Load(&st);

    struct TransportShip* sh;
    Uns16 count = 0;
    for (Uns16 shipId = 1; (sh = TransportState_Ship(&st, shipId)) != NULL; ++shipId) {
        if (TransportShip_HasComponents(sh)) {
            printf("Ship %d:\n", shipId);
            ShowComponents(sh, "  Engines:   ", ENGINE_TECH, ENGINE_NR);
            ShowComponents(sh, "  Beams:     ", BEAM_TECH, BEAM_NR);
            ShowComponents(sh, "  Torpedoes: ", TORP_TECH, TORP_NR);
            ++count;
        }
    }
    printf("Found %d special transports.\n", count);
}

/*
 *  Main Entry Point
 */

int main(int argc, char** argv)
{
    enum Mode mode = Help;
    if (argc > 4 || argc < 2) {
        PrintUsage(stderr, argv[0]);
        return 1;
    }
    if (argc > 3) {
        gRootDirectory = argv[3];
    }
    if (argc > 2) {
        gGameDirectory = argv[2];
    }
    if (!ParseMode(&mode, argv[1])) {
        PrintUsage(stderr, argv[0]);
        return 1;
    }

    switch (mode) {
     case BeforeMovement:
        DoBeforeMovement();
        break;
     case AfterMovement:
        DoAfterMovement();
        break;
     case DumpConfig:
        DoDumpConfig();
        break;
     case DumpShips:
        DoDumpShips();
        break;
     case Help:
        PrintUsage(stdout, argv[0]);
        break;
    }
    return 0;
}
