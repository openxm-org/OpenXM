/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/cmo.c,v 1.2 2000/10/12 15:50:10 ohara Exp $ */

/* 
   This module includes functions for sending/receiveng CMO's.
   Some commnets is written in Japanese by the EUC-JP coded 
   character set.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "ox_toolkit.h"
#include "parse.h"

static cell*        new_cell();
static char*        new_string_set_cmo_null();
static char*        new_string_set_cmo_int32(int integer);
static char*        new_string_set_cmo_list(cmo_list *c);
static char*        new_string_set_cmo_zz(cmo_zz *c);

/* functions for a cmo_list */
static cell* new_cell(cmo *ob)
{
    cell* h = malloc(sizeof(cell));
    h->next = NULL;
    h->prev = NULL;
    h->cmo  = ob;
    return h;
}

cell* list_next(cell *el)
{
    return el->next;
}

cell* list_first(cmo_list *this)
{
    return this->head->next;
}

cmo* list_first_cmo(cmo_list *this)
{
    return list_first(this)->cmo;
}

int list_endof(cmo_list *this, cell *el)
{
    return (this->head == el);
}

/* (prev, head) => (prev, new, head) */
static void list_cons(cell *head, cell *new)
{
    cell* prev = head->prev;
    new->prev  = prev;
    head->prev = new;
    prev->next = new;
    new->next  = head;
}

cmo_list *list_append(cmo_list* this, cmo* ob)
{
    list_cons(this->head, new_cell(ob));
    this->length++;
    return this;
}

/* call as list_appendl(List, ob1, ob2, ob3, NULL) */
cmo_list *list_appendl(cmo_list* this, ...)
{
    cmo *ob;
    va_list ap;
    va_start(ap, this);
	if (this == NULL) {
		this = new_cmo_list();
	}
    while((ob = va_arg(ap, cmo *)) != NULL) {
        list_append(this, ob);
    }
    va_end(ap);
    return this;
}

int list_length(cmo_list* this)
{
    return this->length;
}

cmo *list_nth(cmo_list* this, int n)
{
    cell* el;
    if(list_length(this) > n) {
        el = list_first(this);
        while(n-- > 0) {
            el = list_next(el);
        }
        return el->cmo;
    }
    return NULL;
}

/* for GNU mpz */
void resize_mpz(mpz_ptr mpz, int size)
{
    _mpz_realloc(mpz, abs(size));
    mpz->_mp_size = size;
}

/* functions named new_cmo_*. */
cmo_null* new_cmo_null()
{
    cmo_null* m = malloc(sizeof(cmo_null));
    m->tag = CMO_NULL;
    return m;
}

cmo_int32* new_cmo_int32(int i)
{
    cmo_int32* c;
    c = malloc(sizeof(cmo_int32));
    c->tag     = CMO_INT32;
    c->i = i;
    return c;
}

cmo_string* new_cmo_string(char* s)
{
    cmo_string* c = malloc(sizeof(cmo_string));
    c->tag = CMO_STRING;
    if (s != NULL) {
        c->s = malloc(strlen(s)+1);
        strcpy(c->s, s);
    }else {
        c->s = NULL;
    }
    return c;
}

cmo_mathcap* new_cmo_mathcap(cmo* ob)
{
    cmo_mathcap* c = malloc(sizeof(cmo_mathcap));
    c->tag = CMO_MATHCAP;
    c->ob  = ob;
    return c;
}

cmo_list* new_cmo_list()
{
    cmo_list* c = malloc(sizeof(cmo_list));
    c->tag    = CMO_LIST;
    c->length = 0;
    c->head->next = c->head;
    c->head->prev = c->head;
    return c;
}

cmo_monomial32* new_cmo_monomial32()
{
    cmo_monomial32* c = malloc(sizeof(cmo_monomial32));
    c->tag  = CMO_MONOMIAL32;
    return c;
}

cmo_monomial32* new_cmo_monomial32_size(int size)
{
    cmo_monomial32* c = new_cmo_monomial32();
    if (size>0) {
        c->length = size;
        c->exps = malloc(sizeof(int)*size);
    }
    return c;
}

cmo_zz* new_cmo_zz()
{
    cmo_zz* c = malloc(sizeof(cmo_zz));
    c->tag  = CMO_ZZ;
    mpz_init(c->mpz);
    return c;
}

