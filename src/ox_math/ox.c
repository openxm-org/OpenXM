/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/ox.c,v 1.2 1999/11/02 06:11:57 ohara Exp $ */
/* $Id$ */

/*
関数の名前付け規約(その2):
(1) receive_cmo 関数はCMOタグとデータ本体を受信する. この関数は CMOタグの
値が事前に分からないときに使用する. 返り値として、cmo へのポインタを返す.
(2) receive_cmo_XXX 関数は, CMOタグを親の関数で受信してから呼び出される関
数で、データ本体のみを受信し、cmo_XXX へのポインタを返す.  しかも、
関数内部で new_cmo_XXX 関数を呼び出す.
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
YYY_cmo_XXX 関数が処理する.  cmo の内部に cmo_ZZZ へのポインタが
あるときには、その種類によらずに YYY_cmo 関数を呼び出す.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <gmp.h>

#include "mysocket.h"
#include "ox.h"
#include "parse.h"

static int          cmolen_cmo_int32(cmo_int32* c);
static int          cmolen_cmo_list(cmo_list* c);
static int          cmolen_cmo_mathcap(cmo_mathcap* c);
static int          cmolen_cmo_null(cmo_null* c);
static int          cmolen_cmo_string(cmo_string* c);
static int          cmolen_cmo_zz(cmo_zz* c);
static int          cmolen_cmo_monomial32(cmo_monomial32* c);

static char*        dump_cmo_int32(char* array, cmo_int32* m);
static char*        dump_cmo_list(char* array, cmo_list* m);
static char*        dump_cmo_mathcap(char* array, cmo_mathcap* m);
static char*        dump_cmo_null(char* array, cmo_null* m);
static char*        dump_cmo_string(char* array, cmo_string* m);
static char*        dump_cmo_monomial32(char* array, cmo_monomial32* c);
static char*        dump_cmo_zz(char* array, cmo_zz* c);
static char*        dump_integer(char* array, int x);
static char*        dump_mpz(char* array, mpz_ptr mpz);

static int          funcs(int cmo_type);

/* CMO_xxx の値順にならべること(デバッグのため) */
static int          read_password(int fd, char* passwd);
static cmo_null*    receive_cmo_null(int fd);
static cmo_int32*   receive_cmo_int32(int fd);
static cmo_string*  receive_cmo_string(int fd);
static cmo_mathcap* receive_cmo_mathcap(int fd);
static cmo_list*    receive_cmo_list(int fd);
static cmo_zz*      receive_cmo_zz(int fd);
static cmo_zero*    receive_cmo_zero(int fd);
static cmo_dms_generic* receive_cmo_dms_generic(int fd);
static cmo_error2*  receive_cmo_error2(int fd);
static void         receive_mpz(int fd, mpz_ptr mpz);

static int          send_cmo_null(int fd, cmo_null* c);
static int          send_cmo_int32(int fd, cmo_int32* m);
static int          send_cmo_string(int fd, cmo_string* m);
static int          send_cmo_mathcap(int fd, cmo_mathcap* c);
static int          send_cmo_list(int fd, cmo_list* c);
static int          send_cmo_zz(int fd, cmo_zz* c);
static int          send_cmo_error2(int fd, cmo_error2* c);
static int          send_mpz(int fd, mpz_ptr mpz);


/* エラーハンドリングのため */
static int current_received_serial = 0;

/* エラーを起こしたときはサーバは次のようにすればよい.  */
cmo_error2* gen_error_object(int err_code)
{
    cmo_list* li = new_cmo_list();
    append_cmo_list(li, (cmo *)new_cmo_int32(current_received_serial));
    append_cmo_list(li, (cmo *)new_cmo_int32(err_code));
    /* 他の情報を加えるならココ */
    return new_cmo_error2((cmo *)li);
}

/* add at Mon Sep  7 15:51:28 JST 1998 */
#define DEFAULT_SERIAL_NUMBER 0x0000ffff
#define receive_serial_number(x)   (receive_int32(x))

/* 新しいシリアル番号を得る */
int next_serial()
{
    static int serial_number = DEFAULT_SERIAL_NUMBER;
    return serial_number++;
}

/* int32 型のオブジェクトを送信する.  */
int send_int32(int fd, int int32)
{
    int32 = htonl(int32);
    return write(fd, &int32, sizeof(int));
}

