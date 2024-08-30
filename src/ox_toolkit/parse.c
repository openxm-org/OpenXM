/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/parse.c,v 1.17 2015/08/27 03:03:34 ohara Exp $ */

/* 
   This module is a parser for OX/CMO expressions.
   Some comments are written in Japanese by using the EUC-JP coded 
   character set.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <ctype.h>
#if !defined(_MSC_VER)
#include <sys/param.h>
#endif

#include "ox_toolkit.h"
#include "parse.h"

/* --- Parser --- */
/* Remarks for semantics.
   CMO_LIST, CMO_STRING は、あらかじめ与えられた要素の個数を無視する.
   CMO_MONOMIAL32 は無視しない. (つまりおかしいときは構文エラーになる)
*/

/* 
   parse.c では, Lisp 表現された CMO 文字列を読み込み,
   バイト列を出力する.  中間表現として、cmo 構造体を利用する.
   parse() はトークンの列から cmo 構造体を生成し、そのポインタを返す.  
   重要なことはパーサ(の各サブルーチン)は
   常にトークンをひとつ先読みしていると言うことである.
*/

/* 現在読み込み中のトークンを表す. */
static int token = 0;   

/* トークンの属性値. yylval は lex() によってセットされる. */
static union{
    int  d;
    char *sym;
} yylval;

/* 
   If `pflag_cmo_addrev' sets, then we admit extended CMO expressions.
   For example, (CMO_STRING, "hello") is not a real CMO expression
   but it is admitted.
*/
static int pflag_cmo_addrev = 1;

/* definitions of local functions */
static void  init_parser(char *s);
static cmo  *parse();
static void parse_error(char *s);
static void parse_right_parenthesis();
static void parse_left_parenthesis();
static void parse_comma();
static mpz_ptr parse_mpz_integer();
static int   parse_integer();
static char *parse_string();
static cmo  *parse_cmo_null();
static cmo  *parse_cmo_int32();
static cmo  *parse_cmo_string();
static cmo  *parse_cmo_mathcap();
static cmo  *parse_cmo_list();
static cmo  *parse_cmo_monomial32();
static cmo  *parse_cmo_zz();
static cmo  *parse_cmo_zero();
static cmo  *parse_cmo_dms_generic();
static cmo  *parse_cmo_ring_by_name();
static cmo  *parse_cmo_distributed_polynomial();
static cmo  *parse_cmo_indeterminate();
static cmo  *parse_cmo_error2();
static cmo  *parse_cmo();
static int  parse_sm();
static ox   *parse_ox();
static ox   *parse_ox_command();
static ox   *parse_ox_data();
static void init_lex(char *s);
static int  lex();

/* Parsing a Lisp-style expression of CMO. */
cmo *ox_parse_lisp(char *s)
{
    return (s != NULL && strlen(s) > 0)? init_parser(s), parse(): NULL;
}

static int is_token_cmo(int token)
{
    return (token >= MIN_T_CMO && token < MAX_T_CMO) || token == TOKEN(CMO_ERROR2);
}

static int is_token_sm(int token)
{
    return token == TOKEN(SM);
}

static int is_token_ox(int token)
{
    return token >= MIN_T_OX && token < MAX_T_OX;
}

static jmp_buf env_parse;

/* This is a parsing fault. */
static void parse_error(char *s)
{
    ox_printf("syntax error: %s\n", s);
    longjmp(env_parse, 1);
}

static void setflag_parse(int flag)
{
    pflag_cmo_addrev = flag;
}

static void init_parser(char *s)
{
    setflag_parse(PFLAG_ADDREV);
    init_lex(s);
}

static cmo *parse()
{
    cmo *m = NULL;
    if (setjmp(env_parse) == 0) {
        token = lex();
        if (token == '(') {
            token = lex();
            if (is_token_cmo(token)) {
                m = parse_cmo();
            }else if(is_token_ox(token)) {
                m = parse_ox();
            }else {
                parse_error("invalid symbol.");
            }
        }
    }
    return m;
}

