/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/ox_toolkit.h,v 1.7 2000/11/27 09:57:10 ohara Exp $ */

#ifndef _OX_TOOLKIT_H_

#define _OX_TOOLKIT_H_

#include <stdio.h>
#include <gmp.h>
#include <ox/cmotag.h>
#include <ox/oxMessageTag.h>
#include <ox/smCommand.h>

#if !defined(__GNUC__) && !defined(__inline__)
#define __inline__
#endif

/* Mathcap Local Database */
typedef struct {
    int tag;
    int flag;
} table;

typedef struct mathcap {
    table *cmotbl;
    table *smtbl;
} mathcap;

/* OpenXM File Descripter */
typedef struct OXFILE{
    int fd;
    int (*send_int32)(struct OXFILE *oxfp, int int32);
    int (*receive_int32)(struct OXFILE *oxfp);
    int serial_number;
    struct OXFILE *control;  /* pointer to his control server. */
    struct mathcap *mathcap;
    int error;
} OXFILE;

typedef struct {
    int tag;
} cmo;

typedef cmo cmo_null;
typedef cmo cmo_zero;
typedef cmo cmo_dms_generic;

typedef struct {
    int tag;
    int i;
} cmo_int32;

typedef struct {
    int tag;
    int size;
    char *body;
} cmo_datum;

typedef struct {
    int tag;
    char *s;
} cmo_string;

typedef struct {
    int tag;
    cmo *ob;
} cmo_mathcap;

typedef cmo_mathcap cmo_error2;
typedef cmo_mathcap cmo_ring_by_name;
typedef cmo_mathcap cmo_indeterminate;

/* a double linked list */
typedef struct cell {
    cmo *cmo;
    struct cell *next;
    struct cell *prev;
} cell;

typedef struct {
    int tag;
    int length;   /* length of this list (unnecessary) */
    cell head[1];
} cmo_list;

typedef struct {
    int tag;
    int length;
    int *exps;
    cmo *coef;
} cmo_monomial32;

typedef struct {
    int tag;
    mpz_t mpz;
} cmo_zz;

typedef struct {
    int tag;
    cmo *num;  /* Bunshi (cmo_zz) */
    cmo *den;  /* Bunbo (cmo_zz) */
} cmo_qq;

/* The following is a derived class from cmo_list. 
   that is, list_append can be used. */
typedef struct {
    int tag;
    int length;    /* number of monomials */
    cell head[1];  /* a list of monomials */
    cmo *ringdef;
} cmo_distributed_polynomial;

typedef cmo ox;

typedef ox ox_sync_ball;

typedef struct {
    int tag;
    int command;
} ox_command;

typedef struct {
    int tag;
    cmo *cmo;
} ox_data;

cmo_null*          new_cmo_null();
cmo_int32*         new_cmo_int32(int i);
cmo_string*        new_cmo_string(char* s);
cmo_mathcap*       new_cmo_mathcap(cmo* ob);
cmo_list*          new_cmo_list();
cmo_monomial32*    new_cmo_monomial32();
cmo_monomial32*    new_cmo_monomial32_size(int size);
cmo_zz*            new_cmo_zz();
cmo_zz*            new_cmo_zz_size(int size);
cmo_zz*            new_cmo_zz_set_si(int integer);
cmo_zz*            new_cmo_zz_set_mpz(mpz_ptr z);
cmo_zz*            new_cmo_zz_noinit();
cmo_zz*            new_cmo_zz_set_string(char* s);
cmo_zero*          new_cmo_zero();
cmo_distributed_polynomial* new_cmo_distributed_polynomial();
cmo_dms_generic*   new_cmo_dms_generic();
cmo_ring_by_name*  new_cmo_ring_by_name(cmo* ob);
cmo_indeterminate* new_cmo_indeterminate(cmo* ob);
cmo_error2*        new_cmo_error2(cmo* ob);

ox_data*           new_ox_data(cmo* c);
ox_command*        new_ox_command(int sm_code);
ox_sync_ball*      new_ox_sync_ball();

char*              new_string_set_cmo(cmo* m);

cmo_error2*        make_error_object(int err_code, cmo* ob);

/* Low level API */
cmo*               receive_cmo(OXFILE *fp);
int                receive_int32(OXFILE *fp);
int                receive_ox_tag(OXFILE *fp);