/* int32 型のオブジェクトを受信する.  */
int receive_int32(int fd)
{
    int tag;
    read(fd, &tag, sizeof(int));
    return ntohl(tag);
}

/* (OX_tag, serial number) を受信する.  */
int receive_ox_tag(int fd)
{
    int serial;
    int tag = receive_int32(fd);
    current_received_serial = receive_serial_number(fd);
    return tag;
}

/* (OX_tag, serial number) を送信する.   */
int send_ox_tag(int fd, int tag)
{
    send_int32(fd, tag);
    return send_int32(fd, next_serial());
}


/* CMO_LIST 関係の関数群 */
cell* new_cell(cmo* newcmo)
{
    cell* h = malloc(sizeof(cell));
    h->next = NULL;
    h->cmo  = newcmo;
    return h;
}

cell* next_cell(cell* this)
{
    return this->next;
}

static cell* *tailp(cmo_list* this) {
    cell **cp = &this->head;
    while (*cp != NULL) {
        cp = &((*cp)->next);
    }
    return cp;
}

int length_cmo_list(cmo_list* this)
{
    return this->length;
}

int append_cmo_list(cmo_list* this, cmo* newcmo)
{
    /* リストの最後尾のNULLを保持している変数へのポインタ */
    cell **cp = tailp(this);
    *cp = new_cell(newcmo);
    this->length++;
    return 0;
}

/** receive_cmo_XXX 関数群 **/
static cmo_null* receive_cmo_null(int fd)
{
    return new_cmo_null();
}

static cmo_int32* receive_cmo_int32(int fd)
{
    int i = receive_int32(fd);
    return new_cmo_int32(i);
}

static cmo_string* receive_cmo_string(int fd)
{
    int len = receive_int32(fd);
    char* s = malloc(len+1);
    memset(s, '\0', len+1);
    if (len > 0) {
        read(fd, s, len);
    }
    return new_cmo_string(s);
}

static cmo_mathcap* receive_cmo_mathcap(int fd)
{
    cmo* ob = receive_cmo(fd);
    return new_cmo_mathcap(ob);
}

static cmo_list* receive_cmo_list(int fd)
{
    cmo* newcmo;
    cmo_list* c = new_cmo_list();
    int len = receive_int32(fd);

    while (len>0) {
        newcmo = receive_cmo(fd);
        append_cmo_list(c, newcmo);
        len--;
    }
    return c;
}

static cmo_zz* receive_cmo_zz(int fd)
{
    cmo_zz* c = new_cmo_zz();
    receive_mpz(fd, c->mpz);
    return c;
}

static cmo_zero* receive_cmo_zero(int fd)
{
    return new_cmo_zero();
}

static cmo_dms_generic* receive_cmo_dms_generic(int fd)
{
    return new_cmo_dms_generic();
}

static cmo_ring_by_name* receive_cmo_ring_by_name(int fd)
{
    cmo* ob = receive_cmo(fd);
	/* 意味的チェックが必要 */
    return new_cmo_ring_by_name(ob);
}

static cmo_error2* receive_cmo_error2(int fd)
{
    cmo* ob = receive_cmo(fd);
    return new_cmo_error2(ob);
}

static void receive_mpz(int fd, mpz_ptr mpz)
{
    int i;
    int size  = receive_int32(fd);
    int len   = abs(size);
    resize_mpz(mpz, size);

    for(i=0; i<len; i++) {
        mpz->_mp_d[i] = receive_int32(fd);
    }
}

void resize_mpz(mpz_ptr mpz, int size)
{
    _mpz_realloc(mpz, abs(size));
    mpz->_mp_size = size;
}

/** new_cmo_XXX 関数群 **/
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
	char *s2 = malloc(strlen(s)+1);
	strcpy(s2, s);

    c->tag = CMO_STRING;
    c->s   = s2;
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
    c->head   = NULL;
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
    c->head    = NULL;
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

/* receive_ox_tag() == OX_DATA の後に呼び出される */
/* 関数ポインタを使った方がきれいに書けるような気がする.  */
/* if (foo[tag] != NULL) foo[tag](fd); とか */