static ox *parse_ox()
{
    ox *m = NULL;

    switch(token) {
    case TOKEN(OX_COMMAND):
        token = lex();
        m = parse_ox_command();
        break;
    case TOKEN(OX_DATA):
        token = lex();
        m = parse_ox_data();
        break;
    default:
        parse_error("invalid ox.");
    }
    return m;
}

static ox *parse_ox_data()
{
    ox *m;

    parse_comma();
    parse_left_parenthesis();
    m = (ox *)new_ox_data(parse_cmo());
    parse_right_parenthesis();
    return m;
}

static ox *parse_ox_command()
{
    ox *m;

    parse_comma();
    parse_left_parenthesis();
    m = (ox *)new_ox_command(parse_sm());
    parse_right_parenthesis();
    return m;
}

static int parse_sm()
{
    int sm_code;
    if (token != TOKEN(SM)) {
        parse_error("no opecode.");
    }
    sm_code = yylval.d;
    token = lex();
    parse_right_parenthesis();
    return sm_code;
}

static cmo *parse_cmo()
{
    cmo *m = NULL;

    switch(token) {
    case TOKEN(CMO_NULL):
        token = lex();
        m = parse_cmo_null();
        break;
    case TOKEN(CMO_INT32):
        token = lex();
        m = parse_cmo_int32();
        break;
    case TOKEN(CMO_STRING):
        token = lex();
        m = parse_cmo_string();
        break;
    case TOKEN(CMO_MATHCAP):
        token = lex();
        m = parse_cmo_mathcap();
        break;
    case TOKEN(CMO_LIST):
        token = lex();
        m = parse_cmo_list();
        break;
    case TOKEN(CMO_MONOMIAL32):
        token = lex();
        m = parse_cmo_monomial32();
        break;
    case TOKEN(CMO_ZZ):
        token = lex();
        m = parse_cmo_zz();
        break;
    case TOKEN(CMO_ZERO):
        token = lex();
        m = parse_cmo_zero();
        break;
    case TOKEN(CMO_DMS_GENERIC):
        token = lex();
        m = parse_cmo_dms_generic();
        break;
    case TOKEN(CMO_RING_BY_NAME):
        token = lex();
        m = parse_cmo_ring_by_name();
        break;
    case TOKEN(CMO_DISTRIBUTED_POLYNOMIAL):
        token = lex();
        m = parse_cmo_distributed_polynomial();
        break;
    case TOKEN(CMO_INDETERMINATE):
        token = lex();
        m = parse_cmo_indeterminate();
        break;
    case TOKEN(CMO_ERROR2):
        token = lex();
        m = parse_cmo_error2();
        break;
    default:
        parse_error("invalid cmo.");
    }
    return m;
}

static void parse_left_parenthesis()
{
    if (token != '(') {
        parse_error("no left parenthesis.");
    }
    token = lex();
}

static void parse_right_parenthesis()
{
    if (token != ')') {
        parse_error("no right parenthesis.");
    }
    token = lex();
}

static void parse_comma()
{
    if (token != ',') {
        parse_error("no comma.");
    }
    token = lex();
}

static mpz_ptr new_mpz_set_str(char *s)
{
    mpz_ptr z = MALLOC(sizeof(mpz_t));
    mpz_init_set_str(z, s, 10);
    return z;
}

static mpz_ptr my_mpz_neg(mpz_ptr src)
{
    mpz_ptr z = MALLOC(sizeof(mpz_t));
    mpz_init(z);
    mpz_neg(z, src);
    return z;
}

static mpz_ptr parse_mpz_integer()
{
    int sign = 1;
    mpz_ptr val;

    if (token == '+') {
        token = lex();
    }else if (token == '-') {
        sign = -1;
        token = lex();
    }

    if (token != T_DIGIT) {
        parse_error("no integer.");
    }
    val = new_mpz_set_str(yylval.sym);
    if (sign == -1) {
        val = my_mpz_neg(val);
    }
    token = lex();
    return val;
}

