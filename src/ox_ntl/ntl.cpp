/* $OpenXM: OpenXM/src/ox_ntl/ntl.cpp,v 1.1 2003/11/03 03:11:21 iwane Exp $ */

#include <NTL/ZZXFactoring.h>

#include "ox_toolkit.h"
#include "ntl.h"

#if __NTL_DEBUG
#define __NTL_PRINT (1)
#endif

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
	ZZ c;

	int ret;

	vec_pair_ZZX_long factors;

	cmo_list *ans;
	cmo *fcts;

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

	factor(c, factors, f);

#if __NTL_PRINT
	cout << "fctr : " << factors << endl;
#endif

	cmo_zz *zz = ZZ_to_cmo_zz(c);

	ans = new_cmo_list();

	fcts = (cmo *)vec_pair_ZZX_long_to_cmo(factors, x);

	list_appendl(ans, zz, (cmo *)fcts, NULL);

	return ((cmo *)ans);
}


#if __NTL_DEBUG

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

