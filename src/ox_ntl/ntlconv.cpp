/* $OpenXM$ */

#include <NTL/ZZX.h>

#include <strstream>

#include "ntl.h"

#if __NTL_DEBUG
#define __NTL_PRINT (1)
#endif

/*==========================================================================*
 * Check string format
 *==========================================================================*/

#define NtlIsSpace(c)  ((c) == ' ' || (c) == '\t')
#define NtlIsDigit(c)  ((c) >= '0' && (c) <= '9')

/****************************************************************************
 *
 * test for string format of integer
 *
 * PARAM : I : str    : string
 *       : O : endptr :
 * RETURN: !0         : the string tests true
 *       :  0         : the string tests false
 *
 ****************************************************************************/
static int
ntl_isZZstr_(const char *str, char const **endptr)
{
	while (NtlIsSpace(*str))
		str++;

	/* NTL reject "+999"  */
	if (*str == '-')
		str++;

	if (!NtlIsDigit(*str))
		return (0);

	str++;

	while (NtlIsDigit(*str))
		str++;

	*endptr = str;

	return (!0);

}

static int
ntl_isZZstr(const char *str)
{
	const char *ptr;
	int ret = ntl_isZZstr_(str, &ptr);
	if (!ret)
		return (ret);

	while (NtlIsSpace(*ptr))
		ptr++;

	return (*ptr == '\0');
}


/****************************************************************************
 *
 * test for string format of univariate polynomials with integer coefficients
 * in NTL style.
 *
 * PARAM : I : str    : string
 *       : O : endptr :
 * RETURN: !0         : the string tests true
 *       :  0         : the string tests false
 *
 ****************************************************************************/
static int
ntl_isZZXstr_(const char *str, char const **endptr)
{
	const char *s;

	while (NtlIsSpace(*str))
		str++;

	if (*str != '[')
		return (0);

	str++;

	while (*str != ']' && *str != '\0') {

		if (!ntl_isZZstr_(str, &s))
			return (0);
		str = s;

		while (NtlIsSpace(*str))
			str++;
	}

	while (NtlIsSpace(*str))
		str++;

	if (*str != ']')
		return (0);

	str++;
	*endptr = str;
	return (!0);
}

static int
ntl_isZZXstr(const char *str)
{
	const char *ptr;
	int ret = ntl_isZZXstr_(str, &ptr);
	if (!ret)
		return (ret);

	while (NtlIsSpace(*ptr))
		ptr++;

	return (*ptr == '\0');
}


/*==========================================================================*
 * Convert
 *==========================================================================*/
/****************************************************************************
 * convert ZZ to cmo_zz
 *
 * PARAM : I : z       : integer
 * RETURN: cmo_zz
 ****************************************************************************/
cmo_zz *
ZZ_to_cmo_zz(const ZZ &z)
{
	cmo_zz *c;

	ostrstream sout;
	sout << z << '\0';

	c = new_cmo_zz_set_string(sout.str());

	return (c);
}


/****************************************************************************
 * convert cmo to ZZ which is integer
 *
 * PARAM : O : z       : integer.
 *       : I : c       : cmo.
 * RETURN: success     : NTL_SUCCESS
 *       : failure     : NTL_FAILURE
 ****************************************************************************/
int
cmo_to_ZZ(ZZ &z, cmo *c)
{
	int ret = NTL_SUCCESS;
	char *str;

	switch (c->tag) {
	case CMO_ZERO:
		z = to_ZZ(0);
		break;
	case CMO_ZZ:
	{
		str = new_string_set_cmo(c);
		z = to_ZZ(str);
		break;
	}
	case CMO_INT32:
		z = to_ZZ(((cmo_int32 *)c)->i);
		break;
	case CMO_STRING:
	{
		str = ((cmo_string *)c)->s;
		if (!ntl_isZZstr(str))
			return (NTL_FAILURE);
		z = to_ZZ(str);
		break;
	}
	default:
		ret = NTL_FAILURE;
		break;
	}

	return (ret);
}


/****************************************************************************
 * convert cmo to ZZX which is polynomial in Z[x]
 *
 * PARAM : O : f       : polynomial in Z[x]
 *       : I : m       : cmo.
 *       : O : x       : indeterminate
 * RETURN: success     : NTL_SUCCESS
 *       : failure     : NTL_FAILURE
 ****************************************************************************/
