/* $OpenXM: OpenXM/src/kan96xx/plugin/cmotag.h,v 1.5 2000/02/02 02:03:43 takayama Exp $ */
/*  cmotag.h */
/* Never Use 0 as a CMO tag. */
/* If you add new CMO_*,
   you need to make changes following the comment of cmo.c 
   and modify the two files:
   OpenXM/doc/OpenXM-web/cmotag.h
   OpenXM/doc/OpenXM-specs/*.tex
*/
#include <string.h>

#define LARGEID   0x7f000000    /* 2130706432 */
#define CMO_PRIVATE 0x7fff0000  /* 2147418112 */
#define CMO_ERROR  (LARGEID+1)
#define CMO_ERROR2 (LARGEID+2)
#define CMO_NULL   1
#define CMO_INT32  2
#define CMO_DATUM  3
#define CMO_STRING 4
#define CMO_MATHCAP 5

#define     CMO_ARRAY  16 
#define     CMO_LIST 17
#define     CMO_ATOM 18 
#define     CMO_MONOMIAL32  19
/* #define     CMO_ZZ_OLD      20  */
#define     CMO_ZZ_OLD      -20 
#define     CMO_QQ          21
#define     CMO_ZERO        22
#define     CMO_DMS          23   /* Distributed monomial system */
#define     CMO_DMS_GENERIC     24
#define     CMO_DMS_OF_N_VARIABLES 25
#define     CMO_RING_BY_NAME  26
#define     CMO_RECURSIVE_POLYNOMIAL 27
#define     CMO_LIST_R   28
#define     CMO_INT32COEFF  30
#define     CMO_DISTRIBUTED_POLYNOMIAL 31
/* #define     CMO_ZZ   32  */
#define     CMO_ZZ      20
#define     CMO_POLYNOMIAL_IN_ONE_VARIABLE 33
#define     CMO_RATIONAL 34

#define     CMO_64BIT_MACHINE_DOUBLE   40
#define     CMO_ARRAY_OF_64BIT_MACHINE_DOUBLE  41
#define     CMO_128BIT_MACHINE_DOUBLE   42
#define     CMO_ARRAY_OF_128BIT_MACHINE_DOUBLE  43

#define     CMO_BIGFLOAT    50
#define     CMO_IEEE_DOUBLE_FLOAT 51

#define     CMO_INDETERMINATE  60
#define     CMO_TREE           61
#define     CMO_LAMBDA         62    /* for function definition */


#define CMO_START_SIGNATURE    0x7fabcd03     
#define CMO_LOCAL_OBJECT       0x7fcdef03
#define CMO_LOCAL_OBJECT_ASIR  (CMO_LOCAL_OBJECT+0)
#define CMO_LOCAL_OBJECT_SM1  (CMO_LOCAL_OBJECT+1)

/* end of cmotag.h */

/* #define     CMO   1024  It is defined in stackm.h. */
