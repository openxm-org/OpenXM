/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/ox.h,v 1.5 1999/12/16 06:58:01 ohara Exp $ */

#ifndef _OX_H_

#define _OX_H_

#include <gmp.h>  
#include "oxtag.h"

/*
関数の名前付け規約(その2)
(1) receive_cmo 関数はCMOタグとデータ本体を受信する. この関数は CMOタグの
値が事前に分からないときに使用する. 返り値として、cmo へのポインタを返す。
(2) receive_cmo_XXX 関数は, CMOタグを親の関数で受信してから呼び出される関
数で、データ本体のみを受信し、cmo_XXX へのポインタを返す。しかも、
関数内部で new_cmo_XXX 関数を呼び出す。
(3) send_cmo 関数はCMOタグとデータ本体を送信する.
(4) send_cmo_XXX 関数はCMOタグを親の関数で送信してから呼び出される関数で、
データ本体のみを送信する.
(5) receive_ox_XXX 関数は存在しない(作らない).  receive_cmo を利用する.

----
(6) send_ox_XXX 関数は OX タグを含めて送信する.
(7) ox_XXX 関数は一連の送受信を含むより抽象的な操作を表現する.
ox_XXX 関数は、第一引数として、ox_file_t型の変数 sv をとる.

(8) YYY_cmo 関数と YYY_cmo_XXX 関数の関係は次の通り:
まず YYY_cmo 関数で cmo のタグを処理し、タグを除いた残りの部分を
YYY_cmo_XXX 関数が処理する。cmo の内部に cmo_ZZZ へのポインタが
あるときには、その種類によらずに YYY_cmo 関数を呼び出す 
*/

#define LOGFILE  "/tmp/oxtk.XXXXXX"

/* Open Xm File Descripter */
typedef int oxfd;

#if 0
/* そのうちこちらに移行したい... */
typedef struct {
    int fd;
    int byteorder;
} oxfile;
typedef oxfile *oxfd;
#endif

/* サーバーとの通信路に用いるファイルディスクリプタのペア. */
typedef struct {
    oxfd stream;
    oxfd control;
} __ox_file_struct;

typedef __ox_file_struct *ox_file_t;

/*
警告:
cmo_list 型のリストには破壊的な代入をしてはいけない.
cmo_list の各メンバに直接アクセスしてはいけない.
メソッド(..._cmo_list 関数)を用いること.
*/

/* ここからは新しい定義 */
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

typedef struct cell {
    struct cell *next;
    cmo *cmo;
} cell;

typedef struct {
    int tag;
    int length;   /* リストの長さ(必要??) */
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
    cmo *num;  /* 分子 (cmo_zz) */
    cmo *den;  /* 分母 (cmo_zz) */
} cmo_qq;

/* cmo_list の派生. append_cmo_list を使ってよい. */
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
cmo*               make_mathcap_object(int version, char *id_string);

/* 低水準 API */
cmo*               receive_cmo(int fd);
int                receive_int32(int fd);
int                receive_ox_tag(int fd);

int                send_cmo(int fd, cmo* m);
int                send_int32(int fd, int integer);
int                send_ox(int fd, ox* m);
int                send_ox_cmo(int fd, cmo* m);
void               send_ox_command(int fd, int sm_command);
int                send_ox_tag(int fd, int tag);

int                decideByteOrderClient(int fd, int order);
int                decideByteOrderServer(int fd, int order);
int                next_serial();
void               setCmotypeDisable(int type);
cell*              new_cell();
cmo*               nth_cmo_list(cmo_list* this, int n);
int                set_current_fd(int fd);

/* 高水準 API */
ox_file_t          ox_start(char* host, char* prog1, char* prog2);
ox_file_t          ox_start_insecure_nonreverse(char* host, short portControl, short portStream);
void               ox_close(ox_file_t sv);
void               ox_shutdown(ox_file_t sv);
void               ox_reset(ox_file_t sv);
void               ox_execute_string(ox_file_t sv, char* str);
cmo_mathcap*       ox_mathcap(ox_file_t sv);
char*              ox_popString(ox_file_t sv);
int                ox_pops(ox_file_t sv, int num);
cmo*               ox_pop_cmo(ox_file_t sv);
void               ox_push_cmo(ox_file_t sv, cmo *c);
void               ox_push_cmd(ox_file_t sv, int sm_code);
int                ox_cmo_rpc(ox_file_t sv, char *function, int argc, cmo *argv[]);
int                ox_flush(ox_file_t sv);

int                append_cmo_list(cmo_list* this, cmo *ob);
int                length_cmo_list(cmo_list* this);
cell*              next_cell(cell *this);
int                cmolen_cmo(cmo* m);
int                init_dump_buff(char *buff);
int                dump_cmo(cmo* m);
int                dump_ox_command(ox_command* m);
int                dump_ox_data(ox_data* m);

int                print_cmo(cmo* c);

typedef cmo *(*hook_t)(int, cmo *);

int add_hook_before_send_cmo(hook_t func);
int add_hook_after_send_cmo(hook_t func);

#endif /* _OX_H_ */
