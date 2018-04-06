/* $OpenXM: OpenXM/src/ox_gsl/ox_eval.c,v 1.3 2018/04/05 13:02:39 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "ox_toolkit.h"

/*
Usage:

double d;
replace(3,"x",1.25,"y",-2.0, "z", 2.1);
if(eval_cmo(your_cmo_tree,&d)==0) goto_error();
*/

#define FUNCTION_P(e)      (((e)!=NULL) && ((e)->f != NULL))
#define VALUE_P(e)         (((e)!=NULL) && ((e)->f == NULL))

#define FAILED  0
#define SUCCEED 1

void replace(int n, ...);
void replace2(int n, char *s[], double v[]);
int eval_cmo(cmo *c, double *retval);

static double op_add(double x,double y)
{
    return x+y;
}

static double op_sub(double x,double y)
{
    return x-y;
}

static double op_mul(double x,double y)
{
    return x*y;
}

static double op_div(double x,double y)
{
    return x/y;
}

static double op_negative(double x) 
{
    return -x;
}

/* 定数は引数なしの関数として実現する。*/
typedef struct {
    char *name;
    double v;
    double (*f)();
    int n_args; /* number of args */
} entry;

/* 
グローバル辞書: sin(x), cos(x), exp(x), lgamma(x) などの関数や、pi,e といった定数を扱うための辞書
(グローバル変数で保持、システム固有)

ローカル辞書: [x -> 1/2, y-> 0.5] などの置き換えのため。
*/
entry global_dic[512] = {
    {"sin",0,sin,1},
    {"cos",0,cos,1},
    {"tan",0,tan,1},
    {"sinh",0,sinh,1},
    {"cosh",0,cosh,1},
    {"tanh",0,tanh,1},
    {"asin",0,asin,1},
    {"acos",0,acos,1},
    {"asinh",0,asinh,1},
    {"acosh",0,acosh,1},
    {"erf",0,erf,1},
    {"exp",0,exp,1},
    {"exp2",0,exp2,1},
    {"log",0,log,1},
    {"log2",0,log2,1},
    {"log10",0,log10,1},
    {"gamma",0,gamma,1},
    {"lgamma",0,lgamma,1},
    {"sqrt",0,sqrt,1},
    {"cbrt",0,cbrt,1},
    {"fabs",0,fabs,1},
    {"j0",0,j0,1},
    {"j1",0,j1,1},
    {"y0",0,y0,1},
    {"y1",0,y1,1},
    {"-",  0,op_negative,1},
    {"+",  0,op_add,2},
    {"-",  0,op_sub,2},
    {"*",  0,op_mul,2},
    {"/",  0,op_div,2},
    {"^",  0,pow,2},
    {"pow",  0,pow,2},
    {"@e",  M_E, NULL,0},
    {"@pi", M_PI,NULL,0},
    {NULL,0,NULL,0}
};

#define LOCAL_DIC_SIZE 256
static entry local_dic[LOCAL_DIC_SIZE] = {
	{NULL,0,NULL,0}
};
static int local_dic_counter;

int register_entry(char *s, double v)
{
    entry *e = &local_dic[local_dic_counter];
    if(local_dic_counter<LOCAL_DIC_SIZE-1) {
        e->name = s;
        e->v = v;
		e->f = NULL;
        local_dic_counter++;
        return 1;
    }
    return 0;
}

void init_dic()
{
    int i;
    for(i=0; i<local_dic_counter; i++) {
        free((local_dic+i)->name);
    }
    local_dic_counter=0;
    memset(local_dic, 0, sizeof(entry)*LOCAL_DIC_SIZE);
}

void replace(int n, ...) 
{
    char *s;
    double d;
    va_list ap;
    va_start(ap,n);
    for(init_dic(); n>0; n--) {
        s = va_arg(ap, char *);
        d = va_arg(ap, double);
        register_entry(s,d);
    }
    va_end(ap);
}

void replace2(int n, char *s[], double v[]) 
{
    int i;
    init_dic();
    for(i=0; i<n; i++) {
        register_entry(s[i],v[i]);
    }
}

static entry *find_entry(cmo *node, int len, entry *dic)
{
    char *s;
    entry *e;
    if(node->tag == CMO_STRING) {
        s = ((cmo_string *)node)->s;
    }else if(node->tag == CMO_INDETERMINATE) {
        s = cmo_indeterminate_get_name((cmo_indeterminate *)node);
    }else {
        return NULL;
    }
    for(e=dic; e->name != NULL; e++) {
        if(strcmp(e->name,s)==0 && (len<0 || len==e->n_args)) {
            return e;
        }
    }
    return NULL;
}

