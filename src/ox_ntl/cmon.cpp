/* $OpenXM$ */

#include <NTL/ZZX.h>

#include "ntl.h"

/****************************************************************************
 * Construct
 ****************************************************************************/
cmon_zz_t *
new_cmon_zz(ZZ &z)
{
	cmon_zz_t *p = new_cmon_zz();
	*(p->z) = z; 
	return (p);
}

cmon_zz_t *
new_cmon_zz(void)
{
	cmon_zz_t *p = (cmon_zz_t *)malloc(sizeof(cmon_zz_t));
	p->tag = CMON_ZZ;
	p->z = new ZZ();
	return (p);
}

cmon_zzx_t *
new_cmon_zzx(ZZX &f, cmo_indeterminate *x)
{
	cmon_zzx_t *p = new_cmon_zzx();
	*(p->f) = f;
	p->x = x;
	return (p);
}

cmon_zzx_t *
new_cmon_zzx(void)
{
	cmon_zzx_t *p = (cmon_zzx_t *)malloc(sizeof(cmon_zzx_t));
	p->tag = CMON_ZZX;
	p->f = new ZZX();
	return (p);
}




cmon_factors_t *
new_cmon_factors(ZZ &cont, vec_pair_ZZX_long &factors, cmo_indeterminate *x)
{
	cmon_factors_t *p = new_cmon_factors();
	*(p->cont) = cont;
	*(p->f) = factors;
	p->x = x;
	return (p);
}

cmon_factors_t *
new_cmon_factors(void)
{
	cmon_factors_t *p = (cmon_factors_t *)malloc(sizeof(cmon_factors_t));
	p->tag = CMON_FACTORS;
	p->cont = new ZZ();
	p->f = new vec_pair_ZZX_long();
	return (p);
}

cmon_mat_zz_t *
new_cmon_mat_zz(mat_ZZ &m)
{
	cmon_mat_zz_t *p = new_cmon_mat_zz();
	*(p->mat) = m;
	return (p);
}

cmon_mat_zz_t *
new_cmon_mat_zz(void)
{
	cmon_mat_zz_t *p = (cmon_mat_zz_t *)malloc(sizeof(cmon_mat_zz_t));
	p->tag = CMON_MAT_ZZ;
	p->mat = new mat_ZZ();
	return (p);
}



/****************************************************************************
 * Destruct
 ****************************************************************************/
void
delete_cmon_zz(cmon_zz_t *p)
{
	delete(p->z);
	free(p);
}

void
delete_cmon_zzx(cmon_zzx_t *p)
{
	delete(p->f);
	free(p);
}


void
delete_cmon_factors(cmon_factors_t *p)
{
	delete(p->cont);
	delete(p->f);
	free(p);
}

void
delete_cmon_mat_zz(cmon_mat_zz_t *p)
{
	delete (p->mat);
	free(p);
}

/***
 * PARAM  : p is not null pointer.
 ************/
void
delete_cmon(cmo *p)
{
	switch (p->tag) {
	case CMON_ZZ:
		delete_cmon_zz((cmon_zz_t *)p);
		break;
	case CMON_ZZX:
		delete_cmon_zzx((cmon_zzx_t *)p);
		break;
	case CMON_FACTORS:
		delete_cmon_factors((cmon_factors_t *)p);
		break;
	case CMON_MAT_ZZ:
		delete_cmon_mat_zz((cmon_mat_zz_t *)p);
		break;
	}
}


