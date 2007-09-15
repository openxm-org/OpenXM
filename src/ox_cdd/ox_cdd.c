/*	$OpenXM: OpenXM/src/ox_cdd/ox_cdd.c,v 1.2 2007/09/12 07:07:36 noro Exp $	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined GMPRATIONAL
#include "gmp.h"
#define  __GMP_FAKE_H__
#else
#define dd_almostzero  1.0E-7
#include <math.h>
#endif

#include "ox_toolkit.h"

#if defined GMPRATIONAL
typedef mpq_t mytype;
#else /* built-in C double */
typedef double mytype[1];
#endif

typedef enum {
	dd_LPnone=0, dd_LPmax, dd_LPmin
} dd_LPObjectiveType;

OXFILE *fd_rw;

#define INIT_S_SIZE 2048
#define EXT_S_SIZE  2048

static int stack_size = 0;
static int stack_pointer = 0;
static cmo **stack = NULL;
extern int debug_print;

void init_cdd(void);
int **redcheck(int row,int col,int **matrix,int *presult_row);
mytype *lpsolve(dd_LPObjectiveType type,int row,int col,int **matrix,int *obj);
mytype *lpintpt(int row, int col, int **matrix);


void dprint( int level, char *string )
{
	if( debug_print >= level ){
		printf( string );
		fflush( stdout );
	}
}

#define LP_Q 0
#define LP_I 1
#define LP_MAX 0
#define LP_MIN 1
#define LP_MAXMIN 2
#define LP_INTPT 3

int initialize_stack()
{
	stack_pointer = 0;
 	stack_size = INIT_S_SIZE;
	stack = MALLOC(stack_size*sizeof(cmo*));
	return 0;
}

static int extend_stack()
{
	int size2 = stack_size + EXT_S_SIZE;
	cmo **stack2 = MALLOC(size2*sizeof(cmo*));
	memcpy(stack2, stack, stack_size*sizeof(cmo *));
	free(stack);
	stack = stack2;
	stack_size = size2;
	return 0;
}

int push(cmo* m)
{
 	stack[stack_pointer] = m;
	stack_pointer++;
	if(stack_pointer >= stack_size) {
		extend_stack();
	}
	return 0;
}

cmo* pop()
{
	if(stack_pointer > 0) {
		stack_pointer--;
		return stack[stack_pointer];
	}
	return new_cmo_null();
}

void pops(int n)
{
	stack_pointer -= n;
	if(stack_pointer < 0) {
		stack_pointer = 0;
	}
}

#define OX_CDD_VERSION 0x11121400
#define ID_STRING  "2005/02/14 06:50:00"

int sm_mathcap()
{
	mathcap_init(OX_CDD_VERSION, ID_STRING, "ox_cdd", NULL, NULL);
	push((cmo*)oxf_cmo_mathcap(fd_rw));
	return 0;
}

int sm_popCMO()
{
	cmo* m = pop();

	if(m != NULL) {
		send_ox_cmo(fd_rw, m);
		return 0;
	}
	return SM_popCMO;
}

cmo_error2 *make_error2(int code)
{
	return (cmo_error2 *) new_cmo_int32(-1);
}

int get_i()
{
	cmo *c = pop();
	if(c->tag == CMO_INT32) {
		return ((cmo_int32 *)c)->i;
	}else if(c->tag == CMO_ZZ) {
		return mpz_get_si(((cmo_zz *)c)->mpz);
	}
	make_error2(-1);
	return 0;
}

char *get_str()
{
	cmo *c = pop();
	if(c->tag == CMO_STRING) {
		return ((cmo_string *)c)->s;
	}
	make_error2(-1);
	return "";
}

int cmo2int(cmo *c)
{
	if(c->tag == CMO_INT32) {
		return ((cmo_int32 *)c)->i;
	}else if(c->tag == CMO_ZZ) {
		return mpz_get_si(((cmo_zz *)c)->mpz);
	} else if(c->tag == CMO_NULL){
		return 0;
	}

	return 0;
}

cmo *matrix2cmo( int **matrix, int row, int col )
{
	cmo_list *result,*work;
	cmo *tmp;
	int i,j;

	result = new_cmo_list();
	for(i=0;i<row;i++){

		work = new_cmo_list();
		for(j=0;j<col;j++){
			if( matrix[i][j] != 0 ){
				tmp = (cmo*) new_cmo_zz_set_si( matrix[i][j] );
			} else {
				tmp = (cmo*) new_cmo_zero();
			}
			work = list_append( work, tmp );
		}
		result = list_append( result, (cmo*)work );

	}

	return (cmo *)result;
}

cmo_qq* new_cmo_qq_set_mpq(mpq_ptr q);

cmo *vector2cmo( mytype *vector, int len)
{
	cmo_list *result;
	cmo *tmp;
	int j;

	result = new_cmo_list();
	for(j=0;j<len;j++){
		if( vector[j] != 0 ){
			tmp = (cmo*) new_cmo_qq_set_mpq( (mpq_ptr)vector[j] );
		} else {
			tmp = (cmo*) new_cmo_zero();
		}
		result = list_append( result, tmp );
	}

	return (cmo *)result;
}

