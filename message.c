/**
  *  \file message.c
  *  \brief Starbase Reloaded - Messages
  */

#include <assert.h>
#include <string.h>
#include "message.h"


void Message_Init(struct Message* m)
{
    m->Length = 0;
    m->Lines = 0;
}

void Message_AddChar(struct Message* m, char ch)
{
    if (m->Length < MAX_MESSAGE_LENGTH-1) {
        if (ch == '\n') {
            // Newline transmitted as \r (13) in VGAP
            ++m->Lines;
            m->Content[m->Length] = 13;
            ++m->Length;
        } else if ((unsigned char) ch > 255-13) {
            // These characters cannot be handled by ROT-13 encryption; discard.
            // They can possibly appear in user-provided names.
        } else {
            // Normal case
            m->Content[m->Length] = ch;
            ++m->Length;
        }
    }
}

void Message_Add(struct Message* m, const char* str)
{
    char ch;
    while ((ch = *str++) != '\0') {
        Message_AddChar(m, ch);
    }
}

void Message_Format(struct Message* m, const char* tpl, const Uns32* args, size_t numArgs)
{
    char ch;
    while ((ch = *tpl++) != '\0') {
        if (ch == '%') {
            // Parameter index
            size_t index = 0;
            while (*tpl >= '0' && *tpl <= '9') {
                index = 10*index + (*tpl++ - '0');
            }

            // Format it
            char fmt = *tpl++;
            char tmp[100];
            switch (fmt) {
             case '%':
                Message_Add(m, "%");
                break;

             case 'I':
                // 4-digit Id
                sprintf(tmp, "%04ld", (long) (index < numArgs ? args[index] : 0));
                Message_Add(m, tmp);
                break;

             case 'd':
                // Normal decimal number
                sprintf(tmp, "%ld", (long) (index < numArgs ? args[index] : 0));
                Message_Add(m, tmp);
                break;

             case 'A':
                // Adjective
                if (index < numArgs) {
                    RaceNameAdjective(args[index], tmp);
                    Message_Add(m, tmp);
                }
                break;

             case 'P':
                // Planet name
                if (index < numArgs) {
                    PlanetName(args[index], tmp);
                    Message_Add(m, tmp);
                }
                break;

             case 'S':
                // Ship name
                if (index < numArgs) {
                    ShipName(args[index], tmp);
                    Message_Add(m, tmp);
                }
                break;
            }
        } else {
            Message_AddChar(m, ch);
        }
    }
}

void Message_Send(struct Message* m, RaceType_Def to)
{
    assert(m->Length < sizeof(m->Content));
    m->Content[m->Length] = '\0';
    WriteAUXHOSTMessage(to, m->Content);
}

void Message_SendTemplate(RaceType_Def to, const char* tpl, const Uns32* args, size_t numArgs)
{
    struct Message m;
    Message_Init(&m);
    Message_Format(&m, tpl, args, numArgs);
    Message_Send(&m, to);
}

void Message_Credits_InsufficentTechToReceive(RaceType_Def owner, Uns16 planetId, Uns16 required)
{
    Uns32 args[] = { planetId, required };
    Message_SendTemplate(owner,
                         "(-p%0I)<<< Space Dock Message >>>\n"
                         "\n"
                         "FROM: %0P\n"
                         "  Starbase %0d\n"
                         "\n"
                         "We do not have sufficient tech to\n"
                         "receive credit transfers.\n"
                         "\n"
                         "Required total: %1d\n",
                         args, 2);
}

void Message_Credits_InsufficentTechToSend(RaceType_Def owner, Uns16 planetId, Uns16 required)
{
    Uns32 args[] = { planetId, required };
    Message_SendTemplate(owner,
                         "(-p%0I)<<< Space Dock Message >>>\n"
                         "\n"
                         "FROM: %0P\n"
                         "  Starbase %0d\n"
                         "\n"
                         "We do not have sufficient tech to\n"
                         "send credit transfers.\n"
                         "\n"
                         "Required total: %1d\n",
                         args, 2);
}

void Message_Credits_Transferred(RaceType_Def owner, Uns16 fromPlanet, Uns16 toPlanet, Uns16 amount)
{
    Uns32 args[] = { fromPlanet, toPlanet, amount };
    Message_SendTemplate(owner,
                         "(-z%0I)<<< Space Dock Message >>>\n"
                         "\n"
                         "FROM: %0P\n"
                         "  Starbase %0d\n"
                         "\n"
                         "We have transferred %2d mc\n"
                         "to the starbase at planet %1d,\n"
                         "%1P\n",
                         args, 3);
}