cmo_zz* new_cmo_zz_noinit()
{
    cmo_zz* c = malloc(sizeof(cmo_zz));
    c->tag  = CMO_ZZ;
    return c;
}

cmo_zz* new_cmo_zz_set_si(int i)
{
    cmo_zz* c = new_cmo_zz();
    mpz_set_si(c->mpz, i);
    return c;
}

cmo_zz* new_cmo_zz_set_mpz(mpz_ptr z)
{
    cmo_zz* c = new_cmo_zz();
    mpz_set(c->mpz, z);
    return c;
}

cmo_zz *new_cmo_zz_set_string(char *s)
{
    cmo_zz* c = new_cmo_zz_noinit();
    mpz_init_set_str(c->mpz, s, 10);
    return c;
}

cmo_zz* new_cmo_zz_size(int size)
{
    cmo_zz* c = new_cmo_zz();
    resize_mpz(c->mpz, size);
    return c;
}

cmo_zero* new_cmo_zero()
{
    cmo_zero* m = malloc(sizeof(cmo_zero));
    m->tag = CMO_ZERO;
    return m;
}

cmo_dms_generic* new_cmo_dms_generic()
{
    cmo_dms_generic* m = malloc(sizeof(cmo_dms_generic));
    m->tag = CMO_DMS_GENERIC;
    return m;
}

cmo_ring_by_name* new_cmo_ring_by_name(cmo* ob)
{
    cmo_ring_by_name* c = malloc(sizeof(cmo_ring_by_name));
    c->tag = CMO_RING_BY_NAME;
    c->ob  = ob;
    return c;
}

cmo_indeterminate* new_cmo_indeterminate(cmo* ob)
{
    cmo_indeterminate* c = malloc(sizeof(cmo_indeterminate));
    c->tag = CMO_INDETERMINATE;
    c->ob  = ob;
    return c;
}

cmo_distributed_polynomial* new_cmo_distributed_polynomial()
{
    cmo_distributed_polynomial* c = malloc(sizeof(cmo_distributed_polynomial));
    c->tag     = CMO_DISTRIBUTED_POLYNOMIAL;
    c->length  = 0;
    c->head->next = c->head;
    c->head->prev = c->head;
    c->ringdef = NULL;
    return c;
}

cmo_error2* new_cmo_error2(cmo* ob)
{
    cmo_error2* c = malloc(sizeof(cmo_error2));
    c->tag = CMO_ERROR2;
    c->ob  = ob;
    return c;
}


/* Following functions translate cmo's to (asciiz) strings. */
static char *new_string_set_cmo_zz(cmo_zz *c)
{
    return mpz_get_str(NULL, 10, c->mpz);
}

static char *new_string_set_cmo_null()
{
    static char* null_string = "";
    return null_string;
}

static char *new_string_set_cmo_int32(int integer)
{
    char buff[1024];
    char *s;

    sprintf(buff, "%d", integer);
    s = malloc(strlen(buff)+1);
    strcpy(s, buff);

    return s;
}

static char *new_string_set_cmo_list(cmo_list *m)
{
    char *s;
    int i;
    int size = 0;
    int len = list_length(m);
    char **sp = malloc(len*sizeof(cmo *));

    cell* cp = list_first(m);
    for(i = 0; i < len; i++) {
        sp[i] = new_string_set_cmo(cp->cmo);
        size += strlen(sp[i]) + 3;
        cp = list_next(cp);
    }
    s = malloc(size+2);
    strcpy(s, "[ ");
    for(i = 0; i < len - 1; i++) {
        strcat(s, sp[i]);
        strcat(s, " , ");
    }
    strcat(s, sp[len-1]);
    strcat(s, " ]");
    free(sp);
    return s;
}

char *new_string_set_cmo(cmo *m)
{
    symbol_t symp;
    switch(m->tag) {
    case CMO_ZZ:
        return new_string_set_cmo_zz((cmo_zz *)m);
    case CMO_INT32:
        return new_string_set_cmo_int32(((cmo_int32 *)m)->i);
    case CMO_STRING:
        return ((cmo_string *)m)->s;
    case CMO_NULL:
        return new_string_set_cmo_null();
    case CMO_LIST:
        return new_string_set_cmo_list((cmo_list *)m);
    default:
#ifdef DEBUG
        symp = lookup_by_tag(m->tag);
        fprintf(stderr, "I do not know how to convert %s to a string.\n", symp->key);
#endif
        /* yet not implemented. */
        return NULL;
    }
}

