/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */
/* $Id$ */

#ifndef _PARSE_H_

#include "ox.h"

#define _PARSE_H_

/* トークンを定義する */
/* 2048 より小さいのは特別なもの */

#define T_INTEGER       257
#define T_STRING        258

#define T_MAGIC         2048

#define T_CMO_NULL      (T_MAGIC + CMO_NULL)
#define T_CMO_INT32     (T_MAGIC + CMO_INT32)
#define T_CMO_STRING    (T_MAGIC + CMO_STRING)
#define T_CMO_LIST      (T_MAGIC + CMO_LIST)
#define T_CMO_ZZ        (T_MAGIC + CMO_ZZ)
#define T_CMO_ZERO      (T_MAGIC + CMO_ZERO)
#define T_CMO_DATUM     (T_MAGIC + CMO_DATUM)
#define T_CMO_ERROR2    (T_MAGIC + CMO_ERROR2)

#define T_SM_popCMO     (T_MAGIC + SM_popCMO)
#define T_SM_popString  (T_MAGIC + SM_popString)
#define T_SM_mathcap    (T_MAGIC + SM_mathcap)
#define T_SM_pops       (T_MAGIC + SM_pops)
#define T_SM_executeStringByLocalParser    (T_MAGIC + SM_executeStringByLocalParser)
#define T_SM_executeFunction    (T_MAGIC + SM_executeFunction)
#define T_SM_setMathcap         (T_MAGIC + SM_setMathcap)
#define T_SM_control_kill       (T_MAGIC + SM_control_kill)
#define T_SM_control_reset_connection    (T_MAGIC + SM_control_reset_connection)

#define T_OX_COMMAND    (T_MAGIC + OX_COMMAND)
#define T_OX_DATA       (T_MAGIC + OX_DATA)

/* lexical analyzer で用いる属性値の共用体の定義 */
typedef union{
    int   d;
    char* sym;
} lex_value_t;

int lex();
cmo *parse();

int cmo_addrev;  /* CMO の省略記法を許すか否かのフラグ */

int setgetc(int (*foo)());
int resetgetc();
int mygetc();
int setmode_mygetc(char *s, int len);

#endif /* _PARSE_H_ */