void               send_cmo(OXFILE *fp, cmo* m);
int                send_int32(OXFILE *fp, int integer);
void               send_ox(OXFILE *fp, ox* m);
void               send_ox_cmo(OXFILE *fp, cmo* m);
void               send_ox_command(OXFILE *fp, int sm_command);
int                send_ox_tag(OXFILE *fp, int tag);

int                next_serial();
void               setCmotypeDisable(int type);

/* High level API */
void               ox_close(OXFILE *sv);
void               ox_shutdown(OXFILE *sv);
void               ox_reset(OXFILE *sv);
void               ox_execute_string(OXFILE *sv, char* str);
cmo_mathcap*       ox_mathcap(OXFILE *sv);
char*              ox_popString(OXFILE *sv);
void               ox_pops(OXFILE *sv, int num);
cmo*               ox_pop_cmo(OXFILE *sv);
void               ox_push_cmo(OXFILE *sv, cmo *c);
void               ox_push_cmd(OXFILE *sv, int sm_code);
void               ox_cmo_rpc(OXFILE *sv, char *function, int argc, cmo *argv[]);
int                ox_flush(OXFILE *sv);

cell*              list_first(cmo_list *this);
int                list_endof(cmo_list *this, cell *el);
cell*              list_next(cell *el);
cmo_list*          list_append(cmo_list* this, cmo *ob);
cmo_list*          list_appendl(cmo_list* this, ...);
int                list_length(cmo_list* this);
cmo*               list_nth(cmo_list* this, int n);

int                cmolen_cmo(cmo* m);
void               dump_buffer_init(char *s);
void               dump_cmo(cmo* m);
void               dump_ox_command(ox_command* m);
void               dump_ox_data(ox_data* m);

void               print_cmo(cmo* c);
void               resize_mpz(mpz_ptr mpz, int size);

typedef cmo *(*hook_t)(OXFILE *, cmo *);

int add_hook_before_send_cmo(hook_t func);
int add_hook_after_send_cmo(hook_t func);

/* functions related to parse.c */

#define PFLAG_ADDREV   1

typedef struct symbol *symbol_t;

void setflag_parse(int flag);
cmo *parse();
void init_parser(char *s);

symbol_t lookup_by_symbol(char *key);
symbol_t lookup_by_token(int tok);
symbol_t lookup_by_tag(int tag);
symbol_t lookup(int i);
char *symbol_get_key(symbol_t sp);

/* for mathcap database */
mathcap *new_mathcap();
void mathcap_init(int ver, char *vstr, char *sysname, int cmos[], int sms[]);
cmo_mathcap* mathcap_get(mathcap *this);
mathcap *mathcap_update(mathcap *this, cmo_mathcap *mc);
int mathcap_allowQ_cmo(mathcap *this, cmo *ob);

int oxf_read(void *buffer, size_t size, size_t num, OXFILE *oxfp);
int oxf_write(void *buffer, size_t size, size_t num, OXFILE *oxfp);

/* for OXFILE */
int oxf_listen(short *portp);
OXFILE *oxf_connect_active(char *hostname, short port);
OXFILE *oxf_connect_passive(int listened);
OXFILE *oxf_open(int fd);
OXFILE *oxf_control_set(OXFILE *oxfp, OXFILE *control);
OXFILE *oxf_control(OXFILE *oxfp);
int  oxf_confirm_client(OXFILE *oxfp, char *passwd);
int  oxf_confirm_server(OXFILE *oxfp, char *passwd);
void oxf_flush(OXFILE *oxfp);
void oxf_close(OXFILE *oxfp);
void oxf_setopt(OXFILE *oxfp, int mode);
void oxf_determine_byteorder_client(OXFILE *oxfp);
void oxf_determine_byteorder_server(OXFILE *oxfp);
OXFILE *oxf_execute_cmd(OXFILE *oxfp, char *cmd);
cmo_mathcap *oxf_cmo_mathcap(OXFILE *oxfp);
void oxf_mathcap_update(OXFILE *oxfp, cmo_mathcap *ob);

/* example: which("xterm", getenv("PATH")); */
char *which(char *exe, const char *env);
char *generate_otp();

#endif /* _OX_TOOLKIT_H_ */
