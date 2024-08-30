/* $OpenXM$ */
/************************************************************************
 * MapleObject --> CMO converter
 *
 *********************************************************************** */
#include "oxmaple.h"
#include "maplec.h"

/*==========================================================================*
 * Block interrupt input
 *==========================================================================*/

/*
#define BLOCK_NEW_CMO()         BLOCK_INPUT()
#define UNBLOCK_NEW_CMO()       UNBLOCK_INPUT()
*/

#define BLOCK_NEW_CMO()
#define UNBLOCK_NEW_CMO()

#define DPRINTF(x)	printf x; fflush(stdout)

char *
convert_maple2str(MKernelVector kv, ALGEB alg)
{
	ALGEB s;
	char *str;
	s = MapleALGEB_SPrintf(kv, "%a", alg);
 	str = MapleToString(kv, s);
	return (str);
}

cmo *
conv_num2cmo(MKernelVector kv, ALGEB alg)
{
	int n;
	char buf[124];
	ALGEB s;

	/* どーも asir の CMO_INT32 は負の数に対応していない様子 */
	if (IsMapleInteger16(kv, alg)) {
		n = MapleToInteger16(kv, alg);
		return ((cmo *)new_cmo_zz_set_si(n));
/*		return ((cmo *)new_cmo_int32(n)); */
	} else if (IsMapleInteger32(kv, alg)) {
		n = MapleToInteger32(kv, alg);
		return ((cmo *)new_cmo_zz_set_si(n));
/*		return ((cmo *)new_cmo_int32(n)); */
	} else if (IsMapleInteger(kv, alg)) {
		mpz_ptr z = MapleToGMPInteger(kv, alg);
		cmo_zz *c = new_cmo_zz_set_mpz(z);
		return ((cmo *)c);

	}

#if 1
else if (IsMapleAssignedName(kv, alg)) { DPRINTF(("maple IsMapleAssignedName @ conv_num2cmo\n")); }
else if (IsMapleComplexNumeric(kv, alg)) { DPRINTF(("maple IsMapleComplexNumeric @ conv_num2cmo\n")); }
else if (IsMapleInteger(kv, alg)) { DPRINTF(("maple IsMapleInteger @ conv_num2cmo\n")); }
else if (IsMapleInteger16(kv, alg)) { DPRINTF(("maple IsMapleInteger16 @ conv_num2cmo\n")); }
else if (IsMapleInteger32(kv, alg)) { DPRINTF(("maple IsMapleInteger32 @ conv_num2cmo\n")); }
else if (IsMapleInteger64(kv, alg)) { DPRINTF(("maple IsMapleInteger64 @ conv_num2cmo\n")); }
else if (IsMapleInteger8(kv, alg)) { DPRINTF(("maple IsMapleInteger8 @ conv_num2cmo\n")); }
else if (IsMapleList(kv, alg)) { DPRINTF(("maple IsMapleList @ conv_num2cmo\n")); }
else if (IsMapleName(kv, alg)) { DPRINTF(("maple IsMapleName @ conv_num2cmo\n")); }
else if (IsMapleNULL(kv, alg)) { DPRINTF(("maple IsMapleNULL @ conv_num2cmo\n")); }
else if (IsMaplePointer(kv, alg)) { DPRINTF(("maple IsMaplePointer @ conv_num2cmo\n")); }
else if (IsMaplePointerNULL(kv, alg)) { DPRINTF(("maple IsMaplePointerNULL @ conv_num2cmo\n")); }
else if (IsMapleProcedure(kv, alg)) { DPRINTF(("maple IsMapleProcedure @ conv_num2cmo\n")); }
else if (IsMapleRTable(kv, alg)) { DPRINTF(("maple IsMapleRTable @ conv_num2cmo\n")); }
else if (IsMapleSet(kv, alg)) { DPRINTF(("maple IsMapleSet @ conv_num2cmo\n")); }
else if (IsMapleStop(kv, alg)) { DPRINTF(("maple IsMapleStop @ conv_num2cmo\n")); }
else if (IsMapleString(kv, alg)) { DPRINTF(("maple IsMapleString @ conv_num2cmo\n")); }
else if (IsMapleTable(kv, alg)) { DPRINTF(("maple IsMapleTable @ conv_num2cmo\n")); }
else if (IsMapleUnassignedName(kv, alg)) { DPRINTF(("maple IsMapleUnassignedName @ conv_num2cmo\n")); }
else if (IsMapleUnnamedZero(kv, alg)) { DPRINTF(("maple IsMapleUnnamedZero @ conv_num2cmo\n")); }
#endif

DPRINTF(("maple unknown @ convert_conv_num2cmo \n"));


	s = ToMapleName(kv, "tmp", TRUE);
	MapleAssign(kv, s, alg);

	sprintf(buf, "type(tmp, rational):");
	s = EvalMapleStatement(kv, buf);
	if (MapleToInteger32(kv, s)) {
		mpz_ptr p1, p2;
		cmo_qq *q;

		sprintf(buf, "numer(tmp):");
		p1 = MapleToGMPInteger(kv, EvalMapleStatement(kv, buf));

		sprintf(buf, "denom(tmp):");
		p2 = MapleToGMPInteger(kv, EvalMapleStatement(kv, buf));

		q = new_cmo_qq_set_mpz(p1, p2);
		return ((cmo *)q);
	}


	/* float @@TODO */

	printf("unknown type found. return NULL.\n");

	return NULL;
}