cmo* receive_cmo(int fd)
{
    cmo* m;
    int tag;
    tag = receive_int32(fd);

    switch(tag) {
    case CMO_NULL:
        m = receive_cmo_null(fd);
        break;
    case CMO_INT32:
        m = (cmo *)receive_cmo_int32(fd);
        break;
    case CMO_STRING:
        m = (cmo *)receive_cmo_string(fd);
        break;
    case CMO_MATHCAP:
        m = (cmo *)receive_cmo_mathcap(fd);
        break;
    case CMO_LIST:
        m = (cmo *)receive_cmo_list(fd);
        break;
    case CMO_ZZ:
        m = (cmo *)receive_cmo_zz(fd);
        break;
    case CMO_ERROR2:
        m = (cmo *)receive_cmo_error2(fd);
        break;
    case CMO_DATUM:
    case CMO_QQ:
    case CMO_ZERO:
    case CMO_DMS:
    default:
        fprintf(stderr, "unknown cmo-type: tag = (%d)\n", m->tag);
    }
    return m;
}

void send_ox_command(int fd, int sm_command)
{
    send_ox_tag(fd, OX_COMMAND);
    send_int32(fd, sm_command);
}

int print_cmo(cmo* c)
{
    int tag = c->tag;
    fprintf(stderr, "local::tag = (%d): ", tag);
    switch(tag) {
    case CMO_LIST:
        print_cmo_list((cmo_list *)c);
        break;
    case CMO_INT32:
        print_cmo_int32((cmo_int32 *)c);
        break;
    case CMO_MATHCAP:
        print_cmo_mathcap((cmo_mathcap *)c);
        break;
    case CMO_STRING:
        print_cmo_string((cmo_string *)c);
        break;
    case CMO_NULL:
        fprintf(stderr, "\n");
        break;
    default:
        fprintf(stderr, "print_cmo() does not know how to print.\n");
    }
}

int print_cmo_int32(cmo_int32* c)
{
    fprintf(stderr, "cmo_int32 = (%d)\n", c->i);
}

int print_cmo_list(cmo_list* li)
{
    cell* cp = li->head;
    fprintf(stderr, "length = (%d)\nlist:\n", li->length);
    while(cp != NULL) {
        print_cmo(cp->cmo);
        cp=cp->next;
    }
    fprintf(stderr, "end of list\n");
}

int print_cmo_mathcap(cmo_mathcap* c)
{
    fprintf(stderr, "\n");
    print_cmo(c->ob);
}

int print_cmo_string(cmo_string* c)
{
    fprintf(stderr, "cmo_string = (%s)\n", c->s);
}

/* ox_start() から呼び出す */
/* OneTimePassword の処理 */
static int read_password(int fd, char* passwd)
{
    char buff[1024];
    int n;
    if ((n = read(fd, buff, 1024)) != strlen(passwd)) {
        fprintf(stderr, "Socket#%d: Login incorrect.\n", fd);
        fprintf(stderr, "password = (%s), received = (%s).\n", passwd, buff);
        fprintf(stderr, "         = (%d), received = (%d).\n", strlen(passwd), n);
    }
    fflush(stderr);
}

void ox_close(ox_file_t sv)
{
    send_ox_command(sv->control, SM_control_kill);
#if DEBUG
    sleep(2); /* OpenXM server の終了を待つ. あまり意味はない. */
    fprintf(stderr, "I have closed an Open XM server.\n");
#endif
}

void ox_executeStringByLocalParser(ox_file_t sv, char* s)
{
    /* 文字列ををスタックにプッシュ. */
    send_ox_cmo(sv->stream, (cmo *)new_cmo_string(s));

    /* サーバに実行させる. */
    send_ox_command(sv->stream, SM_executeStringByLocalParser);
}

/* ox_mathcap() をコールする.  */
cmo_mathcap* ox_mathcap(ox_file_t sv)
{
    send_ox_command(sv->stream, SM_mathcap);
    send_ox_command(sv->stream, SM_popCMO);
    receive_ox_tag(sv->stream);          /* OX_DATA */
    return (cmo_mathcap *)receive_cmo(sv->stream);
}

char* ox_popString(ox_file_t sv, int fd)
{
    cmo_string* m = NULL;

    send_ox_command(fd, SM_popString);
    receive_ox_tag(fd); /* OX_DATA */
    m = (cmo_string *)receive_cmo(fd);
    return m->s;
}