void Message_MinefieldLaid(RaceType_Def owner, Uns16 planetId, Uns16 mineId, Uns16 mineX, Uns16 mineY, Uns32 unitsLaid, Uns32 unitsNow, Uns16 radius, Boolean isWeb)
{
    //                  0         1      2      3         4         5        6
    Uns32 args[] = { planetId, mineId, mineX, mineY, unitsLaid, unitsNow, radius };

    const char* prefix = "(-l%1I)<<< Space Dock Message >>>\n"
        "\n"
        "FROM: %0P\n"
        "  Starbase %0d\n"
        "\n";

    const char* action;
    if (isWeb) {
        action =
            "We have converted our\n"
            "torpedoes into web mines\n";
    } else {
        action =
            "We have converted our\n"
            "torpedoes into deep space mines\n";
    };

    const char* result =
        "and laid them in a field centered\n"
        "at ( %2d , %3d )\n"
        " %4d mines were laid\n"
        "Mine field ID# %1d now contains\n"
        " %5d mine units and is\n"
        " %6d light years in radius\n";

    struct Message m;
    Message_Init(&m);
    Message_Format(&m, prefix, args, 7);
    Message_Format(&m, action, args, 7);
    Message_Format(&m, result, args, 7);
    Message_Send(&m, owner);
}

void Message_MinefieldSwept(RaceType_Def owner, Uns16 planetId, Uns16 mineId, Uns16 mineX, Uns16 mineY, RaceType_Def oldOwner, Uns16 oldRadius, Uns32 unitsSwept, Uns32 unitsNow, Boolean isWeb, Boolean usingFighters)
{
    //                  0         1      2      3        4           5           6         7
    Uns32 args[] = { planetId, mineId, mineX, mineY, oldOwner, 2*oldRadius, unitsSwept, unitsNow };

    // Describe situation
    const char* situation;
    if (isWeb) {
        situation =
            "(-p%0I)<<< Starbase Message >>>\n"
            "\n"
            "From: %0P\n"
            "  Starbase %0d\n\n"
            "Detected enemy mine field #%1d\n"
            "at ( %2d, %3d )\n"
            "They are %4A WEB mines!\n"
            "Minefield is %5d ly across.\n";
    } else {
        situation =
            "(-p%0I)<<< Starbase Message >>>\n"
            "\n"
            "From: %0P\n"
            "  Starbase %0d\n\n"
            "Detected enemy mine field #%1d\n"
            "at ( %2d, %3d )\n"
            "They are %4A mines!\n"
            "Minefield is %5d ly across.\n";
    }

    // Describe action taken
    const char* action;
    if (usingFighters) {
        action =
            "Using fighters to sweep mines.\n"
            "%6d mines destroyed.\n"
            "%7d units remain.\n";
    } else {
        action =
            "Using beam weapons to sweep mines.\n"
            "%6d mines destroyed.\n"
            "%7d units remain.\n";
    }

    struct Message m;
    Message_Init(&m);
    Message_Format(&m, situation, args, 8);
    Message_Format(&m, action, args, 8);
    Message_Send(&m, owner);
}

void Message_MinefieldScooped(RaceType_Def owner, Uns16 planetId, Uns16 mineId, Uns16 mineX, Uns16 mineY, Uns16 oldRadius, Uns16 torpsMade, Boolean isWeb)
{
    //                  0         1      2      3         4           5
    Uns32 args[] = { planetId, mineId, mineX, mineY, 2*oldRadius, torpsMade };

    // Describe situation
    const char* situation;
    if (isWeb) {
        situation =
            "(-p%0I)<<< Starbase Message >>>\n"
            "\n"
            "From: %0P\n"
            "  Starbase %0d\n\n"
            "Detected our WEB mine field #%1d\n"
            "at ( %2d, %3d )\n"
            "Minefield is %4d ly across.\n";
    } else {
        situation =
            "(-p%0I)<<< Starbase Message >>>\n"
            "\n"
            "From: %0P\n"
            "  Starbase %0d\n\n"
            "Detected our mine field #%1d\n"
            "at ( %2d, %3d )\n"
            "Minefield is %4d ly across.\n";
    }

    // Describe action taken
    const char* action =
        "Gathering mines.\n"
        "We made %5d torpedoes.\n";

    struct Message m;
    Message_Init(&m);
    Message_Format(&m, situation, args, 6);
    Message_Format(&m, action, args, 6);
    Message_Send(&m, owner);
}

