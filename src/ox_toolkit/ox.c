/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/ox.c,v 1.15 2000/10/12 15:53:25 ohara Exp $ */

/* 
   This module includes functions for sending/receiveng CMO's.
   Some commnets is written in Japanese by the EUC-JP coded 
   character set.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <time.h>

#include "mysocket.h"
#include "ox_toolkit.h"
#include "parse.h"

/* CMO_xxx の値の順にならべること(デバッグのため) */
static cmo_null*         receive_cmo_null(OXFILE *oxfp);
static cmo_int32*        receive_cmo_int32(OXFILE *oxfp);
static cmo_string*       receive_cmo_string(OXFILE *oxfp);
static cmo_mathcap*      receive_cmo_mathcap(OXFILE *oxfp);
static cmo_list*         receive_cmo_list(OXFILE *oxfp);
static cmo_monomial32*   receive_cmo_monomial32(OXFILE *oxfp);
static cmo_zz*           receive_cmo_zz(OXFILE *oxfp);
static cmo_zero*         receive_cmo_zero(OXFILE *oxfp);
static cmo_dms_generic*  receive_cmo_dms_generic(OXFILE *oxfp);
static cmo_ring_by_name* receive_cmo_ring_by_name(OXFILE *oxfp);
static cmo_distributed_polynomial* receive_cmo_distributed_polynomial(OXFILE *oxfp);

static cmo_error2*       receive_cmo_error2(OXFILE *oxfp);
static void              receive_mpz(OXFILE *oxfp, mpz_ptr mpz);

static int          send_cmo_null(OXFILE *oxfp, cmo_null* c);
static int          send_cmo_int32(OXFILE *oxfp, cmo_int32* m);
static int          send_cmo_string(OXFILE *oxfp, cmo_string* m);
static int          send_cmo_mathcap(OXFILE *oxfp, cmo_mathcap* c);
static int          send_cmo_list(OXFILE *oxfp, cmo_list* c);
static int          send_cmo_monomial32(OXFILE *oxfp, cmo_monomial32* c);
static int          send_cmo_zz(OXFILE *oxfp, cmo_zz* c);
static int          send_cmo_error2(OXFILE *oxfp, cmo_error2* c);
static int          send_mpz(OXFILE *oxfp, mpz_ptr mpz);
static int          send_cmo_distributed_polynomial(OXFILE *oxfp, cmo_distributed_polynomial* c);

/* hook functions. (yet not implemented) */
static hook_t hook_before_send_cmo = NULL;
static hook_t hook_after_send_cmo  = NULL;

int add_hook_before_send_cmo(hook_t func)
{
    hook_before_send_cmo = func;
    return 0;
}

int add_hook_after_send_cmo(hook_t func)
{
    hook_after_send_cmo = func;
    return 0;
}

static cmo *call_hook_before_send_cmo(OXFILE *oxfp, cmo *c)
{
    if (hook_before_send_cmo != NULL) {
        return hook_before_send_cmo(oxfp, c);
    }
    return c;
}

static cmo *call_hook_after_send_cmo(OXFILE *oxfp, cmo *c)
{
    if (hook_after_send_cmo != NULL) {
        return hook_after_send_cmo(oxfp, c);
    }
    return c;
}

/* Handling an error. */
static int current_received_serial = 0;

/* If an error object be needed, then a server call the following function. */
cmo_error2* make_error_object(int err_code, cmo *ob)
{
    cmo_list* li = new_cmo_list();
    list_append(li, (cmo *)new_cmo_int32(current_received_serial));
    list_append(li, (cmo *)new_cmo_int32(err_code));
    list_append(li, ob);
    /* 他の情報を加えるならココ */
    return new_cmo_error2((cmo *)li);
}

/* getting a next serial number. */
int next_serial(OXFILE *oxfp)
{
    return oxfp->serial_number++;
}

/* sending an object of int32 type. (not equal to cmo_int32 type)  */
int send_int32(OXFILE *oxfp, int int32)
{
    return oxfp->send_int32(oxfp, int32);
}

/* receiving an object of int32 type. (not equal to cmo_int32 type)  */
int receive_int32(OXFILE *oxfp)
{
    return oxfp->receive_int32(oxfp);
}

/* receiving an (OX_tag, serial number)  */
int receive_ox_tag(OXFILE *oxfp)
{
    int tag = receive_int32(oxfp);
    oxfp->received_serial_number = receive_int32(oxfp);
    return tag;
}

