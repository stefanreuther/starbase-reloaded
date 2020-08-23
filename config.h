/**
  *  \file config.h
  *  \brief Starbase Reloaded - Configuration Handling
  */
#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <phostpdk.h>

/** Configuration structure */
struct Config {
    Boolean BeamSweepMines;
    Uns16   BeamSweepRange;
    Uns16   BeamSweepRate;
    Uns16   BeamWebSweepRate;

    Boolean FighterSweepMines;
    Boolean ColonialFighterOnlySweepMines;
    Uns16   FtrSweepRate;
    Uns16   FtrWebSweepRate;

    Boolean StarbaseMCTransfer;
    Uns16   MaxMCTransfer;

    Boolean LayMinefields;
    Boolean LayWebMinefields;

    Boolean ScoopMinefields;

    Boolean TransportComp;
    Boolean FreighterCarryOnly;
    Boolean NonCloakerCarryOnly;
    Uns16   CargoSpacePerComp;
    Boolean TagSpecialTransport;
};

/** Initialize configuration.
    @param [out] p Configuration structure; will be set to defaults. */
void Config_Init(struct Config* p);

/** Load configuration.
    @param [out] p Configuration structure; will be set with loaded values.
    @pre PDK initialized (gGameDirectory set) */
void Config_Load(struct Config* p);

/** Format configuration.
    Calls the provided callback function for each configuration key,
    passing it the name and stringified value.
    This can be used for printing or sending messages.
    @param [in] p     Configuration
    @param [in] func  Callback function
    @param [in] state Opaque state pointer that is passed to the callback function */
void Config_Format(const struct Config* p, void func(void* state, const char* name, const char* value), void* state);

#endif