cmo* ox_pop_cmo(ox_file_t sv, int fd)
{
    send_ox_command(fd, SM_popCMO);
    receive_ox_tag(fd); /* OX_DATA */
    return receive_cmo(fd);
}

/* 
   ox_start は クライアントが呼び出すための関数である.
   サーバでは使われない.  prog1 は コントロールサーバであり,
   -ox, -reverse, -data, -control, -pass, -host
   というオプションを理解することを仮定する. prog2 は計算サーバである.
   接続時には, sv->control を先にオープンする.
*/

ox_file_t ox_start(char* host, char* prog1, char* prog2)
{
    static char pass[] = "passwd";
    char ctl[16], dat[16];
    short portControl = 0; /* short であることに注意 */
    short portStream  = 0;
    ox_file_t sv = malloc(sizeof(__ox_file_struct));

    sv->control = mysocketListen(host, &portControl);
    sv->stream  = mysocketListen(host, &portStream);
    sprintf(ctl, "%d", portControl);
    sprintf(dat, "%d", portStream);

    if (fork() == 0) {
        dup2(2, 1);
        dup2(open(DEFAULT_LOGFILE, O_RDWR|O_CREAT|O_TRUNC, 0644), 2);
        execl(prog1, prog1, "-reverse", "-ox", prog2,
              "-data", dat, "-control", ctl, "-pass", pass,
              "-host", host, NULL);
    }

    sv->control = mysocketAccept(sv->control);
    decideByteOrderWithReadPasswd(sv->control, sv->control, 0, pass);
    usleep(10);
    sv->stream  = mysocketAccept(sv->stream);
    decideByteOrderWithReadPasswd(sv->control, sv->control, 0, pass);

    return sv;
}

void ox_reset(ox_file_t sv)
{
    send_ox_command(sv->control, SM_control_reset_connection);

    receive_ox_tag(sv->control);      /* OX_DATA */
    receive_cmo(sv->control);         /* (CMO_INT32, 0) */

    while(receive_ox_tag(sv->stream) != OX_SYNC_BALL) {
        receive_cmo(sv->stream); /* skipping a message. */
    }

    send_ox_tag(sv->stream, OX_SYNC_BALL);
#if DEBUG
    fprintf(stderr, "I have reset an Open XM server.\n");
#endif
}

/* 以下は bconv.c で必要とする関数群である. */

/* cmolen 関数は cmo の(送信時の)バイト長を返す. */
/* cmolen_XXX 関数は cmo_XXX の tag を除いたバイト長を返す. */

static int cmolen_cmo_null(cmo_null* c)
{
    return 0;
}

static int cmolen_cmo_int32(cmo_int32* c)
{
    return sizeof(int);
}

static int cmolen_cmo_string(cmo_string* c)
{
    return sizeof(int)+strlen(c->s);
}

static int cmolen_cmo_mathcap(cmo_mathcap* c)
{
    return cmolen_cmo(c->ob);
}

static int cmolen_cmo_list(cmo_list* c)
{
    int size = sizeof(c->head);
    cell* cp = c->head;

    while(cp != NULL) {
        size += cmolen_cmo(cp->cmo);
        cp = cp->next;
    }
    return size;
}

static int cmolen_cmo_monomial32(cmo_monomial32* c)
{
	int len = (c->length + 1)*sizeof(int);
	return len + cmolen_cmo(c->coef);
}

static int cmolen_cmo_zz(cmo_zz* c)
{
    int len = abs(c->mpz->_mp_size);
    return sizeof(len) + len*sizeof(int);
}

static int cmolen_cmo_distributed_polynomial(cmo_distributed_polynomial* c)
{
	return cmolen_cmo_list((cmo_list *)c) + cmolen_cmo(c->ringdef);
}