static int entry_function(entry *e, cmo_list *args, double *retval)
{
	int i,m,n;
	double *dargs;
	n=e->n_args;
	if(n<0 || n>list_length(args)) {
		/* arguments are mismatched */
		return 0;
	}
	if(n>5) {
		/* too many arguments */
		return 0;
	}
	dargs = (double *)alloca(n*sizeof(double));
	for(i=0; i<n; i++) {
		if(eval_cmo(list_nth(args, i), &dargs[i])==0) {
			return 0;
		}
	}
	switch(n) {
	case 0:
		*retval = e->f();
		break;
	case 1:
		*retval = e->f(dargs[0]);
		break;
	case 2:
		*retval = e->f(dargs[0],dargs[1]);
		break;
	case 3:
		*retval = e->f(dargs[0],dargs[1],dargs[2]);
		break;
	case 4:
		*retval = e->f(dargs[0],dargs[1],dargs[2],dargs[3]);
		break;
	case 5:
		*retval = e->f(dargs[0],dargs[1],dargs[2],dargs[3],dargs[4]);
		break;
	default:
		return 0;
	}
	return 1;
}

int entry_value(entry *e, double *retval)
{
	*retval = e->v;
	return 1;
}

/* 
返り値: 評価が成功したか、 1: 成功, 0: 失敗
評価された値: *retval に格納
*/
static int eval_cmo_tree(cmo_tree* t, double *retval)
{
    entry *e = find_entry((cmo *)t->name,list_length(t->leaves),global_dic);
    if (FUNCTION_P(e)) {
        return entry_function(e,t->leaves,retval);
    }else if (VALUE_P(e)) {
        return entry_value(e, retval);
    }
    return 0;
}

static int eval_cmo_indeterminate(cmo_indeterminate *c, double *retval)
{
    entry *e = find_entry((cmo *)c,-1,local_dic);
    if (VALUE_P(e)) {
        return entry_value(e,retval);
    }
    return 0;
}

static double mypow(double x, int n)
{
    int i,k,f=0;
    double s,t;
    if (n==0) {
        return 1;
    }else if (n<0) {
        n=-n;
        f=1;
    }
    /* t=x^(2^i) */
    s=1;
    t=x;
    for(k=n; k!=0; k=k>>1) {
        if(k&1) {
            s*=t;
        }
        t=t*t;
    }
    if (f>0) {
        s = 1/s;
    }
    return s;
}

static int eval_cmo_polynomial_in_one_variable(cmo_polynomial_in_one_variable* c, double vars[], int n, double *retval)
{
    int i;
    cell *cc;
    double r,s=0;
    double x = vars[c->var];
    double *d=(double *)calloc(c->length, sizeof(double));
    for(i=0; i<c->length; i++) {
        cc = list_nth_cell((cmo_list *)c, i);
        if (cc->cmo->tag == CMO_POLYNOMIAL_IN_ONE_VARIABLE) {
            if(eval_cmo_polynomial_in_one_variable((cmo_polynomial_in_one_variable*)cc->cmo,vars,n,&r)==0) {
                return 0;
            }
        }else {
            if(eval_cmo(cc->cmo,&r)==0) {
                return 0;
            }
        }
        s += mypow(x,cc->exp)*r;
    }
    *retval = s;
    return 1;
}

static int eval_cmo_recursive_polynomial(cmo_recursive_polynomial* c, double *retval)
{
	int i,n;
	double *vars;
	entry *e;
	switch(c->coef->tag) {
	case CMO_POLYNOMIAL_IN_ONE_VARIABLE:
		n=list_length(c->ringdef);
		if(local_dic_counter<n) {
			return 0; /* 自由変数が残る */
		}
		vars=(double *)calloc(n,sizeof(double));
		for(i=0; i<n; i++) {
			e = find_entry(list_nth(c->ringdef,i),-1,local_dic);
			if(e == NULL) {
				free(vars);
				return 0; /* failed */
			}
			entry_value(e, &vars[i]);
		}
		return eval_cmo_polynomial_in_one_variable((cmo_polynomial_in_one_variable*)c->coef,vars,n,retval);
	case CMO_DISTRIBUTED_POLYNOMIAL:
		return 0; /* failed */
	default: /* cmo_zz, cmo_qq, cmo_double, ... */
		return eval_cmo(c->coef,retval);
	}
}

int eval_cmo(cmo *c, double *retval)
{
    int tag = c->tag;
    switch(c->tag) {
    case CMO_ZERO:
        *retval = 0;
        break;
    case CMO_INT32:
        *retval = (double)((cmo_int32 *)c)->i;
        break;
    case CMO_ZZ:
        *retval = mpz_get_d(((cmo_zz *)c)->mpz);
        break;
    case CMO_QQ:
        *retval = mpq_get_d(((cmo_qq *)c)->mpq);
        break;
    case CMO_IEEE_DOUBLE_FLOAT:
    case CMO_64BIT_MACHINE_DOUBLE:
        *retval = ((cmo_double *)c)->d;
        break;
    case CMO_BIGFLOAT:
        *retval = mpfr_get_d(((cmo_bf *)c)->mpfr,GMP_RNDN);
        break;
    case CMO_TREE:
        return eval_cmo_tree((cmo_tree *)c,retval);
        break;
	case CMO_INDETERMINATE:
        return eval_cmo_indeterminate((cmo_indeterminate *)c,retval);
        break;
	case CMO_RECURSIVE_POLYNOMIAL:
        return eval_cmo_recursive_polynomial((cmo_recursive_polynomial *)c,retval);
        break;
    default:
        /* 変換できない型 */
        return 0; /* error */
    }
    return 1;
}
