/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/parse.h,v 1.4 1999/11/06 21:39:37 ohara Exp $ */

#ifndef _PARSE_H_

#include "ox.h"

#define _PARSE_H_

/* トークンの定義 */
/* 256 より小さいものは、一文字だけのトークンである */

#define T_INTEGER       257
#define T_STRING        258
#define T_MAGIC         2048
#define TOKEN(x)        (T_MAGIC + (x))
#define SM              SM_popCMO

/* トークンの種類を判定するためのマクロ */
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

int setgetc(int (*foo)());
int resetgetc();
int mygetc();
int setmode_mygetc(char *s, int len);

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
