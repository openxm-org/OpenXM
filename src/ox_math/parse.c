/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */
/* $Id$ */
/* OX expression, CMO expression パーサ */

/* cmo_addrev がセットされていれば、
   厳密には CMO expression でないもの, 例えば 
   (CMO_STRING, "abcdef") も CMO に変換される. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "oxtag.h"
#include "ox.h"
#include "parse.h"

/* --- 構文解析部 --- */
/* parse.c, lex.c では, Lisp 表現された CMO 文字列を読み込み,
   バイト列を出力する.  中間表現として、cmo *を利用する.
   parse() はトークンの列から cmo *(の指す構造体)を生成する.  
*/

/* 重要なことはパーサ(の各サブルーチン)は
   常にトークンをひとつ先読みしていると言うことである.
*/

/* 現在読み込み中のトークンを表す. */
static int token = 0;   

/* yylval は lex() によってセットされる. */
extern lex_value_t yylval;

int cmo_addrev = 1;  /* CMO の省略記法を許すか否かのフラグ */

/* 関数の宣言 */
static int parse_error(char *s);
static int parse_lf();
static int parse_right_parenthesis();
static int parse_left_parenthesis();
static int parse_comma();
static int parse_integer();
static cmo *parse_cmo_null();
static cmo *parse_cmo_zz();
static cmo *parse_cmo_list();
static cmo *parse_cmo_int32();
static cmo *parse_cmo_string();
static cmo *parse_cmo();
static char *parse_string();
static int parse_sm();
static ox* parse_ox();
static ox* parse_ox_command();
static ox* parse_ox_data();

static int parse_error(char *s)
{
    if (s != NULL) {
        fprintf(stderr, "%s\n", s);
    }else {
        fprintf(stderr, "syntax error.\n");
    }
    exit(1);  /* 例外処理.  本当は longjmp すべきであろう. */
}

#define MIN_T_CMO      (T_MAGIC + 0)
#define MAX_T_CMO      (T_MAGIC + 256)

static int is_t_cmo(int token)
{
    return (token >= MIN_T_CMO && token < MAX_T_CMO) || token == T_CMO_ERROR2;
}

#define MIN_T_SM      (T_MAGIC + 256)
#define MAX_T_SM      (T_MAGIC + 1100)

static int is_t_sm(int token)
{
    return token >= MIN_T_SM && token < MAX_T_SM;
}

#define MIN_T_OX      (T_MAGIC + 512)
#define MAX_T_OX      (T_MAGIC + 600)

static int is_t_ox(int token)
{
    return token >= MIN_T_OX && token < MAX_T_OX;
}

/* この部分は書き換え予定. */
cmo *parse()
{
    cmo *m;

    do{
        token = lex();
    }while (token == '\n');

    if (token == '(') {
        token = lex();
        if (is_t_cmo(token)) {
            m = parse_cmo();
        }else if(is_t_ox(token)) {
            m = parse_ox();
        }else {
            parse_error("syntax error: unknown keyword.");
            return NULL;
        }
        parse_lf();
        return m;
    }
    return NULL;
}

/* トークンを先読みしない(重要). */
static int parse_lf()
{
    if (token != '\n') {
        parse_error("syntax error: not new line.");
    }
    return 0;
}


static ox* parse_ox()
{
    ox *m = NULL;

    switch(token) {
    case T_OX_COMMAND:
        token = lex();
        m = parse_ox_command();
        break;
    case T_OX_DATA:
        token = lex();
        m = parse_ox_data();
        break;
    default:
        parse_error("syntax error: invalid ox_tag.");
    }
    return m;
}

static ox* parse_ox_data()
{
    ox* m;

    parse_comma();
    parse_left_parenthesis();
    m = (ox *)new_ox_data(parse_cmo());
    parse_right_parenthesis();
    return m;
}

static int parse_sm()
{
    int sm_code;
    if (!is_t_sm(token)) {
        parse_error("syntax error: invalid sm code.");
    }
    sm_code = token - T_MAGIC;
    token = lex();
    return sm_code;
}


static ox* parse_ox_command()
{
    ox* m;

    parse_comma();
    m = (ox *)new_ox_command(parse_sm());
    parse_right_parenthesis();
    return m;
}