int mytype2int( mytype a, int i )
{
#if defined GMPRATIONAL
/*	How to round "a" depends on "i".
	if i < 0, then the rounding style is "floor".
	if i = 0, then the rounding style is "truncate".
	if i > 0, then the rounding style is "ceil".	*/
	int ret;
	mpz_t q;
	mpz_init(q);

	if( i < 0 ){
		mpz_fdiv_q( q, mpq_numref(a), mpq_denref(a) );
	} else if( i > 0 ){
		mpz_cdiv_q( q, mpq_numref(a), mpq_denref(a) );
	} else {
		mpz_tdiv_q( q, mpq_numref(a), mpq_denref(a) );
	}

	ret = mpz_get_si( q );
	mpz_clear( q );
	return ret;
#else
/*	How to round "a" depends on "i".
	if i < 0, then the rounding style is "floor".
	if i = 0, then the rounding style is "near".
	if i > 0, then the rounding style is "ceil".	*/
	if( i < 0 ){
		return floor( *a + dd_almostzero );
	} else if( i == 0 ){
		return (int)( *a + 0.5 );
	} else {
		return ceil( *a - dd_almostzero );
	}
#endif
}

void my_redcheck(void)
{
	int row,col,result_row;
	int i,j;
	cmo *a,*b,*c;
	int **matrix,**result;

	pop();	/*	for argc	*/
	row = get_i();	/*	row size	*/
	col = get_i();	/*	col size	*/

	a = pop();

	matrix = MALLOC( row * sizeof(int*) );
	for(i=0;i<row;i++){
		matrix[i] = MALLOC( col * sizeof(int) );
	}

	for(i=0;i<row;i++){
		b = list_nth( (cmo_list*)a, i );
		for(j=0;j<col;j++){
			c = list_nth( (cmo_list*)b, j );
			matrix[i][j] = cmo2int( c );
		}
	}

	dprint(1,"redcheck...");
	result = redcheck(row,col,matrix,&result_row);
	dprint(1,"done\n");
	
	push( matrix2cmo(result,result_row,col) );
}

cmo_qq* new_cmo_qq_set_mpq(mpq_ptr q);

void my_lpsolve( int resulttype, int index )
{
	int row,col;
	int i,j;
	cmo *a,*b,*c;
	cmo_list* ret;
	int **matrix,*object;
	int lpmax,lpmin;
	mytype *tmp;
	cmo *cmomin,*cmomax,*cmoint;

	pop();	/*	for argc	*/
	row = get_i();	/*	row size	*/
	col = get_i();	/*	col size	*/

	matrix = MALLOC( row * sizeof(int*) );
	for(i=0;i<row;i++){
		matrix[i] = MALLOC( col * sizeof(int) );
	}

	a = pop();

	/*	For matrix	*/
	for(i=0;i<row;i++){
		b = list_nth( (cmo_list*)a, i );
		for(j=0;j<col;j++){
			c = list_nth( (cmo_list*)b, j );
			matrix[i][j] = cmo2int( c );
		}
	}

	if ( index != LP_INTPT ) {
		/*	For object	*/
		object = MALLOC( col * sizeof(int) );
		a = pop();
	
		for(i=0;i<col;i++){
			c = list_nth( (cmo_list*)a, i );
	
			object[i] = cmo2int( c );
		}
	}

	if( index == LP_MAX ){
		dprint( 1, "lpsolve(max)...");
	} else if( index == LP_MIN ){
		dprint( 1, "lpsolve(min)...");
	} else if( LP_MAXMIN ){
		dprint( 1, "lpsolve(maxmin)...");
	} else if( LP_INTPT ){
		dprint( 1, "lpsolve(intpt)...");
	}

	if( index == LP_MAX || index == LP_MAXMIN ){
		tmp = lpsolve( dd_LPmax, row, col, matrix, object );
		if( resulttype == LP_I ){
			lpmax = mytype2int( *tmp, -1 );
			if( lpmax == 0 ){
				cmomax = (cmo*) new_cmo_zero();
			} else {
				cmomax = (cmo*) new_cmo_zz_set_si( lpmax );
			}
		} else if( resulttype == LP_Q ){
#if defined GMPRATIONAL
			if( mpz_cmp_si( mpq_numref( *tmp ), 0 ) == 0 ){
				cmomax = (cmo*) new_cmo_zero();
			} else {
				cmomax = (cmo*) new_cmo_qq_set_mpz( mpq_numref( *tmp ), mpq_denref( *tmp ) );
			}
#else
			cmomax = (cmo*) new_cmo_double( *tmp[0] );
#endif
		}
#if defined GMPRATIONAL
		mpq_clear( *tmp );
#endif
	}

	if( index == LP_MIN || index == LP_MAXMIN ){	
		tmp = lpsolve( dd_LPmin, row, col, matrix, object );
		if( resulttype == LP_I ){
			lpmin = mytype2int( *tmp, 1 );
			if( lpmin == 0 ){
				cmomin = (cmo*) new_cmo_zero();
			} else {
				cmomin = (cmo*) new_cmo_zz_set_si( lpmin );
			}
		} else if( resulttype == LP_Q ){
#if defined GMPRATIONAL
			if( mpz_cmp_si( mpq_numref( *tmp ), 0 ) == 0 ){
				cmomin = (cmo*) new_cmo_zero();
			} else {
				cmomin = (cmo*) new_cmo_qq_set_mpz( mpq_numref( *tmp ), mpq_denref( *tmp ) );
			}
#else
			cmomin = (cmo*) new_cmo_double( *tmp[0] );
#endif
		}
#if defined GMPRATIONAL
		mpq_clear( *tmp );
#endif

	}

	if( index == LP_INTPT ) {
		tmp = lpintpt(row, col, matrix);
		if ( tmp ) {
			cmoint = (cmo *)new_cmo_list();
			for(j=0;j < col;j++){
				if( tmp[j] != 0 ){
#if defined GMPRATIONAL
					a = (cmo*) new_cmo_qq_set_mpq( (mpq_ptr)tmp[j] );
#else
					a = (cmo*) new_cmo_double( *tmp[j] );
#endif
				} else {
					a = (cmo*) new_cmo_zero();
				}
				cmoint = (cmo *)list_append( (cmo_list *) cmoint, a );
			}
		} else
			cmoint = new_cmo_zero();
#if defined GMPRATIONAL
		for ( i = 0; i < col; i++ )
			mpq_clear( (mpq_ptr)tmp[i] );
#endif
	}

	if( index == LP_MAX ){
		push( cmomax );
	} else if( index == LP_MIN ){
		push( cmomin );
	} else if( index == LP_MAXMIN ){
		ret = new_cmo_list();
		ret = list_append( ret, cmomin );
		ret = list_append( ret, cmomax );
		push( (cmo*) ret );
	} else if ( index == LP_INTPT )
		push( cmoint );
}

