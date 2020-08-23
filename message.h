/**
  *  \file message.h
  *  \brief Starbase Reloaded - Messages
  */
#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <phostpdk.h>

/** Maxium message length in bytes. */
#define MAX_MESSAGE_LENGTH 600

/** Estimated maximum number of message lines.
    This is not a hard limit; code generating variable-length messages
    uses this as a guideline when to start a new message. */
#define MAX_MESSAGE_LINES 17


/*
 *  Low-Level Functions
 */

/** Message building state.
    Tracks content and size for a message. */
struct Message {
    size_t Length;                        /**< Number of characters so far. */
    size_t Lines;                         /**< Number of lines so far. */
    char Content[MAX_MESSAGE_LENGTH];     /**< Character buffer. */
};

/** Initialize a message.
    @param [out] m Structure to initialize */
void Message_Init(struct Message* m);

/** Add character to e message.
    Counts lines and characters.
    If the character limit is exceeded, excess characters are discarded.
    @param [in,out] m  Message
    @param [in]     ch Character to add */
void Message_AddChar(struct Message* m, char ch);

/** Add string to a message.
    Also see Message_AddChar.
    @param [in,out] m   Message
    @param [in]     str String to add */
void Message_Add(struct Message* m, const char* str);

/** Add formatted text to a message.
    Processes a message template, interpolating placeholders.
    Each placeholder has the format `%<index><type>`,
    where `<index>` is the 0-based index into @c args,
    and `<type>` is one of the following:
    - '%' single percent sign
    - 'I' integer formatted with leading zeroes (%04d)
    - 'd' integer
    - 'A' race name adjective
    - 'P' planet name
    - 'S' ship name

    Also see Message_AddChar.

    @param [in,out] m       Message
    @param [in]     tpl     Message template
    @param [in]     args    Parameters
    @param [in]     numArgs Number of parameters (number of elements in args) */
void Message_Format(struct Message* m, const char* tpl, const Uns32* args, size_t numArgs);

/** Send message.
    @param [in] in Message
    @param [in] to Player to receive the message */
void Message_Send(struct Message* m, RaceType_Def to);


/*
 *  Higher-Level Functions
 */

/** Send message with template.
    This combines the Message_Init, Message_Format, Message_Send functions in one call.
    @param [in] to      Receiver
    @param [in] tpl     Template; see Message_Format
    @param [in] args    Parameters; ses Message_Format
    @param [in] numArgs Number of parameters; see Message_Format */
void Message_SendTemplate(RaceType_Def to, const char* tpl, const Uns32* args, size_t numArgs);

/*
 *  Canned Messages
 */

void Message_Credits_InsufficentTechToReceive(RaceType_Def owner, Uns16 planetId, Uns16 required);
void Message_Credits_InsufficentTechToSend(RaceType_Def owner, Uns16 planetId, Uns16 required);
void Message_Credits_Transferred(RaceType_Def owner, Uns16 fromPlanet, Uns16 toPlanet, Uns16 amount);
void Message_MinefieldLaid(RaceType_Def owner, Uns16 planetId, Uns16 mineId, Uns16 mineX, Uns16 mineY, Uns32 unitsLaid, Uns32 unitsNow, Uns16 radius, Boolean isWeb);
void Message_MinefieldSwept(RaceType_Def owner, Uns16 planetId, Uns16 mineId, Uns16 mineX, Uns16 mineY, RaceType_Def oldOwner, Uns16 oldRadius, Uns32 unitsSwept, Uns32 unitsNow, Boolean isWeb, Boolean usingFighters);
void Message_MinefieldScooped(RaceType_Def owner, Uns16 planetId, Uns16 mineId, Uns16 mineX, Uns16 mineY, Uns16 oldRadius, Uns16 torpsMade, Boolean isWeb);
void Message_Transport_LoadNotPermitted(RaceType_Def to, Uns16 shipId);
void Message_Transport_LoadNoParts(RaceType_Def to, Uns16 shipId, Uns16 planetId);
void Message_Transport_LoadConflictingParts(RaceType_Def to, Uns16 shipId);
void Message_Transport_LoadNoSpace(RaceType_Def to, Uns16 shipId);
void Message_Transport_LoadSuccess(RaceType_Def to, Uns16 shipId, Uns16 planetId, Uns16 numComponents);
void Message_Transport_UnloadNoParts(RaceType_Def to, Uns16 shipId);
void Message_Transport_UnloadSuccess(RaceType_Def to, Uns16 shipId, Uns16 planetId, Uns32 numComponents);
void Message_Transport_TrimmedComponents(RaceType_Def to, Uns16 shipId, Uns16 droppedComponents, Uns16 droppedMass);
void Message_Transport_TrimmedCargo(RaceType_Def to, Uns16 shipId, Uns16 droppedMass);

#endif
