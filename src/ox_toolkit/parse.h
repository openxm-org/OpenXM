/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/parse.h,v 1.2 1999/12/22 11:26:37 ohara Exp $ */

#ifndef _PARSE_H_

#include "ox.h"

#define _PARSE_H_

/* Remarks: a token, which is less than 256, means a one character token. */

/* The followings are definitions of some tokens. */
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

#define PFLAG_ADDREV   1

int setflag_parse(int flag);
cmo *parse();
int init_parser(char *s);

typedef struct {
    char *key;
    int  tag;
    int  token;
    int  type;
} symbol; 

symbol* lookup_by_symbol(char *key);
symbol* lookup_by_token(int tok);
symbol* lookup_by_tag(int tag);
symbol* lookup(int i);

#endif /* _PARSE_H_ */