int sm_executeFunction()
{
	cmo_string *func = (cmo_string *)pop();
	if(func->tag != CMO_STRING) {
		printf("sm_executeFunction : func->tag is not CMO_STRING");fflush(stdout);
		push((cmo*)make_error2(0));
		return -1;
	}

	if(strcmp(func->s, "redcheck" ) == 0) {
		my_redcheck();
		return 0;
	} else if( strcmp( func->s, "ilpmax" ) == 0 ){
		my_lpsolve(LP_I,LP_MAX);
		return 0;
	} else if( strcmp( func->s, "ilpmin" ) == 0 ){
		my_lpsolve(LP_I,LP_MIN);
		return 0;
	} else if( strcmp( func->s, "ilpmaxmin" ) == 0 ){
		my_lpsolve(LP_I,LP_MAXMIN);
		return 0;
	} else if( strcmp( func->s, "lpmax" ) == 0 ){
		my_lpsolve(LP_Q,LP_MAX);
		return 0;
	} else if( strcmp( func->s, "lpmin" ) == 0 ){
		my_lpsolve(LP_Q,LP_MIN);
		return 0;
	} else if( strcmp( func->s, "lpmaxmin" ) == 0 ){
		my_lpsolve(LP_Q,LP_MAXMIN);
		return 0;
	} else if( strcmp( func->s, "intpt" ) == 0 ){
		my_lpsolve(LP_Q,LP_INTPT);
		return 0;
	} else if( strcmp( func->s, "debugprint" ) == 0 ){
		pop();
		debug_print = get_i();
		return 0;
	} else if( strcmp( func->s, "exit" ) == 0 ){
		pop();
		exit(0);
		return 0;
	} else {
		push((cmo*)make_error2(0));
		return -1;
	}
}

int receive_and_execute_sm_command()
{
	int code = receive_int32(fd_rw);
	switch(code) {
	case SM_popCMO:
		sm_popCMO();
		break;
	case SM_executeFunction:
		sm_executeFunction();
		break;
	case SM_mathcap:
		sm_mathcap();
		break;
	case SM_setMathCap:
		pop();
		break;
	default:
		printf("receive_and_execute_sm_command : code=%d\n",code);fflush(stdout);
		break;
	}
	return 0;
}

int receive()
{
	int tag;

	tag = receive_ox_tag(fd_rw);
	switch(tag) {
	case OX_DATA:
		push(receive_cmo(fd_rw));
		break;
	case OX_COMMAND:
		receive_and_execute_sm_command();
		break;
	default:
		printf("receive : tag=%d\n",tag);fflush(stdout);
	}
	return 0;
}

int main()
{
	GC_INIT();
	ox_stderr_init(stderr);
	initialize_stack();
	init_cdd();

#if defined GMPRATIONAL
	fprintf(stderr,"ox_cddgmp\n");
#else
	fprintf(stderr,"ox_cdd\n");
#endif

	fd_rw = oxf_open(3);
	oxf_determine_byteorder_server(fd_rw);

	while(1){
		receive();
	}
}