int
cmo_to_ZZX(ZZX &f, cmo *m, cmo_indeterminate *&x)
{
	char *str;
	int ret;

	switch (m->tag) {
	case CMO_STRING:   /* [ 3 4 7 ] ==> 3+4*x+7*x^2 */
		str = ((cmo_string *)m)->s;
		ret = ntl_isZZXstr(str);

		if (!ret) {
			/* format error */
			return (NTL_FAILURE);
		}
		{
			istrstream sin(str, strlen(str));
			sin >> f;
		}
		break;
	case CMO_RECURSIVE_POLYNOMIAL:
	{
		cmo_recursive_polynomial *rec = (cmo_recursive_polynomial *)m;
		cmo_polynomial_in_one_variable *poly = (cmo_polynomial_in_one_variable *)rec->coef;
		cell *el;
		int len;

		if (poly->tag != CMO_POLYNOMIAL_IN_ONE_VARIABLE) {
			return (NTL_FAILURE);
		}

		el = list_first((cmo_list *)poly);
		len = list_length((cmo_list *)poly);

		f = 0;

		while (!list_endof((cmo_list *)poly, el)) {
			ZZ c;
			cmo *coef = el->cmo;
			int exp = el->exp;

			ret = cmo_to_ZZ(c, coef);
			if (ret != NTL_SUCCESS) {
				return (NTL_FAILURE);
			}

			SetCoeff(f, exp, c);

			el = list_next(el);			
		}


		el = list_first(rec->ringdef);
		x = (cmo_indeterminate *)el->cmo;

		break;	
	}
	default:
		break;
	}
	return (NTL_SUCCESS);
}

/****************************************************************************
 * convert polynomial in Z[x] to cmo_recursive_polynomial
 *
 * PARAM : I : factor  : polynomial in Z[x]
 *       : I : x       : indeterminate
 * RETURN: 
 ****************************************************************************/
cmo_recursive_polynomial *
ZZX_to_cmo(ZZX &factor, cmo_indeterminate *x)
{
	cmo_recursive_polynomial *rec;
	cmo_polynomial_in_one_variable *poly;

	cmo_list *ringdef;
	int i;
	cmo *coef;

	ringdef = new_cmo_list();
	list_append(ringdef, (cmo *)x);

	poly = new_cmo_polynomial_in_one_variable(0);
	for (i = deg(factor); i >= 0; i--) {
		if (coeff(factor, i) == 0)
			continue;

		coef = (cmo *)ZZ_to_cmo_zz(coeff(factor, i));
		list_append_monomial((cmo_list *)poly, coef, i);
	}

	rec = new_cmo_recursive_polynomial(ringdef, (cmo *)poly);
	return (rec);
}


/****************************************************************************
 * convert pair of factor and multiplicity to cmo_list
 *
 * PARAM : I : factor  : polynomial in Z[x]
 *       : I : d       : multiplicity
 *       : I : x       : indeterminate
 * RETURN: 
 ****************************************************************************/
cmo_list *
ZZX_int_to_cmo(ZZX &factor, int d, cmo_indeterminate *x)
{
	cmo_recursive_polynomial *poly;
	cmo_int32 *deg;
	cmo_list *list;

	poly = ZZX_to_cmo(factor, x);
	deg = new_cmo_int32(d);

	list = list_appendl(NULL, poly, deg, NULL);

	return (list);
}


/****************************************************************************
 * convert vec_pair_ZZX_long(list which pair of factor and multiplicity)
 * to cmo_list
 *
 * PARAM : I : factors : list which pair of factor and multiplicity 
 *       :   :         :  [[factor1,multiplicity1][factor2,multiplicity2]...]
 *       : I : x       : indeterminate
 * RETURN: 
 ****************************************************************************/
cmo_list *
vec_pair_ZZX_long_to_cmo(vec_pair_ZZX_long &factors, cmo_indeterminate *x)
{
	int i;
	cmo_list *list = new_cmo_list();
	cmo_list *factor;

	for (i = 0; i < factors.length(); i++) {
		factor = ZZX_int_to_cmo(factors[i].a, (int)factors[i].b, x);
		list_append(list, (cmo *)factor);
	}

	return (list);
}


