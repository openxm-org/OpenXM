/* $OpenXM: OpenXM/src/ox_ntl/ntlconv.cpp,v 1.1 2003/11/08 12:34:00 iwane Exp $ */

#include <NTL/ZZX.h>
#include <NTL/mat_ZZ.h>

#include <strstream>

#include "ntl.h"

#if __NTL_DEBUG
#define __NTL_PRINT (1)
#endif

#define BLOCK_NEW_CMO()         BLOCK_INPUT()
#define UNBLOCK_NEW_CMO()       UNBLOCK_INPUT()



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

	BLOCK_NEW_CMO();
	c = new_cmo_zz_set_string(sout.str());
	UNBLOCK_NEW_CMO();

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
	case CMO_NULL:
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
cmo_list *
mat_zz_to_cmo(mat_ZZ &mat)
{
	cmo_list *list;
	int ret;

	cmo_zz *zz;
	int row, col;
	int i, j;

	row = (int)mat.NumRows();
	col = (int)mat.NumCols();

	BLOCK_NEW_CMO();
	list = list_appendl(NULL, new_cmo_int32(row), new_cmo_int32(col), NULL);

	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			zz = ZZ_to_cmo_zz(mat[i][j]);
			list_append(list, (cmo *)zz);
		}
	}
	UNBLOCK_NEW_CMO();

	return (list);
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
cmo_to_mat_zz(mat_ZZ &mat, cmo *m)
{
	cmo_list *list;
	int ret;
	ZZ row, col, size;
	int len;
	cell *el;
	int i, j;
	int c, r;

	if (m->tag != CMO_LIST) {
		return (NTL_FAILURE);
	}

	list = (cmo_list *)m;
	len = list_length(list);

	if (len < 2) {
		return (NTL_FAILURE);
	}

	el = list_first(list);
	ret = cmo_to_ZZ(row, el->cmo);
	if (ret != NTL_SUCCESS) {
		return (ret);
	}

	el = list_next(el);
	ret = cmo_to_ZZ(col, el->cmo);
	if (ret != NTL_SUCCESS) {
		return (ret);
	}

	mul(size, row, col);

	if (len - 2 != size) {
		return (NTL_FAILURE);
	}

	/* row and col is less than INT_MAX */
	r = to_int(row);
	c = to_int(col);
	mat.SetDims(r, c);
	for (i = 0; i < r; i++) {
		for (j = 0; j < c; j++) {
			el = list_next(el);
			ret = cmo_to_ZZ(mat[i][j], el->cmo);
			if (ret) {
				return (ret);
			}
		}
	}
	return (NTL_SUCCESS);
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

	BLOCK_NEW_CMO();

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

	UNBLOCK_NEW_CMO();

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

	BLOCK_NEW_CMO();

	deg = new_cmo_int32(d);
	list = list_appendl(NULL, poly, deg, NULL);

	UNBLOCK_NEW_CMO();

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
	cmo_list *list;
	cmo_list *factor;

	BLOCK_NEW_CMO();

	list = new_cmo_list();
	for (i = 0; i < factors.length(); i++) {
		factor = ZZX_int_to_cmo(factors[i].a, (int)factors[i].b, x);
		list_append(list, (cmo *)factor);
	}

	UNBLOCK_NEW_CMO();

	return (list);
}


/****************************************************************************
 * convert local object to cmo.
 * for SM_popCMO
 *
 * PARAM : I : p : cmo
 * RETURN: cmo
 ****************************************************************************/
cmo *
convert_cmon(cmo *p)
{

	switch (p->tag) {
	case CMON_ZZ:
	{
		cmon_zz_t *z = (cmon_zz_t *)p;
		return ((cmo *)ZZ_to_cmo_zz(*z->z));
	}
	case CMON_ZZX:
	{
		cmon_zzx_t *f = (cmon_zzx_t *)p;
		return ((cmo *)ZZX_to_cmo(*f->f, f->x));
	}
	case CMON_FACTORS:
	{
		cmon_factors_t *f = (cmon_factors_t *)p;
		cmo_zz *z = ZZ_to_cmo_zz(*f->cont);
		cmo_list *list = vec_pair_ZZX_long_to_cmo(*f->f, f->x);
		return ((cmo *)list_appendl(NULL, (cmo *)z, list, NULL));
	}
	case CMON_MAT_ZZ:
	{
		cmon_mat_zz_t *m = (cmon_mat_zz_t *)p;
		cmo_list *list = mat_zz_to_cmo(*m->mat);
		return ((cmo *)list);
	}
	default:
		return (p);
	}
}



/****************************************************************************
 * convert tag of local object to tag of cmo.
 * for SM_pushCMOtag
 *
 * PARAM : I : p : cmo
 * RETURN:  tag
 ****************************************************************************/
int
get_cmon_tag(cmo *p)
{
	switch (p->tag) {
	case CMON_ZZ:
		return (CMO_ZZ);
	case CMON_ZZX:
		return (CMO_RECURSIVE_POLYNOMIAL);
	case CMON_FACTORS:
		return (CMO_LIST);
	case CMON_MAT_ZZ:
		return (CMON_MAT_ZZ);
	default:
		return (p->tag);
	}
}




