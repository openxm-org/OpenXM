/* $OpenXM$ */

/* These are temporaly at OpenXM/doc/OpenXM-web  */
#include "cmotag.h"
#include "oxMessageTag.h"
#include "smCommand.h"

#ifndef  _OXASIR_H_
#define _OXASIR_H_ 1

void asir_ox_push_cmo(void *cmo);
int asir_ox_pop_cmo(void *cmo, int limit);
void asir_ox_push_cmd(unsigned int cmd);
void asir_ox_execute_string(char *s);
int asir_ox_peek_cmo_size(void);
void asir_ox_init(int byteorder);

#endif

