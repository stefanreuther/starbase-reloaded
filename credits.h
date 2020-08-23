/**
  *  \file credits.h
  *  \brief Starbase Reloaded - Credit Transfer
  */
#ifndef CREDITS_H_INCLUDED
#define CREDITS_H_INCLUDED

struct Config;

/** Credit transfer stage (TMx, RMT).
    @param [in] Configuration */
void DoCreditTransfer(const struct Config* c);

#endif
