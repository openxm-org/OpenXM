/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/parse.c,v 1.5 1999/11/03 10:56:40 ohara Exp $ */

/* OX expression, CMO expression パーサ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <setjmp.h>
#include "oxtag.h"
#include "ox.h"
#include "parse.h"

/* --- 構文解析部 --- */
/* (重要)セマンティックスについての注意.
   CMO_LIST, CMO_STRING は、あらかじめ与えられた要素の個数を無視する.
   CMO_MONOMIAL32 は無視しない. (つまりおかしいときは構文エラーになる)
*/

/* parse.c, lex.c では, Lisp 表現された CMO 文字列を読み込み,
   バイト列を出力する.  中間表現として、cmo 構造体を利用する.
   parse() はトークンの列から cmo 構造体を生成し、そのポインタを返す.  
*/

/* 重要なことはパーサ(の各サブルーチン)は
   常にトークンをひとつ先読みしていると言うことである.
*/

/* 現在読み込み中のトークンを表す. */
static int token = 0;   

/* トークンの属性値. yylval は lex() によってセットされる. */
static union{
    int  d;
    char *sym;
} yylval;

/* pflag_cmo_addrev がセットされていれば、厳密には CMO expression では
   ないもの, 例えば (CMO_STRING, "hello") も CMO に変換される. */

static int pflag_cmo_addrev = 1;  /* CMO の省略記法を許すか否かのフラグ */

/* 関数の宣言 */
static int  parse_error(char *s);
static int  parse_lf();
static int  parse_right_parenthesis();
static int  parse_left_parenthesis();
static int  parse_comma();
static int  parse_integer();
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

/* 構文解析に失敗したことを意味する. */
static int parse_error(char *s)
{
	fprintf(stderr, "syntax error: %s\n", s);
	longjmp(env_parse, 1);
}

