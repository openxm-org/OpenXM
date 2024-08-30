/*	$Id$	*/

/*
* ##  original  #################################################
MixedVol: Polynomial system mixed volume calculation program.
* Version 1.0 Copyright (C) 2003  Tangan Gao, T. Y. Li, Xing Li, and Mengnien Wu.
* ###############################################################
*/

#include <cstdlib>
#include <string>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits.h>

#include "GetOption/getopt.h"
#include "PolyReader/PolynomialSystemReader.h"
#include "PolyReader/PolynomialException.h"
#include "PreProcess/Pre4MV.h"
#include "MixedVol/MixedVol.h"

extern int debug_print;

extern "C" int mvol(int nVar,int *Spt1stIdx,int **Spt)
{
	int i, j=0, k;
	int nPts = 0;
	int nSpt;

	nPts = Spt1stIdx[nVar];

	for (i=nVar-1; i>=0; i--){
		Spt1stIdx[i] = Spt1stIdx[i+1] - Spt1stIdx[i];
	}

	// Quick return if 1-variable system or less than two terms
	k = -1;
	for (i=0; i<nVar; i++){
		if ( Spt1stIdx[i+1]-Spt1stIdx[i] < 2 )	{
			k = i;
			break;
		}
	}
	if ( k >= 0 ){
		if( debug_print >= 2 ){
			cout << "The " << k+1 << "-th support has less than 2 points" << endl;
		}
		return 0;   // end of the case: too few terms
	}

	if ( nVar == 1 ){
		int kmin, kmax;
		kmin = INT_MAX/2;
		kmax = -INT_MAX/2;
		for (i=0; i<Spt1stIdx[1]; i++){
			kmax = max(kmax, Spt[i][0]);
			kmin = min(kmin, Spt[i][0]);
		}
		if( debug_print >= 2 ){
			cout << "A 1-dimensional support, its mixed volume is " << kmax-kmin << endl;
		}
		return (kmax-kmin);   // end of the case: 1-variable
	}
	for (i=0; i<Spt1stIdx[nVar]; i++){
		int kmin, kmax;
		kmin = INT_MAX/2;
		kmax = -INT_MAX/2;
		k=0;
		for (j=0; j<nVar; j++){
			kmax = max(kmax, Spt[i][j]);
			kmin = min(kmin, Spt[i][j]);
			k += abs(Spt[i][j]);
		}
		if ( kmin != 0 || kmax > 1 || k > 1 ){
			j = -1;
			break;
		}
	}
	if ( j != -1 ){
		if( debug_print >= 2 ){
			cout << "A linear support, its mixed volume <= 1" << endl;
		}
		return 1;
	} 
	// end of quick return

	// To preprocess the support

	int* SptType = new int [nVar];
	int* SptVtx1stIdx = new int [nVar+1];
	int** SptVtx = new int* [nPts];
	SptVtx[0] = new int [nVar*nPts];
	for (i=1; i<nPts; i++) SptVtx[i] = SptVtx[i-1] + nVar;
	int* NuIdx2OldIdx = new int [nPts];
	nSpt = nVar;

	Pre4MV(nVar,nSpt,SptType,Spt,Spt1stIdx,SptVtx,SptVtx1stIdx,NuIdx2OldIdx);

	// To calculate the mixed volume of the support

	srand( unsigned(time(0)) );
	double* lft = new double[SptVtx1stIdx[nSpt]];
	for (j=0; j<SptVtx1stIdx[nSpt]; j++){
		lft[j] = 1.0+(3*double(rand())-double(rand()))/RAND_MAX;
	}

	int CellSize = nSpt;
	for (j=0; j<nSpt; j++ ){
		CellSize += SptType[j];
	}
	CellStack  MCells( CellSize );
	int MVol;

	MixedVol(nVar,nSpt,SptType,SptVtx1stIdx,SptVtx,lft,MCells,MVol);

	if( debug_print >= 2 ){
		cout << "The mixed volume is " << MVol << endl;
	}
	// Clean the memory

	while( ! MCells.IsEmpty() ) MCells.Pop();

	delete [] SptType;
	delete [] SptVtx1stIdx;
	delete [] SptVtx[0];
	delete [] SptVtx;
	delete [] NuIdx2OldIdx;
	delete [] lft;

	return MVol;
}