static cmo_polynomial_in_one_variable *
conv_1poly2cmo(MKernelVector kv, const char *f, cell *cl, int lv, char *buf)
{
	char buf2[10];
	int i, deg;
	ALGEB s;
	cmo *cm;
	cmo_polynomial_in_one_variable *poly;
	const char *x;


	cm = cl->cmo; /* indeterminate */
	cm = ((cmo_indeterminate *)cm)->ob; /* string */
	x = ((cmo_string *)cm)->s;

	sprintf(buf, "degree(%s,%s):", f, x);
	s = EvalMapleStatement(kv, buf);
	deg = MapleToInteger32(kv, s);
	if (deg < 0) {
		return (conv_1poly2cmo(kv, f, cl->next, lv + 1, buf));
	}

	poly = new_cmo_polynomial_in_one_variable(lv);
	for (i = deg; i >= 0; i--) {
		sprintf(buf, "%s_%d:=coeff(%s,%s,%d):", f, lv, f, x, i);
		s = EvalMapleStatement(kv, buf);
		if (!IsMapleUnnamedZero(kv, s)) {
			if (IsMapleNumeric(kv, s)) {
				cm = conv_num2cmo(kv, s);
			} else {
				sprintf(buf2, "%s_%d", f, lv);
				cm = (cmo *)conv_1poly2cmo(kv, buf2, cl->next, lv + 1, buf);
			}
			if (cm == NULL) {
				return (NULL);
			}
			list_append_monomial((cmo_list *)poly, cm, i);
		}
	}

	return (poly);
}

static cmo *
conv_matrix2cmo(MKernelVector kv, const char *f)
{
	char buf[100];
	int row;
	int col;
	int i, j;
	cmo_list *list;

	ALGEB s;


	sprintf(buf, "LinearAlgebra[RowDimension](%s):", f);
	s = EvalMapleStatement(kv, buf);
	row = MapleToInteger32(kv, s);
	
	sprintf(buf, "LinearAlgebra[ColumnDimension](%s):", f);
	s = EvalMapleStatement(kv, buf);
	col = MapleToInteger32(kv, s);


	list = new_cmo_list();
	list_append(list, (cmo *)new_cmo_int32(row));
	list_append(list, (cmo *)new_cmo_int32(col));

	/* RTableSelect を使うべきか @@TODO */
	for (i = 1; i <= row; i++) {
		for (j = 1; j <= col; j++) {
			sprintf(buf, "%s[%d,%d]:", f, i, j);
			s = EvalMapleStatement(kv, buf);
			list_append(list, convert_maple2cmo(kv, s));
		}
	}

	return ((cmo *)list);
}

