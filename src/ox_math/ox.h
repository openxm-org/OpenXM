/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */
/* $Id$ */

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

----
(5) receive_ox_XXX 関数は存在しない(作らない).  receive_cmo を利用する.
(6) send_ox_XXX 関数は OX タグを含めて送信する.
(7) ox_XXX 関数は一連の送受信を含むより抽象的な操作を表現する.
ox_XXX 関数は、第一引数として、ox_file_t型の変数 sv をとる.

(8) YYY_cmo 関数と YYY_cmo_XXX 関数の関係は次の通り:
まず YYY_cmo 関数で cmo のタグを処理し、タグを除いた残りの部分を
YYY_cmo_XXX 関数が処理する。cmo の内部に cmo_ZZZ へのポインタが
あるときには、その種類によらずに YYY_cmo 関数を呼び出す 
*/


#define DEFAULT_LOGFILE  "/tmp/result"

/* サーバーとの通信路に用いるファイルディスクリプタのペア. */
typedef struct {
    int stream;
    int control;
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

typedef struct {
    int tag;
    int i;
} cmo_int32;

typedef struct {
    int tag;
    char *s;
} cmo_string;

typedef struct {
    int tag;
    mpz_t mpz;
} cmo_zz;

typedef struct cell {
    struct cell *next;
    cmo *cmo;
} cell;

typedef struct {
    int tag;
    int length;   /* リストの長さ(必要??) */
    cell *head;
} cmo_list;

typedef struct {
    int tag;
    cmo_list *li;
} cmo_mathcap;

typedef cmo_mathcap cmo_error;

/*
typedef struct {
    int tag;
} ox;
*/

typedef cmo ox;

typedef struct {
    int tag;
    int command;
} ox_command;

typedef struct {
    int tag;
    cmo *cmo;
} ox_data;

cell*         new_cell(cmo* newcmo);
cmo_int32*    new_cmo_int32(int i);
cmo_list*     new_cmo_list();
cmo_string*   new_cmo_string(char* s);
cmo_zz*       new_cmo_zz();
cmo_zz*       new_cmo_zz_size(int size);
cmo_zz*       new_cmo_zz_set_si(int integer);
cmo_zz*       new_cmo_zz_noinit();
cmo_null*     new_cmo_null();
cmo_mathcap*  new_cmo_mathcap(cmo_list* li);
cmo_error*    new_cmo_error(cmo_list* li);
ox_data*      new_ox_data(cmo* c);
ox_command*   new_ox_command(int sm_code);

cmo_error*    gen_error_object(int err_code);
cmo*          make_mathcap_object(int version, char *id_string);

void          resize_mpz(mpz_ptr mpz, int size);
cmo*          receive_cmo(int fd);
cmo*          receive_cmo2(int fd);
int           receive_int32(int fd);
int           receive_ox_tag(int fd);

int           send_cmo(int fd, cmo* m);
int           send_int32(int fd, int integer);
int           send_ox(ox_file_t s, ox* m);
int           send_ox_cmo(int fd, cmo* m);
void          send_ox_command(int fd, int sm_command);
int           send_ox_tag(int fd, int tag);

int           append_cmo_list(cmo_list* this, cmo *newcmo);
int           length_cmo_list(cmo_list* this);
cell*         next_cell(cell *this);
int           cmolen_cmo(cmo* m);

void          ox_close(ox_file_t sv);
void          ox_executeStringByLocalParser(ox_file_t sv, char* str);
cmo_mathcap*  ox_mathcap(ox_file_t sv);
char*         ox_popString(ox_file_t sv, int fd);
cmo*          ox_pop_cmo(ox_file_t sv, int fd);
void          ox_reset(ox_file_t sv);
ox_file_t     ox_start(char* host, char* prog1, char* prog2);

char*         dump_cmo(char* array, cmo* m);
char*         dump_ox_command(char* array, ox_command* m);
char*         dump_ox_data(char* array, ox_data* m);

int           print_cmo(cmo* c);
int           print_cmo_int32(cmo_int32* c);
int           print_cmo_list(cmo_list* li);
int           print_cmo_mathcap(cmo_mathcap* c);
int           print_cmo_string(cmo_string* c);

int           decideByteOrder(int fd_read, int fd_write, int order);
int           decideByteOrder2(int fd_read, int fd_write, int order);
int           next_serial();
void          setCmotypeDisable(int type);

cmo_zz*       new_cmo_zz_set_string(char *s);
char*         CONVERT_ZZ_TO_CSTRING(cmo_zz *c);
char*         CONVERT_CMO_TO_CSTRING(cmo *m);
char*         CONVERT_NULL_TO_CSTRING();
char*         CONVERT_INT_TO_CSTRING(int integer);

#endif /* _OX_H_ */