/* CMO がバイトエンコードされた場合のバイト列の長さを求める */
int cmolen_cmo(cmo* c)
{
    int size = sizeof(int);

    switch(c->tag) {
    case CMO_NULL:
    case CMO_ZERO:
    case CMO_DMS_GENERIC:
        size += cmolen_cmo_null(c);
        break;
    case CMO_INT32:
        size += cmolen_cmo_int32((cmo_int32 *)c);
        break;
    case CMO_STRING:
        size += cmolen_cmo_string((cmo_string *)c);
        break;
    case CMO_MATHCAP:
    case CMO_RING_BY_NAME:
    case CMO_ERROR2:
        size += cmolen_cmo_mathcap((cmo_mathcap *)c);
        break;
    case CMO_LIST:
        size += cmolen_cmo_list((cmo_list *)c);
        break;
    case CMO_MONOMIAL32:
        size += cmolen_cmo_monomial32((cmo_monomial32 *)c);
        break;
    case CMO_ZZ:
        size += cmolen_cmo_zz((cmo_zz *)c);
        break;
	case CMO_DISTRIBUTED_POLYNOMIAL:
		size += cmolen_cmo_distributed_polynomial((cmo_distributed_polynomial *)c);
		break;
    default:
    }
    return size;
}

static char* dump_cmo_null(char* array, cmo_null* m)
{
    return array;
}

static char* dump_cmo_int32(char* array, cmo_int32* m)
{
    return dump_integer(array, m->i);
}

static char* dump_cmo_string(char* array, cmo_string* m)
{
    int len = strlen(m->s);
    array = dump_integer(array, len);
    memcpy(array, m->s, len);
    return array + len;
}

static char* dump_cmo_mathcap(char* array, cmo_mathcap* c)
{
    return dump_cmo(array, c->ob);
}

static char* dump_cmo_list(char* array, cmo_list* m)
{
    cell* cp = m->head;
    int len = length_cmo_list(m);
    array = dump_integer(array, len);

    while(cp != NULL) {
        array = dump_cmo(array, cp->cmo);
        cp = cp->next;
    }
    return array;
}

static char* dump_cmo_monomial32(char* array, cmo_monomial32* c)
{
	int i;
	int length = c->length;
	array = dump_integer(array, c->length);
	for(i=0; i<length; i++) {
		array = dump_integer(array, c->exps[i]);
	}
	array = dump_cmo(array, c->coef);
	return array;	
}

static char* dump_cmo_zz(char* array, cmo_zz* c)
{
    return dump_mpz(array, c->mpz);
}

static char* dump_cmo_distributed_polynomial(char* array, cmo_distributed_polynomial* m)
{
    cell* cp = m->head;
    int len = length_cmo_list(m);
    array = dump_integer(array, len);
	array = dump_cmo(array, m->ringdef);
    while(cp != NULL) {
        array = dump_cmo(array, cp->cmo);
        cp = cp->next;
    }
    return array;
}

/* タグを書き出してから、各関数を呼び出す */
char* dump_cmo(char* array, cmo* m)
{
    array = dump_integer(array, m->tag);
    switch(m->tag) {
    case CMO_NULL:
    case CMO_ZERO:
    case CMO_DMS_GENERIC:
        return dump_cmo_null(array, m);
    case CMO_INT32:
        return dump_cmo_int32(array, (cmo_int32 *)m);
    case CMO_STRING:
        return dump_cmo_string(array, (cmo_string *)m);
    case CMO_MATHCAP:
    case CMO_RING_BY_NAME:
    case CMO_ERROR2:
        return dump_cmo_mathcap(array, (cmo_mathcap *)m);
    case CMO_LIST:
        return dump_cmo_list(array, (cmo_list *)m);
    case CMO_MONOMIAL32:
        return dump_cmo_monomial32(array, (cmo_monomial32 *)m);
    case CMO_ZZ:
        return dump_cmo_zz(array, (cmo_zz *)m);
    case CMO_DISTRIBUTED_POLYNOMIAL:
        return dump_cmo_distributed_polynomial(array, (cmo_distributed_polynomial *)m);
    default:
        return NULL;
    }
}

static char* dump_integer(char* array, int x)
{
    int nx = htonl(x);
    memcpy(array, &nx, sizeof(int));
    return array + sizeof(int);
}

static char* dump_mpz(char* array, mpz_ptr mpz)
{
    int i;
    int len = abs(mpz->_mp_size);
    array = dump_integer(array, mpz->_mp_size);
    for(i=0; i<len; i++) {
        array = dump_integer(array, mpz->_mp_d[i]);
    }
    return array;
}


