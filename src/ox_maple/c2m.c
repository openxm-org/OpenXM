/* $OpenXM$ */
/************************************************************************
 * CMO ==> MapleObject converter
 *
 *
 *********************************************************************** */

#include <stdio.h>
#include <string.h>

#include "oxmaple.h"
#include "maplec.h"

#define DPRINTF(x)	printf x; fflush(stdout)



/*********************************************************************
 *
 *********************************************************************/
static ALGEB
convert_cmolist_to_maple(
	MKernelVector kv, 
	cmo_list *c)
{
	cell *cl;
	ALGEB list;
	int i;

	cl = list_first(c);
	list = MapleListAlloc(kv, c->length);
	for (i = 0; i < c->length; i++) {
		ALGEB alg = convert_cmo_to_maple(kv, cl->cmo, NULL);
		if (alg == NULL) {
			return (NULL);
		}
		MapleListAssign(kv, list, i+1, alg);
		cl = list_next(cl);
	}

	return (list);
}

/*********************************************************************
 *
 *********************************************************************/
static ALGEB
convert_cmorpoly_to_maple(
	MKernelVector kv, 
	cmo_recursive_polynomial *c)
{
	return (convert_cmo_to_maple(kv, c->coef, c->ringdef));
}

/*********************************************************************
 *
 *********************************************************************/
static ALGEB
convert_cmo1poly_to_maple(
	MKernelVector kv, 
	cmo_polynomial_in_one_variable *c,
	cmo_list *ringdef)
{
	ALGEB t, n;
	int i;
	ALGEB var, tm;
	char *str;
	cell *cl;
	cmo *ob;
	char buf[100];

	if (ringdef == NULL) {
		return (NULL);
	}

	cl = list_first(ringdef);
	for (i = 0; i < c->var; i++) {
		cl = list_next(cl);
	}
	ob = cl->cmo;
	if (ob->tag == CMO_INDETERMINATE) {
		ob = ((cmo_indeterminate *)ob)->ob;
	}

	var = ToMapleString(kv, ((cmo_string *)ob)->s);
	

	sprintf(buf, "vArNaMe_%d_tt", c->var);
	tm = ToMapleString(kv, buf);
	t = ToMapleName(kv, buf, TRUE);
	n = MapleAssign(kv, t, ToMapleInteger(kv, 0));

	cl = list_first((cmo_list *)c);
	for (; !list_endof((cmo_list *)c, cl); cl = list_next(cl)) {
		n = convert_cmo_to_maple(kv, cl->cmo, ringdef);
		if (n == NULL) {
			return (NULL);
		}
		if (cl->exp == 0) {
			n = MapleALGEB_SPrintf(kv, "%s := %s+(%a):", tm, tm, n);
			str = MapleToString(kv, n);
		} else {
			n = MapleALGEB_SPrintf(kv, "%s := %s+(%a) * %s^%d:", tm, tm, n, var, ToMapleInteger(kv, cl->exp));
			str = MapleToString(kv, n);
		}

		n = EvalMapleStatement(kv, str);
	}

	return (n);
}

/*********************************************************************
 * convert CMO to Maple Object
 *********************************************************************/
ALGEB
convert_cmo_to_maple(MKernelVector kv, cmo *c, cmo_list *ringdef)
{
	char *str, *str2;
	ALGEB alg = NULL;
printf("tag=%d @ conv_cmo2maple\n", c->tag);
	switch (c->tag) {
	case CMO_ZERO:
		alg = ToMapleInteger(kv, 0);
		break;
	case CMO_INT32:
		alg = ToMapleInteger(kv, ((cmo_int32 *)c)->i);
		break;
	case CMO_ZZ:
		str2 = new_string_set_cmo((cmo *)c);
		str = MapleAlloc(kv, strlen(str2) + 3);
		sprintf(str, "%s:", str2);
printf("ZZ=%s, alg=%p\n", str, alg);
		alg = EvalMapleStatement(kv, str);
printf("ZZ=%s, alg=%p\n", str, alg);
MapleALGEB_Printf(kv, "alg=%a\n", alg);
		break;
	case CMO_STRING:
		alg = ToMapleString(kv, ((cmo_string *)c)->s);
		break;
	case CMO_LIST:
		alg = convert_cmolist_to_maple(kv, (cmo_list *)c);
		break;
	case CMO_RECURSIVE_POLYNOMIAL:
		alg = convert_cmorpoly_to_maple(kv, (cmo_recursive_polynomial *)c);
		break;
	case CMO_POLYNOMIAL_IN_ONE_VARIABLE:
		alg = convert_cmo1poly_to_maple(kv, (cmo_polynomial_in_one_variable *)c, ringdef);
		break;
	case CMO_QQ:
		str2 = mpq_get_str(NULL, 10, ((cmo_qq *)c)->mpq);
		str = MapleAlloc(kv, strlen(str2) + 3);
		sprintf(str, "%s:", str2);
		alg = EvalMapleStatement(kv, str);
	default:
		alg = NULL;
	}

	return (alg);
}


