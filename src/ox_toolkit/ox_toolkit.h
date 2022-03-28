/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/ox_toolkit.h,v 1.46 2020/11/01 10:28:22 noro Exp $ */

#ifndef _OX_TOOLKIT_H_
#define _OX_TOOLKIT_H_


#include <stdio.h>
#include <string.h>
#include <gmp.h>
#include <mpfr.h>
#include <ox/cmotag.h>
#include <ox/oxMessageTag.h>
#include <ox/smCommand.h>
#include <gc/gc.h>

#if defined(_MSC_VER)
#include <malloc.h>
#else
#include <sys/select.h>
#endif

#define MALLOC(x) GC_MALLOC((x))
#define MALLOC_ATOMIC(x) GC_MALLOC_ATOMIC((x))
#define ALLOCA(x) alloca((x))
/* #define FREE(x)   free((x)) */
#define FREE(x)   

#ifdef __cplusplus
extern "C" {
#endif

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
    char  **opts;
} mathcap;

/* OpenXM File Descripter */
typedef struct OXFILE{
    int fd;
    int (*send_int32)(struct OXFILE *oxfp, int int32);
    int (*receive_int32)(struct OXFILE *oxfp);
    int serial_number;
    int received_serial_number;
    struct OXFILE *control;  /* pointer to his control server. */
    struct mathcap *mathcap;
    int error;
    char *wbuf;
    int wbuf_size;
    int wbuf_count;
    int (*send_double)(struct OXFILE *oxfp, double int64);
    double (*receive_double)(struct OXFILE *oxfp);
} OXFILE;

#if 0
#define OX_FD_SETSIZE   FD_SETSIZE
#else
#define OX_FD_SETSIZE   32
#endif

typedef struct {
    int count;
    fd_set fdset;
    OXFILE *p[OX_FD_SETSIZE];
} OXFILE_set;

typedef struct cmo {
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
    struct cmo *cmo;
    struct cell *next;
    struct cell *prev;
    int exp;
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
    mpq_t mpq;
} cmo_qq;

typedef struct {
    int tag;
    mpfr_t mpfr;
} cmo_bf;

typedef struct {
    int tag;
    cmo *re,*im;
} cmo_complex;

typedef struct {
    int tag;
    double d; /* machine dependent */
} cmo_double;

/* The following is a derived class from cmo_list. 
   that is, list_append can be used. */
typedef struct {
    int tag;
    int length;    /* number of monomials */
    cell head[1];  /* a list of monomials */
    cmo *ringdef;
} cmo_distributed_polynomial;

/* The following is a derived class from cmo_list. 
   that is, list_append can be used. */
typedef struct {
    int tag;
    int length;   /* number of monomials */
    cell head[1]; /* list of monomials */
    int var;      /* name of the main variable */
} cmo_polynomial_in_one_variable;

typedef struct {
    int tag;
    cmo_list *ringdef; /* list of variables */
    cmo *coef;  /* ZZ, QQ, int32, Poly_in_1var, Tree, Zero, DPoly */
} cmo_recursive_polynomial;

typedef struct {
    int tag;
    cmo_string *name;
    cmo_list *attributes;
    cmo_list *leaves;
} cmo_tree;

typedef struct {
    int tag;
    cmo_list *args;
    cmo_tree *body;
} cmo_lambda;

typedef cmo ox;

typedef ox ox_sync_ball;

typedef struct {
    int tag;
    int command;
} ox_command;

typedef struct {
    int tag;
    struct cmo *cmo;
} ox_data;

