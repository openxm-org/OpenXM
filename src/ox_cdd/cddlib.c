/*	$OpenXM: OpenXM/src/ox_cdd/cddlib.c,v 1.1 2005/05/25 04:42:20 noro Exp $	*/

/*	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include "setoper.h"
#include "cdd.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

/*	for gc	*/
#include <gc/gc.h>
#define MALLOC(x) GC_MALLOC((x))
#define MALLOC_ATOMIC(x) GC_MALLOC_ATOMIC((x))
#define ALLOCA(x) alloca((x))
/*	#define FREE(x) free((x))	*/
#define FREE(x)  

int debug_print = 1;
int mytype2int( mytype , int );


void init_cdd(void)
{
	dd_set_global_constants();	/* First, this must be called. */
	debug_print = 0;
}

int **redcheck(int row,int col,int **matrix,int *presult_row)
{
	dd_MatrixPtr M=NULL,M2=NULL,M3=NULL;
	dd_ErrorType err=dd_NoError;
	dd_rowset redrows,linrows,ignoredrows, basisrows;
	dd_colset ignoredcols, basiscols;
	long rank;

	time_t starttime, endtime;

	int i,j;
	int **result=NULL;

	M = dd_CreateMatrix( row, col );
	for(i=0;i<row;i++){
		for(j=0;j<col;j++){
			dd_set_si(M->matrix[i][j],matrix[i][j]);
		}
	}

	M->representation=dd_Generator;

	if(debug_print>=2){
		printf("Inputted Matrix : \n");
		dd_WriteMatrix(stdout, M);

		fprintf(stdout, "\nredundant rows:\n");
		time(&starttime);
	}

	redrows=dd_RedundantRows(M, &err);

	if(debug_print>=2){
		time(&endtime);
		set_fwrite(stdout, redrows);
		dd_WriteTimes(stdout,starttime,endtime);
	}

	M2=dd_MatrixSubmatrix(M, redrows);

	if(debug_print>=2){
		printf("Implicit linearity (after removal of redundant rows): ");
	}

	linrows=dd_ImplicitLinearityRows(M2, &err);

	if(debug_print>=2){
		fprintf(stdout," %ld  ", set_card(linrows));
		set_fwrite(stdout,linrows);
	}
	set_uni(M2->linset, M2->linset, linrows); 
	/* add the implicit linrows to the explicit linearity rows */

	if(debug_print>=2){
		printf("\nNonredundant representation (except possibly for the linearity part):\n");
		dd_WriteMatrix(stdout, M2);
	}

	/* To remove redundancy of the linearity part, 
	we need to compute the rank and a basis of the linearity part. */
	set_initialize(&ignoredrows, M2->rowsize);
	set_initialize(&ignoredcols, M2->colsize);
	set_compl(ignoredrows, M2->linset);
	rank=dd_MatrixRank(M2,ignoredrows,ignoredcols, &basisrows, &basiscols);
	set_diff(ignoredrows, M2->linset, basisrows);
	M3=dd_MatrixSubmatrix(M2, ignoredrows);
	if (rank>0){
		if(debug_print>=2){
			fprintf(stdout,"\nThe following %ld linearity rows are dependent and unnecessary:", set_card(ignoredrows));
			set_fwrite(stdout,ignoredrows);
		}
	} else{
		if(debug_print>=2){
			fprintf(stdout,"\nThe linearity rows are independent and thus minimal\n");
		}
	}

	if(debug_print>=2){
		printf("Nonredundant representation (= minimal representation):\n");
		dd_WriteMatrix(stdout, M3);
	}

	*presult_row = M3->rowsize;

	result = MALLOC( *presult_row * sizeof(int*) );
	for(i=0;i<*presult_row;i++){
		result[i] = MALLOC( col * sizeof(int) );
	}

	for(i=0;i<*presult_row;i++){
		for(j=0;j<col;j++){
			result[i][j] = mytype2int( M3->matrix[i][j], 0 );
		}
	}

	set_free(linrows);
	set_free(basisrows);
	set_free(basiscols);
	set_free(ignoredrows);
	set_free(ignoredcols);
	set_free(redrows);

	dd_FreeMatrix(M);
	dd_FreeMatrix(M2);
	dd_FreeMatrix(M3);

	if (err!=dd_NoError) dd_WriteErrorMessages(stderr,err);
	return result;
}

mytype *lpsolve(dd_LPObjectiveType type,int row,int col,int **matrix,int *obj)
{
	dd_ErrorType error=dd_NoError;
	dd_LPSolverType solver;	/*  either DualSimplex or CrissCross  */
	dd_LPPtr lp;

	dd_MatrixPtr A;
	dd_ErrorType err;
	int i,j;
	static mytype result;

	A = dd_CreateMatrix( row, col );
	for(i=0;i<row;i++){
		for(j=0;j<col;j++){
			dd_set_si(A->matrix[i][j],matrix[i][j]);
		}
	}

	for(j=0;j<col;j++){
		dd_set_si(A->rowvec[j],obj[j]);
	}

	A->objective = type;
	lp=dd_Matrix2LP(A, &err);	/* load an LP */
	if (lp==NULL) goto _L99;

	if(debug_print>=2){
		/*	Print the LP.	*/
		printf("\n--- LP to be solved  ---\n");
		dd_WriteLP(stdout, lp);

		/*	Solve the LP by cdd LP solver.	*/
		printf("\n--- Running dd_LPSolve ---\n");
	}

	solver=dd_DualSimplex;
	dd_LPSolve(lp, solver, &error);	/* Solve the LP */
	if (error!=dd_NoError) goto _L99;

	if(debug_print>=2){
		/*	Write the LP solutions by cdd LP reporter.	*/
		dd_WriteLPResult(stdout, lp, error);
	}

	dd_init( result );
	dd_set( result, lp->optvalue );

	/*	Free allocated spaces.	*/
	dd_FreeLPData(lp);
	dd_FreeMatrix(A);

_L99:
	if (error!=dd_NoError) dd_WriteErrorMessages(stdout, error);
	return &result;
}

mytype *lpintpt(int row,int col,int **matrix,mytype *ptp)
{
	dd_ErrorType error=dd_NoError;
	dd_rowset ImL, Lbasis;
	dd_LPSolutionPtr lps1;

	dd_MatrixPtr A;
	dd_ErrorType err;
	int i,j;
	mytype *ret;

	A = dd_CreateMatrix( row, col );
	for(i=0;i<row;i++){
		for(j=0;j<col;j++){
			dd_set_si(A->matrix[i][j],matrix[i][j]);
		}
	}

	dd_FindRelativeInterior(A,&ImL,&Lbasis,&lps1,&error);
	if (error!=dd_NoError) goto _L99;
	if ( dd_Positive(lps1->optvalue) ) {
		ret = (mytype *)MALLOC(lps1->d*sizeof(mytype));
		for ( i = 0; i < lps1->d; i++ ) {
			dd_init(ret[i]);
			dd_set(ret[i],lps1->sol[i]);
		}
	} else {
		ret = 0;
	}
	dd_FreeLPSolution(lps1);
	set_free(ImL);
	set_free(Lbasis);
_L99:
	if (error!=dd_NoError) dd_WriteErrorMessages(stdout, error);
	return ret;
}
