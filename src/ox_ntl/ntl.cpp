/* $OpenXM: OpenXM/src/ox_ntl/ntl.cpp,v 1.2 2003/11/08 12:34:00 iwane Exp $ */

#include <NTL/ZZXFactoring.h>
#include <NTL/LLL.h>

#include "ox_toolkit.h"
#include "ntl.h"

#if __NTL_DEBUG
#define __NTL_PRINT (1)
#endif

#define __NTL_PRINT (1)
#define DPRINTF(x)	printf x; fflush(stdout)

/****************************************************************************
 *
 * Factorize polynomial over the integers.
 *
 * PARAM : I : arg  : polynomial
 *       : I : argc :
 * RETURN: [num,[[factor1,multiplicity1],[factor2,multiplicity2],...]]
 *
 * EX    : ntl_fctr([2*x^11+4*x^8-2*x^6+2*x^5-4*x^3-2],1) ==>
         : [2,[[x-1,1],[x^4+x^3+x^2+x+1,1],[x+1,2],[x^2-x+1,2]]]
 *
 ****************************************************************************/
cmo *
ntl_fctr(cmo **arg, int argc)
{
	cmo *poly = arg[0];
	cmo_indeterminate *x;
	ZZX f;
	int ret;

	if (argc != 1) {
		return ((cmo *)new_cmo_error2((cmo *)new_cmo_string("Invalid Parameter(#)")));
	}

	ret = cmo_to_ZZX(f, poly, x);
	if (ret != NTL_SUCCESS) {
		/* format error */
		return ((cmo *)new_cmo_error2((cmo *)new_cmo_string("Invalid Parameter(type)")));
	}

#if __NTL_PRINT
	cout << "input: " << f << endl;
#endif

	cmon_factors_t *factors = new_cmon_factors();
	factors->x = x;

	factor(*factors->cont, *factors->f, f);

#if __NTL_PRINT
	cout << "fctr : " << *factors->f << endl;
#endif


	return ((cmo *)factors);
}

/****************************************************************************
 *
 * Factorize polynomial over the integers.
 *
 * PARAM : I : arg  : polynomial
 *       : I : argc :
 * RETURN: [num,[[factor1,multiplicity1],[factor2,multiplicity2],...]]
 *
 * EX    : ntl_fctr([2*x^11+4*x^8-2*x^6+2*x^5-4*x^3-2],1) ==>
         : [2,[[x-1,1],[x^4+x^3+x^2+x+1,1],[x+1,2],[x^2-x+1,2]]]
 *
 ****************************************************************************/
cmo *
ntl_lll(cmo **arg, int argc)
{
	cmo *poly = arg[0];
	cmo_indeterminate *x;
	ZZX f;
	int ret;
	cmon_mat_zz_t *mat;
	
DPRINTF(("lll start\n"));

	if (argc != 1) {
DPRINTF(("invalid argc %d\n", argc));
		return ((cmo *)new_cmo_error2((cmo *)new_cmo_string("Invalid Parameter(#)")));
	}

	mat = new_cmon_mat_zz();
	ret = cmo_to_mat_zz(*mat->mat, arg[0]);
DPRINTF(("ret = %d, convert\n", ret));
	if (ret != NTL_SUCCESS) {
DPRINTF(("convert failed\n", ret));
		delete_cmon_mat_zz(mat);
		/* format error */
		return ((cmo *)new_cmo_error2((cmo *)new_cmo_string("Invalid Parameter(type)")));
	}

DPRINTF(("convert success\n", ret));

#if __NTL_PRINT
DPRINTF(("convert success\n", ret));
	cout << "input: " << (*mat->mat) << endl;
#endif


DPRINTF(("lll start\n", ret));
	ZZ det2;
	long rd = LLL(det2, *mat->mat);


#if __NTL_PRINT
	cout << "output: " << (*mat->mat) << endl;
#endif

	return ((cmo *)mat);
}



#if __NTL_DEBUG /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

int
main(int argc, char *argv[])
{
	ZZX f;
	const char *str = "[1 -1 -1 1]";
	int num = 1;
	char *var = "x";
	cmo *c;

	f = to_ZZ(str);;

	cmo_indeterminate *x = new_cmo_indeterminate((cmo *)new_cmo_string(var));
	cmo_recursive_polynomial *poly = ZZX_to_cmo(f, x);
	cmo *arg[3];

	arg[0] = (cmo *)poly;

	c = ntl_fctr(arg, num);

	return (0);
}

#endif