/* 正しい入力ならば, parse_cmo を呼ぶ時点で, token には 
   T_CMO_xxx, T_OX_xxx のいずれかがセットされている. */
static cmo *parse_cmo()
{
    cmo *m = NULL;

    switch(token) {
    case T_CMO_NULL:
        token = lex();
        m = parse_cmo_null();
        break;
    case T_CMO_INT32:
        token = lex();
        m = parse_cmo_int32();
        break;
    case T_CMO_STRING:
        token = lex();
        m = parse_cmo_string();
        break;
    case T_CMO_ZZ:
        token = lex();
        m = parse_cmo_zz();
        break;
    case T_CMO_LIST:
        token = lex();
        m = parse_cmo_list();
        break;
    default:
        parse_error("syntax error: invalid cmo_tag.");
    }
    return m;
}

static int parse_right_parenthesis()
{
    if (token != ')') {
        parse_error("syntax error: no right parenthesis exists.");
        return 0;
    }
    token = lex();
}

static int parse_left_parenthesis()
{
    if (token != '(') {
        parse_error("syntax error: no left parenthesis exists.");
        return 0;
    }
    token = lex();
}

static int parse_comma()
{
    if (token != ',') {
        parse_error("syntax error: no comma exists.");
        return 0;
    }
    token = lex();
    return 1;
}

/* cmo_zz の内部を直接いじる. */
static cmo *parse_cmo_zz()
{
    int length;
    int i=0;
    cmo_zz *m= NULL;

    parse_comma();
    length = parse_integer();
    if (token == ',') {
        m = new_cmo_zz_size(length);
        
        length = abs(length);
        for(i=0; i<length; i++) {
            parse_comma();
            m->mpz->_mp_d[i] = parse_integer();
        }
    }else if (cmo_addrev) {
        m = new_cmo_zz_set_si(length);
    }else {
        parse_error("syntax error: invalid keyword.");
    }

    parse_right_parenthesis();
    return (cmo *)m;
}

static cmo *parse_cmo_list()
{
    int length=0;
    int i=0;
    cmo_list *m;
    cmo *newcmo;

    parse_comma();

    length = parse_integer();
    m = new_cmo_list();
    if (length<0) {
        parse_error("semantics error: a list has negative length.");
    }
    
    for(i=0; i<length; i++) {
        parse_comma();
        parse_left_parenthesis();
        newcmo = parse_cmo();
        append_cmo_list(m, newcmo);
    }
    parse_right_parenthesis();
    return (cmo *)m;
}

static int parse_integer()
{
    int val;
    if (token != T_INTEGER) {
        parse_error("syntax error: in parse_integer().");
    }
    val = yylval.d;
    token = lex();
    return val;
}

static char *parse_string()
{
    char *s;
    if (token != T_STRING) {
        parse_error("syntax error: in parse_string().");
    }
    s = yylval.sym;
    token = lex();
    return s;
}

static cmo *parse_cmo_null()
{
    cmo_null *m = new_cmo_null();
    parse_right_parenthesis();
    return (cmo *)m;
}

static cmo *parse_cmo_int32()
{
    cmo_int32 *m;
    int i;

    parse_comma();
    i = parse_integer();
    m = new_cmo_int32(i);
    parse_right_parenthesis();
    return (cmo *)m;
}

static cmo *parse_cmo_string()
{
    cmo_string *m;
    int length;
    char *s;

    parse_comma();
    if (token == T_INTEGER) {
        length = parse_integer();
        parse_comma();
        s = parse_string();
        if (length != strlen(s)) {
          fprintf(stderr, "warning: strlen unmatched.\n");
        }
    }else if (cmo_addrev) {
        s = parse_string();
    }else {
        parse_error("syntax error: invalid keyword.");
    }
    m = new_cmo_string(s);
    parse_right_parenthesis();
    return (cmo *)m;
}

/* --- 字句解析部 --- */

lex_value_t yylval;

/* lexical analyzer で読み飛ばされる文字なら何を初期値にしてもよい */
static int c = ' ';    

/* 一文字読み込む関数 */
static int (*GETC)() = getchar;

int setgetc(int (*foo)())
{
    GETC = foo;
}

int resetgetc()
{
    GETC = getchar;
}

#define SIZE_BUFFER  1024
static char buffer[SIZE_BUFFER];
static char* PARS = "(),\n";