int send_ox(ox_file_t s, ox *m)
{
    int tag = m->tag;
    int code;
    if (tag == OX_DATA) {
        send_ox_cmo(s->stream, ((ox_data *)m)->cmo);
    }else if (tag == OX_COMMAND) {
        code = ((ox_command *)m)->command;
        if (code >= 1024) {
            /* control command */
            send_ox_command(s->control, code);
        }else {
            send_ox_command(s->stream, code);
        }
    }else {
        /* CMO?? */
        send_ox_cmo(s->stream, (cmo *)m);
    }
}

int send_ox_cmo(int fd, cmo* m)
{
    send_ox_tag(fd, OX_DATA);
    send_cmo(fd, m);
}

/* send_cmo_xxx 関数群 */
static int send_cmo_null(int fd, cmo_null* c)
{
    return 0;
}

static int send_cmo_int32(int fd, cmo_int32* m)
{
    send_int32(fd, m->i);
}

static int send_cmo_string(int fd, cmo_string* m)
{
    int len = strlen(m->s);
    send_int32(fd, len);
    if (len > 0) {
        write(fd, m->s, len);
    }
    return 0;
}

static int send_cmo_mathcap(int fd, cmo_mathcap* c)
{
    send_cmo(fd, c->ob);
    return 0;
}

static int send_cmo_list(int fd, cmo_list* c)
{
    cell* cp = c->head;
    int len = length_cmo_list(c);
    send_int32(fd, len);

    while(cp != NULL) {
        send_cmo(fd, cp->cmo);
        cp = cp->next;
    }
    return 0;
}

static int send_cmo_zz(int fd, cmo_zz* c)
{
    send_mpz(fd, c->mpz);
}

static int send_cmo_error2(int fd, cmo_error2* c)
{
    send_cmo(fd, c->ob);
    return 0;
}

/* CMOを送る.  OX_tag は送信済*/
int send_cmo(int fd, cmo* c)
{
    int tag = c->tag;

    send_int32(fd, tag);
    switch(tag) {
    case CMO_NULL:
        send_cmo_null(fd, c);  /* 空の関数 */
        break;
    case CMO_INT32:
        send_cmo_int32(fd, (cmo_int32 *)c);
        break;
    case CMO_STRING:
        send_cmo_string(fd, (cmo_string *)c);
        break;
    case CMO_MATHCAP:
        send_cmo_mathcap(fd, (cmo_mathcap *)c);
        break;
    case CMO_LIST:
        send_cmo_list(fd, (cmo_list *)c);
        break;
    case CMO_ZZ:
        send_cmo_zz(fd, (cmo_zz *)c);
        break;
    case CMO_ERROR2:
        send_cmo_error2(fd, (cmo_error2 *)c);
        break;
    default:
    }
}

