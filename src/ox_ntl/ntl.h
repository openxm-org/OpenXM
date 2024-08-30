/* $OpenXM: OpenXM/src/ox_ntl/ntl.h,v 1.5 2008/09/19 10:55:40 iwane Exp $ */

#ifndef __NTL_H__
#define __NTL_H__

#include <NTL/ZZXFactoring.h>
#include <NTL/mat_ZZ.h>
#include "ox_toolkit.h"
#include "oxserv.h"	/* for block interrupt input */
#include "oxstack.h"

#define NTL_FAILURE	(-1)
#define NTL_SUCCESS	( 0)

/*===========================================================================*
 * Local CMO
 *===========================================================================*/
#define	CMON_MAT_ZZ          (CMO_PRIVATE + 1)
#define	CMON_ZZ              (CMO_PRIVATE + 2)
#define	CMON_ZZX             (CMO_PRIVATE + 3)
#define	CMON_FACTORS         (CMO_PRIVATE + 4)

using namespace NTL;

typedef struct cmon_mat_zz_tag {
	int tag;
	mat_ZZ *mat;
} cmon_mat_zz_t;


typedef struct cmon_zz_tag {
	int tag;
	ZZ *z;
} cmon_zz_t;


typedef struct cmon_zzx_tag {
	int tag;
	ZZX *f;
	cmo_indeterminate *x;
} cmon_zzx_t;


typedef struct cmon_factors_tag {
	int tag;
	ZZ  *cont;		/* cont    */
	vec_pair_ZZX_long *f;	/* factors */
	cmo_indeterminate *x;
} cmon_factors_t;


/*** cmon.cpp ***/

/*===========================================================================*
 * Construct
 *===========================================================================*/
cmon_zz_t	*	new_cmon_zz	(void);
cmon_zz_t	*	new_cmon_zz	(ZZ &);
cmon_zzx_t	*	new_cmon_zzx	(void);
cmon_zzx_t	*	new_cmon_zzx	(ZZX &, cmo_indeterminate *x);
cmon_mat_zz_t	*	new_cmon_mat_zz	(void);
cmon_mat_zz_t	*	new_cmon_mat_zz	(mat_ZZ &);
cmon_factors_t	*	new_cmon_factors	(void);
cmon_factors_t	*	new_cmon_factors	(ZZ &, vec_pair_ZZX_long &, cmo_indeterminate *x);


/*===========================================================================*
 * Destruct
 *===========================================================================*/
void	delete_cmon_zz	(cmon_zz_t *);
void	delete_cmon_zzx	(cmon_zzx_t *);
void	delete_cmon_mat_zz	(cmon_mat_zz_t *);
void	delete_cmon_factors	(cmon_factors_t *);
void	delete_cmon	(cmo *);


/*** ntlconv.cpp ***/

/*===========================================================================*
 * CONVERT
 *===========================================================================*/
int		get_cmon_tag	(cmo *);
cmo	*	convert_cmon	(cmo *);	/* convert local object to CMO */

cmo_zz		*ZZ_to_cmo_zz	(const ZZ &);
int		 cmo_to_ZZ	(ZZ &, cmo *);
int		 cmo_to_ZZX	(ZZX &, cmo *, cmo_indeterminate *&);
cmo_recursive_polynomial	*ZZX_to_cmo	(ZZX &, cmo_indeterminate *);
cmo_list	*ZZX_int_to_cmo	(ZZX &, int d, cmo_indeterminate *);
cmo_list	*vec_pair_ZZX_long_to_cmo	(vec_pair_ZZX_long &, cmo_indeterminate *);

int		 cmo_to_mat_zz	(mat_ZZ &, cmo *);
cmo_list	*mat_zz_to_cmo	(mat_ZZ &);


/*** ntl.cpp ***/

/*===========================================================================*
 * NTL
 *===========================================================================*/
oxstack_node	*ntl_fctr	(oxstack_node **, int);
oxstack_node	*ntl_lll 	(oxstack_node **, int);

#endif /* !__NTL_H__*/