static int parse_integer()
{
#if 0
    return mpz_get_si(parse_mpz_integer());
#else
    int sign = 1;
    int val;

    if (token == '+') {
        token = lex();
    }else if (token == '-') {
        sign = -1;
        token = lex();
    }

    if (token != T_DIGIT) {
        parse_error("no integer.");
    }
    val = sign*atoi(yylval.sym);
    token = lex();
    return val;
#endif
}

static char *parse_string()
{
    char *s;
    if (token != T_STRING) {
        parse_error("no string.");
    }
    s = yylval.sym;
    token = lex();
    return s;
}

static cmo *parse_cmo_null()
{
    parse_right_parenthesis();
    return (cmo *)new_cmo_null();
}

static cmo *parse_cmo_int32()
{
    int z;

    parse_comma();
    z = parse_integer();
    parse_right_parenthesis();
    return (cmo *)new_cmo_int32(z);
}

static cmo *parse_cmo_string()
{
    cmo_string *m;
    char *s;

    parse_comma();
    if (token == T_DIGIT) {
        parse_integer();
        parse_comma();
    }else if (!pflag_cmo_addrev) {
        parse_error("invalid cmo string.");
    }
    s = parse_string();
    m = new_cmo_string(s);
    parse_right_parenthesis();
    return (cmo *)m;
}

static cmo *parse_cmo_mathcap()
{
    cmo *ob;

    parse_comma();
    parse_left_parenthesis();
    ob = parse_cmo();
    parse_right_parenthesis();
    return (cmo *)new_cmo_mathcap(ob);
}

static cmo *parse_cmo_list()
{
    cmo_list *m = new_cmo_list();
    cmo *newcmo;

    if (token == ',') {
        parse_comma();

        if (token == T_DIGIT) {
            parse_integer();
            parse_comma();
        }else if (!pflag_cmo_addrev) {
            parse_error("invalid cmo_list.");
        }

        while(token == '(') {
            parse_left_parenthesis();
            newcmo = parse_cmo();
            list_append(m, newcmo);
            if (token != ',') {
                break;
            }
            parse_comma();
        }
    }else if (!pflag_cmo_addrev) {
        parse_error("invalid cmo_list.");
    }
    parse_right_parenthesis();
    return (cmo *)m;
}

static cmo *parse_cmo_monomial32()
{
    int size;
    int i;
    cmo_monomial32 *m;
    int tag;

    parse_comma();
    size = parse_integer();
    if (size < 0) {
        parse_error("invalid value.");
    }
    m = new_cmo_monomial32_size(size);

    for(i=0; i<size; i++) {
        parse_comma();
        m->exps[i] = parse_integer();
    }
    parse_comma();
    parse_left_parenthesis();
    m->coef = parse_cmo(); 
    tag = m->coef->tag;

    /* semantics: 
       The tag of m->coef must be CMO_ZZ or CMO_INT32. */
    if (tag != CMO_ZZ && tag != CMO_INT32) {
        parse_error("invalid cmo.");
    }
    parse_right_parenthesis();
    return (cmo *)m;
}

/* the following function rewrite internal data of mpz/cmo_zz. */
static cmo *parse_cmo_zz()
{
    int length;
    int i=0;
    cmo_zz *m= NULL;
    mpz_ptr z;

    parse_comma();
    z = parse_mpz_integer();
    if (token == ',') {
        length = mpz_get_si(z);
        m = new_cmo_zz_size(length);
        
        length = abs(length);
        for(i=0; i<length; i++) {
            parse_comma();
            m->mpz->_mp_d[i] = parse_integer();
        }
    }else if (pflag_cmo_addrev) {
        m = new_cmo_zz_set_mpz(z);
    }else {
        parse_error("no comma.");
    }

    parse_right_parenthesis();
    return (cmo *)m;
}

static cmo *parse_cmo_zero()
{
    parse_right_parenthesis();
    return (cmo *)new_cmo_zero();
}

static cmo *parse_cmo_dms_generic()
{
    parse_right_parenthesis();
    return (cmo *)new_cmo_dms_generic();
}