static cmo *
conv_poly2cmo(MKernelVector kv, const char *f)
{
	ALGEB s, s2;
	char buf[100];

        cmo_recursive_polynomial *rec;
        cmo_polynomial_in_one_variable *poly;

        cmo_list *ringdef;
        int i;
	int varn;
	cell *cl;

	char *x;

	sprintf(buf, "%s_n := indets(%s):", f, f);
	s = EvalMapleStatement(kv, buf);

	sprintf(buf, "nops(%s_n):", f);
	s2 = EvalMapleStatement(kv, buf);

	varn = MapleToInteger32(kv, s2);

	BLOCK_NEW_CMO();
	ringdef = new_cmo_list();
	for (i = 1; i <= varn; i++) {
		cmo_indeterminate *xc;
		sprintf(buf, "%s_n[%d]:", f, i);
		s = EvalMapleStatement(kv, buf);
 		x = MapleToString(kv, s);
		xc = new_cmo_indeterminate((cmo *)new_cmo_string(x));
		list_append(ringdef, (cmo *)xc);
	}

	cl = ringdef->head->next;
	poly = conv_1poly2cmo(kv, f, cl, 0, buf);

	rec = new_cmo_recursive_polynomial(ringdef, (cmo *)poly);
	UNBLOCK_NEW_CMO();

	if (poly == NULL) {
		return (NULL);
	}

	return ((cmo *)rec);
}

static cmo *
conv_list2cmo(MKernelVector kv, ALGEB alg)
{
	int i, n;
	ALGEB s;
	char *str;
	cmo_list *list;
	cmo *c;

	s = MapleALGEB_SPrintf(kv, "nops(%a):", alg);
 	str = MapleToString(kv, s);
	n = MapleToInteger32(kv, EvalMapleStatement(kv, str));
	

	list = new_cmo_list();
	for (i = 1; i <= n; i++) {
		s = MapleListSelect(kv, alg, i);
		c = convert_maple2cmo(kv, s);
		if (c == NULL) {
			list = NULL;
			break;
		}
		list_append(list, c);
	}
	return ((cmo *)list);
}


cmo *
convert_maple2cmo(MKernelVector kv, ALGEB alg)
{
	if (IsMapleNumeric(kv, alg)) {
		return (conv_num2cmo(kv, alg));

	} else if (IsMapleString(kv, alg)) {
DPRINTF(("maplestring @ convert_maple2cmo\n"));
		char *str = MapleToString(kv, alg);
		return ((cmo *)new_cmo_string(str));
	} else if (IsMapleExpressionSequence(kv, alg)) {
DPRINTF(("mapleexpress@ convert_maple2cmo\n"));

	} else if (IsMapleAssignedName(kv, alg)) {
DPRINTF(("maple assigned name@ convert_maple2cmo\n"));

	} else if (IsMapleProcedure(kv, alg)) {
DPRINTF(("maple procedure @ convert_maple2cmo\n"));

	} else if (IsMaplePointer(kv, alg)) {
DPRINTF(("maple pointer @ convert_maple2cmo\n"));

	} else if (IsMapleTable(kv, alg)) {
DPRINTF(("maple table@ convert_maple2cmo\n"));

	} else if (IsMapleRTable(kv, alg)) {
DPRINTF(("maple rtable@ convert_maple2cmo\n"));
		ALGEB s, F;

		F = ToMapleName(kv, "F", TRUE);
		MapleAssign(kv, F, alg);

		/* matrix */
		s = EvalMapleStatement(kv, "type(F, Matrix):");
		if (MapleToInteger32(kv, s)) {
			return (conv_matrix2cmo(kv, "F"));
		}

	} else if (IsMapleNULL(kv, alg)) {
DPRINTF(("maple null@ convert_maple2cmo\n"));

	} else if (IsMapleAssignedName(kv, alg)) {
DPRINTF(("maple assigned name @ convert_maple2cmo\n"));

	} else if (IsMapleUnassignedName(kv, alg)) {
DPRINTF(("maple unassigned name @ convert_maple2cmo\n"));

	} else if (IsMapleList(kv, alg)) {
DPRINTF(("maple IsMapleList @ conv_num2cmo\n"));
		return (conv_list2cmo(kv, alg));
	} else {
DPRINTF(("mapleunknown @ convert_maple2cmo\n"));
		ALGEB s, F;

		F = ToMapleName(kv, "F", TRUE);
		MapleAssign(kv, F, alg);


		/* polynomial */
		s = EvalMapleStatement(kv, "type(F, polynom(integer)):");
		if (MapleToInteger32(kv, s)) {
			return (conv_poly2cmo(kv, "F"));
		}


	}

	return ((cmo *)new_cmo_error2((cmo *)new_cmo_string("cannot convert from mapleObj to CMO")));

}