/* この部分は書き換え予定. */
cmo *parse()
{
    cmo *m;

	if (setjmp(env_parse) != 0) {
		return NULL; /* 構文解析に失敗したら NULL を返す. */
	}

    do{
        token = lex();
    }while (token == '\n');

    if (token == '(') {
        token = lex();
        if (is_token_cmo(token)) {
            m = parse_cmo();
        }else if(is_token_ox(token)) {
            m = parse_ox();
        }else {
            parse_error("invalid symbol.");
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
        parse_error("no new line.");
    }
    return 0;
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


/* 正しい入力ならば, parse_cmo を呼ぶ時点で, token には 
   TOKEN(CMO_xxx), TOKEN(OX_xxx) のいずれかがセットされている. */
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

static int parse_left_parenthesis()
{
    if (token != '(') {
        parse_error("no left parenthesis.");
    }
    token = lex();
}

static int parse_right_parenthesis()
{
    if (token != ')') {
        parse_error("no right parenthesis.");
    }
    token = lex();
}

static int parse_comma()
{
    if (token != ',') {
        parse_error("no comma.");
    }
    token = lex();
}

static int parse_integer()
{
    int val;
    if (token != T_INTEGER) {
        parse_error("no integer.");
    }
    val = yylval.d;
    token = lex();
    return val;
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
    int i;

    parse_comma();
    i = parse_integer();
    parse_right_parenthesis();
    return (cmo *)new_cmo_int32(i);
}

static cmo *parse_cmo_string()
{
    cmo_string *m;
    char *s;

    parse_comma();
    if (token == T_INTEGER) {
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
    int length=0;
    int i=0;
    cmo_list *m = new_cmo_list();
    cmo *newcmo;

	if (token == ',') {
		parse_comma();

		if (token == T_INTEGER) {
			parse_integer();
			parse_comma();
		}else if (!pflag_cmo_addrev) {
			parse_error("invalid cmo_list.");
		}

		while(token == '(') {
			parse_left_parenthesis();
			newcmo = parse_cmo();
			append_cmo_list(m, newcmo);
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
	int *exps;
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

    /* m->coef は CMO_ZZ 型か CMO_INT32 型でなければならない */
	if (tag != CMO_ZZ && tag != CMO_INT32) {
		parse_error("invalid cmo.");
	}
    parse_right_parenthesis();
    return (cmo *)m;
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
    }else if (pflag_cmo_addrev) {
        m = new_cmo_zz_set_si(length);
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

    /* ob は CMO_STRING 型でなければならない */
	if (ob->tag != CMO_STRING) {
		parse_error("invalid cmo.");
	}
    parse_right_parenthesis();
    return (cmo *)new_cmo_ring_by_name(ob);
}

static cmo *parse_cmo_distributed_polynomial()
{
    int length=0;
    int i=0;
    cmo_distributed_polynomial *m = new_cmo_distributed_polynomial();
    cmo *ob;
	int tag;

	if (token == ',') {
		parse_comma();

		if (token == T_INTEGER) {
			parse_integer();
			parse_comma();
		}else if (!pflag_cmo_addrev) {
			parse_error("invalid d-polynomial.");
		}

		parse_left_parenthesis();
		m->ringdef = parse_cmo();
		tag = m->ringdef->tag;
		/* m->ringdef は DringDefinition でなければならない */
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
			append_cmo_list((cmo_list *)m, ob);
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

/* --- 字句解析部 --- */

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

#define SIZE_BUFFER  8192
static char buffer[SIZE_BUFFER];

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

typedef struct {
	char *key;
	int  tag;
	int  token;
} symbol; 

#define MK_KEY_CMO(x)  { #x , x  , TOKEN(x)  }
#define MK_KEY_SM(x)   { #x , x  , TOKEN(SM) }
#define MK_KEY(x)      { #x , x  , TOKEN(x)  }

static symbol symbol_list[] = {
	MK_KEY_CMO(CMO_NULL),
    MK_KEY_CMO(CMO_INT32), 
	MK_KEY_CMO(CMO_DATUM),
	MK_KEY_CMO(CMO_STRING), 
	MK_KEY_CMO(CMO_MATHCAP),
	MK_KEY_CMO(CMO_LIST), 
	MK_KEY_CMO(CMO_MONOMIAL32),
	MK_KEY_CMO(CMO_ZZ),
	MK_KEY_CMO(CMO_ZERO), 
	MK_KEY_CMO(CMO_DMS_GENERIC),
	MK_KEY_CMO(CMO_RING_BY_NAME),
	MK_KEY_CMO(CMO_INDETERMINATE),
	MK_KEY_CMO(CMO_DISTRIBUTED_POLYNOMIAL),
	MK_KEY_CMO(CMO_ERROR2),
    MK_KEY_SM(SM_popCMO),   
	MK_KEY_SM(SM_popString), 
	MK_KEY_SM(SM_mathcap),  
	MK_KEY_SM(SM_pops), 
	MK_KEY_SM(SM_executeStringByLocalParser), 
	MK_KEY_SM(SM_executeFunction), 
	MK_KEY_SM(SM_setMathCap),
    MK_KEY_SM(SM_control_kill), 
	MK_KEY_SM(SM_control_reset_connection),
    MK_KEY(OX_COMMAND),
	MK_KEY(OX_DATA),
	{NULL, 0, 0}        /* a gate keeper */
}; 

static int token_of_symbol(char *key)
{
	symbol *kp;
	for(kp = symbol_list; kp->key != NULL; kp++) {
		if (strcmp(key, kp->key)==0) {
			yylval.d = kp->tag;
			return kp->token;
		}
	}
#if DEBUG
    fprintf(stderr, "lex error\n");
#endif
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
        c = GETC();
    }
    fprintf(stderr, "buffer overflow!\n");
	return 0;
}

/* return する前に一文字先読みしておく. */
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
        return lex_symbol();
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

/* 一文字読み込む関数 */
static char *mygetc_line;
static int  mygetc_counter;
static int  mygetc_counter_max;
static int  mygetc_nonlf_flag;

int mygetc()
{
	int c = '\0';

	if (mygetc_nonlf_flag && mygetc_counter <= mygetc_counter_max) {
		c = mygetc_line[mygetc_counter++];
		if (c == '\0') {
			c = '\n';
			mygetc_nonlf_flag = 0;
		}
    }
    return c;
}

int setmode_mygetc(char *s, int len)
{
	mygetc_nonlf_flag=1;
    mygetc_counter=0;
    mygetc_counter_max=len;
    mygetc_line=s;
}

int setflag_parse(int flag)
{
	pflag_cmo_addrev = flag;
}