cmo_null*          new_cmo_null();
cmo_int32*         new_cmo_int32(int i);
cmo_string*        new_cmo_string(char* s);
cmo_mathcap*       new_cmo_mathcap(cmo* ob);
cmo_list*          new_cmo_list();
cmo_list*          new_cmo_list_array(void *a[], int n);
cmo_list*          new_cmo_list_array_map(void *a[], int n, void *(* mapf)(void *));
cmo_monomial32*    new_cmo_monomial32();
cmo_monomial32*    new_cmo_monomial32_size(int size);
cmo_zz*            new_cmo_zz();
cmo_zz*            new_cmo_zz_size(int size);
cmo_zz*            new_cmo_zz_set_si(int integer);
cmo_zz*            new_cmo_zz_set_mpz(mpz_ptr z);
cmo_zz*            new_cmo_zz_noinit();
cmo_zz*            new_cmo_zz_set_string(char* s);
cmo_qq*            new_cmo_qq();
cmo_qq*            new_cmo_qq_set_mpq(mpq_ptr q);
cmo_qq*            new_cmo_qq_set_mpz(mpz_ptr num, mpz_ptr den);
cmo_bf*            new_cmo_bf();
cmo_bf*            new_cmo_bf_set_mpfr(mpfr_ptr q);
cmo_complex*       new_cmo_complex();
cmo_complex*       new_cmo_complex_set_re_im(cmo *re,cmo *im);
cmo_zero*          new_cmo_zero();
cmo_double*        new_cmo_double(double d);
cmo_distributed_polynomial* new_cmo_distributed_polynomial();
cmo_dms_generic*   new_cmo_dms_generic();
cmo_ring_by_name*  new_cmo_ring_by_name(cmo* ob);
cmo_indeterminate* new_cmo_indeterminate(cmo* ob);
cmo_indeterminate* new_cmo_indeterminate_set_name(char *s);
cmo_polynomial_in_one_variable* new_cmo_polynomial_in_one_variable(int var);
cmo_recursive_polynomial* new_cmo_recursive_polynomial(cmo_list* ringdef, cmo* coef);
cmo_tree*          new_cmo_tree(cmo_string* name, cmo_list *attributes, cmo_list *leaves);
cmo_lambda*        new_cmo_lambda(cmo_list* args, cmo_tree* body);
cmo_error2*        new_cmo_error2(cmo* ob);

ox_data*           new_ox_data(cmo* c);
ox_command*        new_ox_command(int sm_code);
ox_sync_ball*      new_ox_sync_ball();

char*              new_string_set_cmo(cmo* m);

cmo_error2*        make_error_object(int err_code, cmo* ob);

cmo*               ox_parse_lisp(char *s);

/* Low level API */
cmo*               receive_cmo(OXFILE *fp);
cmo*               receive_cmo_tag(OXFILE *fp, int tag);
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

cell*              list_first(cmo_list *);
int                list_endof(cmo_list *, cell *el);
cell*              list_next(cell *el);
cmo_list*          list_append(cmo_list*, cmo *ob);
cmo_list*          list_append_monomial(cmo_list* , cmo* coef, int exp);
cmo_list*          list_appendl(cmo_list*, ...);
int                list_length(cmo_list* );
cmo*               list_nth(cmo_list* , int n);
cell*              list_nth_cell(cmo_list* , int n);
cmo*               list_first_cmo(cmo_list *);
char*              cmo_indeterminate_get_name(cmo_indeterminate *);

int                cmolen_cmo(cmo* m);
void               dump_buffer_init(char *s);
void               dump_cmo(cmo* m);
void               dump_ox_command(ox_command* m);
void               dump_ox_data(ox_data* m);

void               print_cmo(cmo* c);
void               resize_mpz(mpz_ptr mpz, int size);

int                cmo_to_int(cmo *n);

typedef cmo *(*hook_t)(OXFILE *, cmo *);

int add_hook_before_send_cmo(hook_t func);
int add_hook_after_send_cmo(hook_t func);

/* functions related to parse.c */

#define PFLAG_ADDREV   1

typedef struct symbol *symbol_t;

symbol_t lookup_by_symbol(char *key);
symbol_t lookup_by_token(int tok);
symbol_t lookup_by_tag(int tag);
symbol_t lookup(int i);
char*    get_symbol_by_tag(int tag);

/* for mathcap database */
mathcap *new_mathcap();
void mathcap_init(int ver, char *vstr, char *sysname, int cmos[], int sms[]);
void mathcap_init2(int ver, char *vstr, char *sysname, int cmos[], int sms[], char *options[]);
cmo_mathcap* mathcap_get(mathcap *);
mathcap *mathcap_update(mathcap *, cmo_mathcap *mc);
int mathcap_allowQ_cmo(mathcap *, cmo *ob);

int oxf_read(void *buffer, size_t size, size_t num, OXFILE *oxfp);
int oxf_write(void *buffer, size_t size, size_t num, OXFILE *oxfp);

/* for OXFILE */
int oxf_listen(int *portp);
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
int oxf_setbuffer(OXFILE *oxfp, char *buf, int size);

/* example: which("xterm", getenv("PATH")); */
char *which(char *exe, const char *env);
char *generate_otp();

int ox_stderr_init(FILE *fp);
int ox_printf(char *format, ...);

int gclose();
int gopen();
int gFlush(); 
int moveto(int x, int y);
int lineto(int x, int y);
int setpixel(int x, int y);

#ifdef __cplusplus
}
#endif

#endif /* _OX_TOOLKIT_H_ */