static cmo *parse_cmo_ring_by_name()
{
    cmo *ob;

    parse_comma();
    parse_left_parenthesis();
    ob = parse_cmo();   

    /* The ob has a type of CMO_STRING. */
    if (ob->tag != CMO_STRING) {
        parse_error("invalid cmo.");
    }
    parse_right_parenthesis();
    return (cmo *)new_cmo_ring_by_name(ob);
}

static cmo *parse_cmo_distributed_polynomial()
{
    cmo_distributed_polynomial *m = new_cmo_distributed_polynomial();
    cmo *ob;
    int tag;

    if (token == ',') {
        parse_comma();

        if (token == T_DIGIT) {
            parse_integer();
            parse_comma();
        }else if (!pflag_cmo_addrev) {
            parse_error("invalid d-polynomial.");
        }

        parse_left_parenthesis();
        m->ringdef = parse_cmo();
        tag = m->ringdef->tag;
        /* m->ringdef needs to be a DringDefinition. */
        if (tag != CMO_RING_BY_NAME && tag != CMO_DMS_GENERIC 
            && tag != CMO_DMS_OF_N_VARIABLES) {
            parse_error("invalid cmo.");
        }

        parse_comma();

        while(token == '(') {
            parse_left_parenthesis();
            ob = parse_cmo();
            if (ob->tag != CMO_MONOMIAL32 && ob->tag != CMO_ZERO) {
                parse_error("invalid cmo.");
            }
            list_append((cmo_list *)m, ob);
            if (token != ',') {
                break;
            }
            parse_comma();
        }
    }else if (!pflag_cmo_addrev) {
        parse_error("invalid d-polynomial.");
    }
    parse_right_parenthesis();
    return (cmo *)m;
}

static cmo *parse_cmo_indeterminate()
{
    cmo *ob;

    parse_comma();
    parse_left_parenthesis();
    ob = parse_cmo();
    parse_right_parenthesis();
    return (cmo *)new_cmo_indeterminate(ob);
}

static cmo *parse_cmo_error2()
{
    cmo *ob;

    parse_comma();
    parse_left_parenthesis();
    ob = parse_cmo();
    parse_right_parenthesis();
    return (cmo *)new_cmo_error2(ob);
}

/* --- lexical analyzer --- */

/* A white space is ignored by lexical analyzer. */
static int c = ' ';    

/* getting a character from string. */
static char *mygetc_ptr;
static int mygetc()
{
    return *mygetc_ptr++;
}

static void init_lex(char *s)
{
    c=' ';
    mygetc_ptr=s;
}

#define SIZE_BUFFER  8192
static char buffer[SIZE_BUFFER];

static char *new_string(char *s)
{
    char *t = MALLOC(strlen(s)+1);
    strcpy(t, s);
    return t;
}

/* no measure for buffer overflow */
static char *lex_digit()
{
    static char buff[SIZE_BUFFER];
    int i;

    for(i=0; i<SIZE_BUFFER-1; i++) {
        if(isdigit(c)) {
            buff[i] = c;
        }else {
            buff[i] = '\0';
            return new_string(buff);
        }
        c = mygetc();
    }
    buff[SIZE_BUFFER-1] = '\0';
    return new_string(buff);
}

#define MK_KEY_CMO(x)  { #x , x  , TOKEN(x)  , IS_CMO }
#define MK_KEY_SM(x)   { #x , x  , TOKEN(SM) , IS_SM  }
#define MK_KEY_OX(x)   { #x , x  , TOKEN(x)  , IS_OX  }