/* sending an (OX_tag, serial number)  */
int send_ox_tag(OXFILE *oxfp, int tag)
{
    send_int32(oxfp, tag);
    return send_int32(oxfp, next_serial(oxfp));
}

/* functions named receive_cmo_*. */
static cmo_null* receive_cmo_null(OXFILE *oxfp)
{
    return new_cmo_null();
}

static cmo_int32* receive_cmo_int32(OXFILE *oxfp)
{
    int i = receive_int32(oxfp);
    return new_cmo_int32(i);
}

static cmo_string* receive_cmo_string(OXFILE *oxfp)
{
    int len = receive_int32(oxfp);
    char* s = malloc(len+1);
    memset(s, '\0', len+1);
    if (len > 0) {
        oxf_read(s, 1, len, oxfp);
    }
    return new_cmo_string(s);
}

static cmo_mathcap* receive_cmo_mathcap(OXFILE *oxfp)
{
    cmo* ob = receive_cmo(oxfp);
    return new_cmo_mathcap(ob);
}

static cmo_list* receive_cmo_list(OXFILE *oxfp)
{
    cmo* ob;
    cmo_list* c = new_cmo_list();
    int len = receive_int32(oxfp);

    while (len>0) {
        ob = receive_cmo(oxfp);
        list_append(c, ob);
        len--;
    }
    return c;
}

static cmo_monomial32* receive_cmo_monomial32(OXFILE *oxfp)
{
    int i;
    int len = receive_int32(oxfp);
    cmo_monomial32* c = new_cmo_monomial32(len);

    for(i=0; i<len; i++) {
        c->exps[i] = receive_int32(oxfp);
    }
    c->coef = receive_cmo(oxfp);
    return c;
}

static cmo_zz* receive_cmo_zz(OXFILE *oxfp)
{
    cmo_zz* c = new_cmo_zz();
    receive_mpz(oxfp, c->mpz);
    return c;
}

static cmo_zero* receive_cmo_zero(OXFILE *oxfp)
{
    return new_cmo_zero();
}

static cmo_dms_generic* receive_cmo_dms_generic(OXFILE *oxfp)
{
    return new_cmo_dms_generic();
}

static cmo_ring_by_name* receive_cmo_ring_by_name(OXFILE *oxfp)
{
    cmo* ob = receive_cmo(oxfp);
    /* We need to check semantics but yet ... */
    return new_cmo_ring_by_name(ob);
}

static cmo_distributed_polynomial* receive_cmo_distributed_polynomial(OXFILE *oxfp)
{
    cmo* ob;
    cmo_distributed_polynomial* c = new_cmo_distributed_polynomial();
    int len = receive_int32(oxfp);
    c->ringdef = receive_cmo(oxfp);

    while (len>0) {
        ob = receive_cmo(oxfp);
        list_append((cmo_list *)c, ob);
        len--;
    }
    return c;
}

static cmo_error2* receive_cmo_error2(OXFILE *oxfp)
{
    cmo* ob = receive_cmo(oxfp);
    return new_cmo_error2(ob);
}

/* receive_ox_tag() == OX_DATA の後に呼び出される */
/* 関数ポインタを使った方がきれいに書けるような気がする.  */
/* if (foo[tag] != NULL) foo[tag](oxfp); とか */

cmo* receive_cmo(OXFILE *oxfp)
{
    cmo* m;
    int tag = receive_int32(oxfp);
    switch(tag) {
    case CMO_NULL:
        m = receive_cmo_null(oxfp);
        break;
    case CMO_INT32:
        m = (cmo *)receive_cmo_int32(oxfp);
        break;
    case CMO_STRING:
        m = (cmo *)receive_cmo_string(oxfp);
        break;
    case CMO_MATHCAP:
        m = (cmo *)receive_cmo_mathcap(oxfp);
        break;
    case CMO_LIST:
        m = (cmo *)receive_cmo_list(oxfp);
        break;
    case CMO_MONOMIAL32:
        m = (cmo *)receive_cmo_monomial32(oxfp);
        break;
    case CMO_ZZ:
        m = (cmo *)receive_cmo_zz(oxfp);
        break;
    case CMO_ZERO:
        m = (cmo *)receive_cmo_zero(oxfp);
        break;
    case CMO_DMS_GENERIC:
        m = (cmo *)receive_cmo_dms_generic(oxfp);
        break;
    case CMO_RING_BY_NAME:
        m = (cmo *)receive_cmo_ring_by_name(oxfp);
        break;
    case CMO_DISTRIBUTED_POLYNOMIAL:
        m = (cmo *)receive_cmo_distributed_polynomial(oxfp);
        break;
    case CMO_ERROR2:
        m = (cmo *)receive_cmo_error2(oxfp);
        break;
    case CMO_DATUM:
    case CMO_QQ:
    default:
        m = NULL;
        fprintf(stderr, "the CMO (%d) is not implemented.\n", tag);
    }
    return m;
}

