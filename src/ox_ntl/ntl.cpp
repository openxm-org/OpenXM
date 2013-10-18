/* $OpenXM: OpenXM/src/ox_ntl/ntl.cpp,v 1.6 2008/09/19 10:55:40 iwane Exp $ */

#include <NTL/ZZXFactoring.h>
#include <NTL/LLL.h>

#include "ox_toolkit.h"
#include "ntl.h"

#if __NTL_DEBUG
#define __NTL_PRINT (1)
#endif

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
oxstack_node *
ntl_fctr(oxstack_node **arg, int argc)
{
	oxstack_node *p = arg[0];
	cmo *poly = p->c;
	cmo_indeterminate *x;
	cmo *err;
	ZZX f;
	int ret;
	char emss[25] = "Invalid Parameter(#)";
	char emst[25] = "Invalid Parameter(type)";

	if (argc != 1) {
		err = ((cmo *)new_cmo_error2((cmo *)new_cmo_string(emss)));
		p = oxstack_node_init(err);
		return (p);
	}

	ret = cmo_to_ZZX(f, poly, x);
	if (ret != NTL_SUCCESS) {
		/* format error */
		err = (cmo *)new_cmo_error2((cmo *)new_cmo_string(emst));
		p = oxstack_node_init(err);
		return (p);
	}

#if __NTL_PRINT
	std::cout << "input: " << f << std::endl;
#endif

	cmon_factors_t *factors = new_cmon_factors();
	factors->x = x;

	factor(*factors->cont, *factors->f, f);

#if __NTL_PRINT
	std::cout << "fctr : " << *factors->f << std::endl;
#endif

	p = oxstack_node_init(NULL);
	p->p = factors;

	return (p);
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
oxstack_node *
ntl_lll(oxstack_node **arg, int argc)
{
	oxstack_node *p = arg[0];
	cmo *err;
	ZZX f;
	int ret;
	cmon_mat_zz_t *mat;
	char emss[25] = "Invalid Parameter(#)";
	char emst[25] = "Invalid Parameter(type)";

	if (argc != 1) {
		err = ((cmo *)new_cmo_error2((cmo *)new_cmo_string(emss)));
		p = oxstack_node_init(err);
		return (p);
	}

	mat = new_cmon_mat_zz();
	ret = cmo_to_mat_zz(*mat->mat, p->c);
	if (ret != NTL_SUCCESS) {
		delete_cmon_mat_zz(mat);
		/* format error */
		err = ((cmo *)new_cmo_error2((cmo *)new_cmo_string(emst)));
		p = oxstack_node_init(err);
		return (p);
	}

#if __NTL_PRINT
	std::cout << "input: " << (*mat->mat) << std::endl;
#endif

	ZZ det2;
	mat_ZZ U;

#if __NTL_PRINT
	std::cout << "output: " << (*mat->mat) << std::endl;
	std::cout << U <<std:: endl;
#endif

	p = oxstack_node_init(NULL);
	p->p = mat;
	return (p);
}



#if __NTL_DEBUG /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

#include <unistd.h>
#include <gc/gc.h>

void ntl_free(void *ptr, size_t size) {}

void *ntl_realloc(void *org, size_t old, size_t size)
{
	void *ptr = GC_realloc(org, size);
	return (ptr);
}


int
main(int argc, char *argv[])
{
#if 0
{
	ZZX f;
	const char *str = "12345";
	int num = 1;
	char *var = "x";
	cmo *c;

	f = to_ZZ(str);;

	cmo_indeterminate *x = new_cmo_indeterminate((cmo *)new_cmo_string(var));
	cmo_recursive_polynomial *poly = ZZX_to_cmo(f, x);
	cmo *arg[3];

	arg[0] = (cmo *)poly;

	c = ntl_fctr(arg, num);
}
#endif

{
	
	ZZ cont;
	ZZX fac;
	vec_pair_ZZX_long facs;
//	char xx[2] = "x";
//	cmo_indeterminate *x = new_cmo_indeterminate((cmo *)new_cmo_string(xx));

//	std::istringstream istr("[3 -3 -6]");
//	istr >> fac;

	SetCoeff(fac, 0, 3);
	SetCoeff(fac, 1, -3);
	SetCoeff(fac, 2, -6);


	factor(cont, facs, fac);

	//	mpz_clear(ppp->mpz);
}

if (0) {
	mp_set_memory_functions(GC_malloc, ntl_realloc, ntl_free);
	for (int i = 0;; i++) {

//		cmon_factors_t *mat = new_cmon_factors(cont, facs, x);
//		cmo *aaa = convert_cmon((cmo *)mat);
//		delete_cmon((cmo *)mat);

		if (i % 1000 == 0) {
			printf("GC-counts: %lu,   size: %u\n", GC_gc_no, GC_get_heap_size());
			GC_gcollect();
			usleep(1);
		}
	}
}


	return (0);
}

#endif /* __NTL_DEBUG @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

