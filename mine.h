/**
  *  \file mine.h
  *  \brief Starbase Reloaded - Mine Laying and Sweeping
  */
#ifndef MINE_H_INCLUDED
#define MINE_H_INCLUDED

struct Config;

/** Mine Sweeping/Scooping Stage.
    @param [in] c Configuration */
void DoMineSweeping(const struct Config* c);

/** Mine Laying Stage.
    @param [in] c Configuration */
void DoMineLaying(const struct Config* c);

#endif