static void receive_mpz(OXFILE *oxfp, mpz_ptr mpz)
{
    int i;
    int size  = receive_int32(oxfp);
    int len   = abs(size);
    resize_mpz(mpz, size);

    for(i=0; i<len; i++) {
        mpz->_mp_d[i] = receive_int32(oxfp);
    }
}

void send_ox_command(OXFILE *oxfp, int sm_command)
{
    send_ox_tag(oxfp, OX_COMMAND);
    send_int32(oxfp, sm_command);
    oxf_flush(oxfp);
}

void ox_close(OXFILE *sv)
{
    send_ox_command(oxf_control(sv), SM_control_kill);
#ifdef DEBUG
    sleep(2);
    /* We wait thar an OpenXM server terminates. */
    fprintf(stderr, "I have closed the connection to an Open XM server.\n");
#endif
}

void ox_shutdown(OXFILE *sv)
{
    /* We need to use SM_shutdown but yet ... */
    ox_close(sv);
}

void ox_cmo_rpc(OXFILE *sv, char *function, int argc, cmo *argv[])
{
    int i = argc;
    while(i-- > 0) {
        send_ox_cmo(sv, argv[i]);
    }
    send_ox_cmo(sv, (cmo *)new_cmo_int32(argc));
    send_ox_cmo(sv, (cmo *)new_cmo_string(function));
    send_ox_command(sv, SM_executeFunction);
}

void ox_execute_string(OXFILE *sv, char* s)
{
    send_ox_cmo(sv, (cmo *)new_cmo_string(s));
    send_ox_command(sv, SM_executeStringByLocalParser);
}

void ox_push_cmd(OXFILE *sv, int sm_code)
{
    send_ox_command(sv, sm_code);
}

cmo_mathcap* ox_mathcap(OXFILE *sv)
{
    send_ox_command(sv, SM_mathcap);
    send_ox_command(sv, SM_popCMO);
    receive_ox_tag(sv);          /* OX_DATA */
    return (cmo_mathcap *)receive_cmo(sv);
}

char* ox_popString(OXFILE *sv)
{
    cmo_string* m = NULL;

    send_ox_command(sv, SM_popString);
    receive_ox_tag(sv); /* OX_DATA */
    m = (cmo_string *)receive_cmo(sv);
    return m->s;
}

void ox_pops(OXFILE *sv, int num)
{
    send_ox_cmo(sv, (cmo *)new_cmo_int32(num));
    send_ox_command(sv, SM_pops);
}

cmo* ox_pop_cmo(OXFILE *sv)
{
    send_ox_command(sv, SM_popCMO);
    receive_ox_tag(sv); /* OX_DATA */
    return receive_cmo(sv);
}

void ox_push_cmo(OXFILE *sv, cmo *c)
{
    send_ox_cmo(sv, c);
}

/* a dummy function for flushing a connection. */
int ox_flush(OXFILE *sv)
{
    return 1;
}

void ox_reset(OXFILE *sv)
{
    send_ox_command(oxf_control(sv), SM_control_reset_connection);

    receive_ox_tag(oxf_control(sv));      /* OX_DATA */
    receive_cmo(oxf_control(sv));         /* (CMO_INT32, 0) */

    while(receive_ox_tag(sv) != OX_SYNC_BALL) {
        receive_cmo(sv); /* skipping a message. */
    }

    send_ox_tag(sv, OX_SYNC_BALL);
#ifdef DEBUG
    fprintf(stderr, "I have reset an Open XM server.\n");
#endif
}

void send_ox(OXFILE *oxfp, ox *m)
{
    switch(m->tag) {
    case OX_DATA:
        send_ox_cmo(oxfp, ((ox_data *)m)->cmo);
        break;
    case OX_COMMAND:
        send_ox_command(oxfp, ((ox_command *)m)->command);
        break;
    default:
        /* CMO?? */
        send_ox_cmo(oxfp, (cmo *)m);
    }
}

void send_ox_cmo(OXFILE *oxfp, cmo* m)
{
    send_ox_tag(oxfp, OX_DATA);
    send_cmo(oxfp, m);
    oxf_flush(oxfp);
}