static int send_mpz(int fd, mpz_ptr mpz)
{
    int i;
    int len = abs(mpz->_mp_size);
    send_int32(fd, mpz->_mp_size);
    for(i=0; i<len; i++) {
        send_int32(fd, mpz->_mp_d[i]);
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

char* dump_ox_data(char* array, ox_data* m)
{
    array = dump_integer(array, OX_DATA);
    array = dump_integer(array, -1);
    return dump_cmo(array, m->cmo);
}

char* dump_ox_command(char* array, ox_command* m)
{
    array = dump_integer(array, OX_COMMAND);
    array = dump_integer(array, -1);
    return dump_integer(array, m->command);
}

#define  MAX_TYPES  8
static int known_types[] = {
    -1,   /* gate keeper */
    CMO_NULL,
    CMO_INT32,
    CMO_STRING,
    CMO_MATHCAP,
    CMO_LIST,
    CMO_ZZ,
    CMO_ERROR2,
};

#define ID_TEMP   "(CMO_MATHCAP, (CMO_LIST, (CMO_LIST, (CMO_INT32, %d), (CMO_STRING, \"%s\"), (CMO_STRING, \"%s\"), (CMO_STRING, \"%s\")), (CMO_LIST, (CMO_INT32, 1), (CMO_INT32, 2), (CMO_INT32, 4), (CMO_INT32, 5), (CMO_INT32, 17), (CMO_INT32, 20), (CMO_INT32, 2130706434))))\n"

cmo* make_mathcap_object2(int ver, char* ver_s, char* sysname)
{
    cmo *cap;
    char buff[8192];

    setgetc(mygetc);
    sprintf(buff, ID_TEMP, ver, sysname, ver_s, getenv("HOSTTYPE"));
    setmode_mygetc(buff, 8192);
    cap = parse();
    resetgetc();

    return cap;
}

cmo* make_mathcap_object(int version, char* id_string)
{
    cmo_list *li_ver, *li_cmo, *li;
    
    int i;
    li_ver = new_cmo_list();
    append_cmo_list(li_ver, (cmo *)new_cmo_int32(version));
    append_cmo_list(li_ver, (cmo *)new_cmo_string(id_string));

    li_cmo = new_cmo_list();
    for(i=0; i<MAX_TYPES; i++) {
        if (known_types[i] != -1) {
            append_cmo_list(li_cmo, (cmo *)new_cmo_int32(known_types[i]));
        }
    }

    li = new_cmo_list();
    append_cmo_list(li, (cmo *)li_ver);
    append_cmo_list(li, (cmo *)li_cmo);
    
    return (cmo *)new_cmo_mathcap((cmo *)li);
}

static int funcs(int cmo_type)
{
    int i;
    for(i=0; i<MAX_TYPES; i++) {
        if (known_types[i] == cmo_type) {
            return i;
        }
    }
    return 0;
}

void setCmotypeDisable(int type)
{
    int i = funcs(type);
    known_types[i] = -1;
}
#if 0
cmo* (*received_funcs[])(int fd) = {
    NULL,  /* gate keeper */
    receive_cmo_null,
    receive_cmo_int32,
    receive_cmo_string,
    receive_cmo_mathcap,
    receive_cmo_list,
    receive_cmo_zz,
    receive_cmo_error2
};

cmo* receive_cmo2(int fd)
{
    int tag;
    cmo* (*foo)() = received_funcs[funcs(tag)];
    if (foo != NULL) {
        return foo(fd);
    }
}
#endif

/* ファイルディスクリプタ fd の通信路での integer の byte order を決定する */
/* 実際には order (0,1,or 0xFF)をみてはいない */
int decideByteOrderWithReadPasswd(int fd_read, int fd_write, int order, char* passwd)
{
    char zero = OX_BYTE_NETWORK_BYTE_ORDER;
    char dest;
	read_password(fd_read, passwd);
    write(fd_write, &zero, sizeof(char));
    read(fd_read, &dest, sizeof(char));
    return 0;
}

/* ファイルディスクリプタ fd の通信路での integer の byte order を決定する */
/* 実際には order (0,1,or 0xFF)をみてはいない */
int decideByteOrder(int fd_read, int fd_write, int order)
{
    char zero = OX_BYTE_NETWORK_BYTE_ORDER;
    char dest;
    write(fd_write, &zero, sizeof(char));
    read(fd_read, &dest, sizeof(char));
    return 0;
}

/* Server 側ではこちらを用いる */
int decideByteOrder2(int fd_read, int fd_write, int order)
{
    char zero = OX_BYTE_NETWORK_BYTE_ORDER;
    char dest;
    read(fd_read, &dest, sizeof(char));
    write(fd_write, &zero, sizeof(char));
    return 0;
}

/* cmo と string の変換関数群 */

cmo_zz *new_cmo_zz_set_string(char *s)
{
    cmo_zz* c = new_cmo_zz_noinit();
    mpz_init_set_str(c->mpz, s, 10);
    return c;
}

char *convert_zz_to_cstring(cmo_zz *c)
{
    return mpz_get_str(NULL, 10, c->mpz);
}

char *convert_cmo_to_cstring(cmo *m)
{
    switch(m->tag) {
    case CMO_ZZ:
        return convert_zz_to_cstring((cmo_zz *)m);
    case CMO_INT32:
        return convert_int_to_cstring(((cmo_int32 *)m)->i);
    case CMO_STRING:
        return ((cmo_string *)m)->s;
    case CMO_NULL:
        return convert_null_to_cstring();
    default:
        fprintf(stderr, "sorry, not implemented CMO\n");
        /* まだ実装していません. */
        return NULL;
    }
}

char *convert_null_to_cstring()
{
    static char* null_string = "";
    return null_string;
}

char *convert_int_to_cstring(int integer)
{
    char buff[1024];
    char *s;

    sprintf(buff, "%d", integer);
    s = malloc(strlen(buff)+1);
    strcpy(s, buff);

    return s;
}
