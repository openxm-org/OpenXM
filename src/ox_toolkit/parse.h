/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/parse.h,v 1.3 2000/01/17 19:55:56 ohara Exp $ */

#ifndef _PARSE_H_

#define _PARSE_H_

struct symbol {
    char *key;
    int  tag;
    int  token;
    int  type;
};

#include "ox_toolkit.h"

/* Remarks: a token, which is less than 256, means a one character token. */

/* The following are definitions of some tokens. */
#define T_DIGIT         257
#define T_STRING        258
#define T_MAGIC         2048
#define TOKEN(x)        (T_MAGIC + (x))
#define SM              SM_popCMO

/* Macros for distinction of kinds of tokens. */
#define MIN_T_CMO     TOKEN(0)
#define MAX_T_CMO     TOKEN(256)
#define MIN_T_OX      TOKEN(512)
#define MAX_T_OX      TOKEN(600)

#define IS_CMO   1
#define IS_SM    2
#define IS_OX    3

#endif /* _PARSE_H_ */