/* send_cmo_* functions */
static int send_cmo_null(OXFILE *oxfp, cmo_null* c)
{
    return 0;
}

static int send_cmo_int32(OXFILE *oxfp, cmo_int32* m)
{
    return send_int32(oxfp, m->i);
}

static int send_cmo_string(OXFILE *oxfp, cmo_string* m)
{
    int len = (m->s != NULL)? strlen(m->s): 0;
    send_int32(oxfp, len);
    if (len > 0) {
        oxf_write(m->s, 1, len, oxfp);
    }
    return 0;
}

static int send_cmo_mathcap(OXFILE *oxfp, cmo_mathcap* c)
{
    send_cmo(oxfp, c->ob);
    return 0;
}

static int send_cmo_list(OXFILE *oxfp, cmo_list* c)
{
    cell* el = list_first(c);
    int len = list_length(c);
    send_int32(oxfp, len);

    while(!list_endof(c, el)) {
        send_cmo(oxfp, el->cmo);
        el = list_next(el);
    }
    return 0;
}

static int send_cmo_distributed_polynomial(OXFILE *oxfp, cmo_distributed_polynomial* c)
{
    cell* el = list_first((cmo_list *)c);
    int len = list_length((cmo_list *)c);
    send_int32(oxfp, len);
    send_cmo(oxfp, c->ringdef);
    while(!list_endof((cmo_list *)c, el)) {
        send_cmo(oxfp, el->cmo);
        el = list_next(el);
    }
    return 0;
}

static int send_cmo_monomial32(OXFILE *oxfp, cmo_monomial32* c)
{
    int i;
    int len = c->length;
    send_int32(oxfp, len);
    for(i=0; i<len; i++) {
        send_int32(oxfp, c->exps[i]);
    }
    send_cmo(oxfp, c->coef);
    return 0;
}

static int send_cmo_zz(OXFILE *oxfp, cmo_zz* c)
{
    send_mpz(oxfp, c->mpz);
    return 0;
}

static int send_cmo_error2(OXFILE *oxfp, cmo_error2* c)
{
    send_cmo(oxfp, c->ob);
    return 0;
}

/* sending a CMO.  (Remarks: OX_tag is already sent.) */
void send_cmo(OXFILE *oxfp, cmo* c)
{
    int tag = c->tag;

    c = call_hook_before_send_cmo(oxfp, c);

    send_int32(oxfp, tag);
    switch(tag) {
    case CMO_NULL:
    case CMO_ZERO:
    case CMO_DMS_GENERIC:
        send_cmo_null(oxfp, c);  /* empty function. */
        break;
    case CMO_INT32:
        send_cmo_int32(oxfp, (cmo_int32 *)c);
        break;
    case CMO_STRING:
        send_cmo_string(oxfp, (cmo_string *)c);
        break;
    case CMO_MATHCAP:
    case CMO_ERROR2:
    case CMO_RING_BY_NAME:
    case CMO_INDETERMINATE:
        send_cmo_mathcap(oxfp, (cmo_mathcap *)c);
        break;
    case CMO_LIST:
        send_cmo_list(oxfp, (cmo_list *)c);
        break;
    case CMO_MONOMIAL32:
        send_cmo_monomial32(oxfp, (cmo_monomial32 *)c);
        break;
    case CMO_ZZ:
        send_cmo_zz(oxfp, (cmo_zz *)c);
        break;
    case CMO_DISTRIBUTED_POLYNOMIAL:
        send_cmo_distributed_polynomial(oxfp, (cmo_distributed_polynomial *)c);
        break;
    default:
        call_hook_after_send_cmo(oxfp, c);
    }
}

static int send_mpz(OXFILE *oxfp, mpz_ptr mpz)
{
    int i;
    int len = abs(mpz->_mp_size);
    send_int32(oxfp, mpz->_mp_size);
    for(i=0; i<len; i++) {
        send_int32(oxfp, mpz->_mp_d[i]);
    }
    return 0;
}

ox_data* new_ox_data(cmo* c)
{
    ox_data* m = malloc(sizeof(ox_data));
    m->tag = OX_DATA;
    m->cmo = c;
    return m;
}

ox_command* new_ox_command(int sm_code)
{
    ox_command* m = malloc(sizeof(ox_command));
    m->tag = OX_COMMAND;
    m->command = sm_code;
    return m;
}

ox_sync_ball* new_ox_sync_ball()
{
    ox_sync_ball *m = malloc(sizeof(ox_sync_ball));
    m->tag = OX_SYNC_BALL;
    return m;
}
