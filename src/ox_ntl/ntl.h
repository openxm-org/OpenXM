/* $OpenXM: OpenXM/src/ox_ntl/ntl.h,v 1.1 2003/11/03 03:11:21 iwane Exp $ */

#ifndef __NTL_H__
#define __NTL_H__

#include <NTL/ZZXFactoring.h>
#include "ox_toolkit.h"

#define NTL_FAILURE	(-1)
#define NTL_SUCCESS	( 0)

/*===========================================================================*
 * CONVERT
 *===========================================================================*/
cmo_zz		*ZZ_to_cmo_zz	(const ZZ &);
int		 cmo_to_ZZ	(ZZ &, cmo *);
int		 cmo_to_ZZX	(ZZX &, cmo *, cmo_indeterminate *&);
cmo_recursive_polynomial	*ZZX_to_cmo	(ZZX &, cmo_indeterminate *);
cmo_list	*ZZX_int_to_cmo	(ZZX &, int d, cmo_indeterminate *);
cmo_list	*vec_pair_ZZX_long_to_cmo	(vec_pair_ZZX_long &, cmo_indeterminate *);

/*===========================================================================*
 * NTL
 *===========================================================================*/
cmo	*ntl_fctr	(cmo **, int);

#endif /* !__NTL_H__*/