/* 桁溢れの場合の対策はない */
static int lex_digit()
{
    int d = 0;
    do {
        d = 10*d + (c - '0');
        c = GETC();
    } while(isdigit(c));
    return d;
}

/* バッファあふれした場合の対策をちゃんと考えるべき */
static char *lex_quoted_string()
{
    int i;
    char c0 = ' ';
    char *s = NULL;
    for (i=0; i<SIZE_BUFFER; i++) {
        c = GETC();
        if(c == '"') {
            s = malloc(i+1);
            buffer[i]='\0';
            strcpy(s, buffer);
      
            c = GETC();
            return s;
        }else if (c == '\\') {
            c0 = c;
            c = GETC();
            if (c != '"') {
                buffer[i++] = c0;
            }
        }
        buffer[i]=c;
    }
    fprintf(stderr, "buffer overflow!\n");
    exit(1);
    /* return NULL; */
}

/* キーワードを増やしたあと修正するのを忘れてはいけない */
#undef NUM_OF_KEYWORDS  18

static char *keywords[] = {
    "CMO_INT32", "CMO_STRING", "CMO_LIST", "CMO_ZZ", "CMO_NULL",
    "CMO_ZERO", "CMO_DATUM", 
    "SM_popCMO", "SM_popString", "SM_mathcap", "SM_pops",
    "SM_executeStringByLocalParser", "SM_executeFunction",
    "SM_setMathcap",
    "SM_control_kill", "SM_control_reset_connection",
    "OX_COMMAND", "OX_DATA",
    NULL  /* a gate keeper */
};

static int token_of_keyword[] = {
    T_CMO_INT32, T_CMO_STRING, T_CMO_LIST, T_CMO_ZZ, T_CMO_NULL,
    T_CMO_ZERO, T_CMO_DATUM,
    T_SM_popCMO, T_SM_popString, T_SM_mathcap, T_SM_pops,
    T_SM_executeStringByLocalParser, T_SM_executeFunction,
    T_SM_setMathcap,
    T_SM_control_kill, T_SM_control_reset_connection,
    T_OX_COMMAND, T_OX_DATA,
    0     /* dummy */
};

static int token_of_matched_keyword(char *key)
{
    int i;

    for(i=0; keywords[i] != NULL; i++) {
        if(strcmp(key, keywords[i])==0) {
            return token_of_keyword[i];
        }
    }
    fprintf(stderr, "lex error\n");
    return 0;
}

static int lex_keyword()
{
    int i;
    for (i=0; i<SIZE_BUFFER; i++) {
        if (!isalnum(c) && c != '_') {
            buffer[i]='\0';
            return token_of_matched_keyword(buffer);
        }
        buffer[i]=c;
        c = GETC();
    }
    fprintf(stderr, "buffer overflow!\n");
    exit(1);
}

/* return する前に一文字先読みしておく。 */
int lex()
{
    int c_dash = 0;
  
    /* 空白をスキップする. */
    while (isspace(c) && c != '\n') {
        c = GETC();
    }

    switch(c) {
    case '(':
    case ')':
    case ',':
    case '\n':
        c_dash = c;
        c = ' ';
        return c_dash;
    case EOF:
        c = GETC();
        return c_dash;
    case '"':      /* a quoted string! */
        yylval.sym = lex_quoted_string();
        return T_STRING;
    default:
    }

    if (isalpha(c)) {    /* 識別子 */
        return lex_keyword();
    }

    /* 32bit 整数値 */
    if (isdigit(c)){
        yylval.d = lex_digit();
        return T_INTEGER;
    }
    if (c == '-') {
        c = GETC();
        while (isspace(c) && c != '\n') {
            c = GETC();
        }
        if (isdigit(c)){
            yylval.d = - lex_digit();
            return T_INTEGER;
        }
        return 0;
    }

    c = GETC();
    return 0;
}

static char *mygetc_line;
static int  mygetc_counter;
static int  mygetc_counter_max;

int mygetc()
{
    if (mygetc_counter <= mygetc_counter_max) {
        return mygetc_line[mygetc_counter++];
    }
    return 0;
}

int setmode_mygetc(char *s, int len)
{
    mygetc_counter=0;
    mygetc_counter_max=len;
    mygetc_line=s;
}