static struct symbol symbol_list[] = {
    MK_KEY_CMO(CMO_NULL),
    MK_KEY_CMO(CMO_INT32), 
    MK_KEY_CMO(CMO_DATUM),
    MK_KEY_CMO(CMO_STRING), 
    MK_KEY_CMO(CMO_MATHCAP),
    MK_KEY_CMO(CMO_LIST), 
    MK_KEY_CMO(CMO_MONOMIAL32),
    MK_KEY_CMO(CMO_ZZ),
    MK_KEY_CMO(CMO_ZERO), 
    MK_KEY_CMO(CMO_TREE),
    MK_KEY_CMO(CMO_DMS_GENERIC),
    MK_KEY_CMO(CMO_RING_BY_NAME),
    MK_KEY_CMO(CMO_INDETERMINATE),
    MK_KEY_CMO(CMO_DISTRIBUTED_POLYNOMIAL),
    MK_KEY_CMO(CMO_POLYNOMIAL_IN_ONE_VARIABLE),
    MK_KEY_CMO(CMO_RECURSIVE_POLYNOMIAL),
    MK_KEY_CMO(CMO_ERROR2),
    MK_KEY_SM(SM_popCMO),   
    MK_KEY_SM(SM_popString), 
    MK_KEY_SM(SM_mathcap),  
    MK_KEY_SM(SM_pops), 
    MK_KEY_SM(SM_executeStringByLocalParser), 
    MK_KEY_SM(SM_executeFunction), 
    MK_KEY_SM(SM_setMathCap),
    MK_KEY_SM(SM_shutdown),
    MK_KEY_SM(SM_control_kill), 
    MK_KEY_SM(SM_control_reset_connection),
    MK_KEY_OX(OX_COMMAND),
    MK_KEY_OX(OX_DATA),
    {NULL, 0, 0, 0}        /* a gate keeper */
}; 

symbol_t lookup_by_symbol(char *key)
{
    symbol_t symp;
    for(symp = symbol_list; symp->key != NULL; symp++) {
        if (strcmp(key, symp->key)==0) {
            return symp;
        }
    }
    return NULL;
}

symbol_t lookup_by_token(int tok)
{
    symbol_t symp;
    for(symp = symbol_list; symp->key != NULL; symp++) {
        if (tok == symp->token) {
            return symp;
        }
    }
    return NULL;
}

symbol_t lookup_by_tag(int tag)
{
    symbol_t symp;
    for(symp = symbol_list; symp->key != NULL; symp++) {
        if (tag == symp->tag) {
            return symp;
        }
    }
    return NULL;
}

symbol_t lookup(int i)
{
    return &symbol_list[i];
}

char *get_symbol_by_tag(int tag)
{
    symbol_t symp = lookup_by_tag(tag);
    return (symp != NULL)? symp->key: NULL;
}

/* no measure for buffer overflow */
static char *lex_quoted_string()
{
    int i;
    char c0 = ' ';

    for (i=0; i<SIZE_BUFFER; i++) {
        c = mygetc();
        if(c == '"') {
            c = mygetc();
            buffer[i]='\0';
            return new_string(buffer);
        }else if (c == '\\') {
            c0 = c;
            c = mygetc();
            if (c != '"') {
                buffer[i++] = c0;
            }
        }
        buffer[i]=c;
    }
    ox_printf("buffer overflow!\n");
    exit(1);
    /* return NULL; */
}

static int token_of_symbol(char *key)
{
    symbol_t symp = lookup_by_symbol(key);
    if (symp != NULL) {
        yylval.d = symp->tag;
        return symp->token;
    }
    ox_printf("lex error:: \"%s\" is unknown symbol.\n", key);
    return 0;
}

static int lex_symbol()
{
    int i;
    for (i=0; i<SIZE_BUFFER; i++) {
        if (!isalnum(c) && c != '_') {
            buffer[i]='\0';
            return token_of_symbol(buffer);
        }
        buffer[i]=c;
        c = mygetc();
    }
    ox_printf("buffer overflow!\n");
    return 0;
}

/* Remark: prefetching a character before return. */
static int lex()
{
    int c_dash = 0;
  
    /* white spaces are ignored. */
    while (isspace(c)) {
        c = mygetc();
    }

    switch(c) {
    case '(':
    case ')':
    case ',':
    case '+':
    case '-':
        c_dash = c;
        c = ' ';
        return c_dash;
    case EOF:
        c = mygetc();
        return c_dash;
    case '"':      /* a quoted string! */
        yylval.sym = lex_quoted_string();
        return T_STRING;
    default:
		;
    }

    if (isalpha(c)) {
        /* symbols */
        return lex_symbol();
    }

    /* digit */
    if (isdigit(c)){
        yylval.sym = lex_digit();
        return T_DIGIT;
    }
    c = mygetc();
    return 0;
}
