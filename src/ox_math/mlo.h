#ifndef _MLO_H_

#define _MLO_H_

#include "ox.h"

/* Mathematica でのみ使用される Local Object の定義 */
typedef struct {
    int tag;
    int length;
    cell head[1];
    char *function;
} mlo_function;

cmo *receive_mlo();


#endif
