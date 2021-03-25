#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pari/pari.h"
#if 0
#include "pari/paripriv.h"
#endif
#include "gmp.h"
/* inconsistency between PARI and GMP */
#undef un
#include "gmp-impl.h"
#include "mpfr.h"
#include "ox_toolkit.h"

void init_pari(void);
cmo_error2 *make_error2(char *message);

int gmp_check(void);
cmo *GEN_to_cmo(GEN z);
cmo_zz *GEN_to_cmo_zz(GEN z);
cmo_qq *GEN_to_cmo_qq(GEN z);
cmo_bf *GEN_to_cmo_bf(GEN z);
cmo_list *GEN_to_cmo_list(GEN z);
cmo_complex *GEN_to_cmo_cmo_complex(GEN z);
cmo_polynomial_in_one_variable *GEN_to_cmo_up(GEN z);
cmo_recursive_polynomial *GEN_to_cmo_rp(GEN z);

GEN cmo_to_GEN(cmo *c);
GEN cmo_int32_to_GEN(cmo_int32 *c);
GEN cmo_zz_to_GEN(cmo_zz *c);
GEN cmo_qq_to_GEN(cmo_qq *c);
GEN cmo_bf_to_GEN(cmo_bf *c);
GEN cmo_list_to_GEN(cmo_list *c);
GEN cmo_rp_to_GEN(cmo_recursive_polynomial *c);
GEN cmo_up_to_GEN(cmo_polynomial_in_one_variable *c);
GEN cmo_complex_to_GEN(cmo_complex *c);

struct parif *search_parif(char *name);

#define PARI_MAX_AC 64
struct parif {
  char *name;
  GEN (*f)();
  int type;
  int opt;
};

#define MPFR_PREC(x)      ((x)->_mpfr_prec)
#define MPFR_EXP(x)       ((x)->_mpfr_exp)
#define MPFR_MANT(x)      ((x)->_mpfr_d)
#define MPFR_LAST_LIMB(x) ((MPFR_PREC (x) - 1) / GMP_NUMB_BITS)
#define MPFR_LIMB_SIZE(x) (MPFR_LAST_LIMB (x) + 1)