void Message_Transport_LoadNotPermitted(RaceType_Def to, Uns16 shipId)
{
    Uns32 args[] = { shipId };
    Message_SendTemplate(to,
                         "(-s%0I)<<< Fleet Message >>>\n"
                         "\n"
                         "FROM: %0S\n"
                         "  Ship %0d\n"
                         "\n"
                         "We were ordered to load starship parts.\n"
                         "Unfortunately, our ship is not able\n"
                         "to carry starship parts.\n",
                         args, 1);
}

void Message_Transport_LoadNoParts(RaceType_Def to, Uns16 shipId, Uns16 planetId)
{
    Uns32 args[] = { shipId, planetId };
    Message_SendTemplate(to,
                         "(-s%0I)<<< Fleet Message >>>\n"
                         "\n"
                         "FROM: %0S\n"
                         "  Ship %0d\n"
                         "\n"
                         "We were ordered to load starship parts.\n"
                         "Unfortunately, the starbase #%1d\n"
                         "at %1P\n"
                         "did not have any matching parts\n"
                         "in storage.\n",
                         args, 2);
}

void Message_Transport_LoadConflictingParts(RaceType_Def to, Uns16 shipId)
{
    Uns32 args[] = { shipId };
    Message_SendTemplate(to,
                         "(-s%0I)<<< Fleet Message >>>\n"
                         "\n"
                         "FROM: %0S\n"
                         "  Ship %0d\n"
                         "\n"
                         "We were ordered to load starship parts.\n"
                         "Unfortunately, we already parts of\n"
                         "a different type aboard and cannot\n"
                         "load another type.\n",
                         args, 1);
}

void Message_Transport_LoadNoSpace(RaceType_Def to, Uns16 shipId)
{
    Uns32 args[] = { shipId };
    Message_SendTemplate(to,
                         "(-s%0I)<<< Fleet Message >>>\n"
                         "\n"
                         "FROM: %0S\n"
                         "  Ship %0d\n"
                         "\n"
                         "We were ordered to load starship parts.\n"
                         "Unfortunately, we didn't have any space\n"
                         "in our cargo room to take the parts.\n",
                         args, 1);
}

void Message_Transport_LoadSuccess(RaceType_Def to, Uns16 shipId, Uns16 planetId, Uns16 numComponents)
{
    Uns32 args[] = { shipId, planetId, numComponents };
    Message_SendTemplate(to,
                         "(-s%0I)<<< Fleet Message >>>\n"
                         "\n"
                         "FROM: %0S\n"
                         "  Ship %0d\n"
                         "\n"
                         "We have successfully loaded %2d parts\n"
                         "from starbase #%1d\n"
                         "at %1P\n",
                         args, 3);
}

void Message_Transport_UnloadNoParts(RaceType_Def to, Uns16 shipId)
{
    Uns32 args[] = { shipId };
    Message_SendTemplate(to,
                         "(-s%0I)<<< Fleet Message >>>\n"
                         "\n"
                         "FROM: %0S\n"
                         "  Ship %0d\n"
                         "\n"
                         "We were ordered to unload starship\n"
                         "parts, but did not have anything\n"
                         "to unload.",
                         args, 1);
}

void Message_Transport_UnloadSuccess(RaceType_Def to, Uns16 shipId, Uns16 planetId, Uns32 numComponents)
{
    Uns32 args[] = { shipId, planetId, numComponents };
    Message_SendTemplate(to,
                         "(-s%0I)<<< Fleet Message >>>\n"
                         "\n"
                         "FROM: %0S\n"
                         "  Ship %0d\n"
                         "\n"
                         "We have unloaded %2d parts\n"
                         "to starbase #%1d\n"
                         "at %1P",
                         args, 3);
}

void Message_Transport_TrimmedComponents(RaceType_Def to, Uns16 shipId, Uns16 droppedComponents, Uns16 droppedMass)
{
    Uns32 args[] = { shipId, droppedComponents, droppedMass };
    Message_SendTemplate(to,
                         "(-s%0I)<<< Fleet Message >>>\n"
                         "\n"
                         "FROM: %0S\n"
                         "  Ship %0d\n"
                         "\n"
                         "We are overloaded!\n"
                         "We had to jettison %1d parts\n"
                         "weighing %2d kt total!\n",
                         args, 3);
}

void Message_Transport_TrimmedCargo(RaceType_Def to, Uns16 shipId, Uns16 droppedMass)
{
    Uns32 args[] = { shipId, droppedMass };
    Message_SendTemplate(to,
                         "(-s%0I)<<< Fleet Message >>>\n"
                         "\n"
                         "FROM: %0S\n"
                         "  Ship %0d\n"
                         "\n"
                         "We are overloaded!\n"
                         "We had to jettison %1d kt cargo!\n",
                         args, 2);
}
