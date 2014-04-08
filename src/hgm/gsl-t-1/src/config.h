#define HAVE_EXTENDED_PRECISION_REGISTERS 0
#if HAVE_EXTENDED_PRECISION_REGISTERS
#define GSL_COERCE_DBL(x) (gsl_coerce_double(x))
#else
#define GSL_COERCE_DBL(x) (x)
#endif

#define RETURN_IF_NULL(x) if (!x) { return ; }

/* gsl_machine.h */
#define GSL_DBL_MIN        2.2250738585072014e-308
#define DBL_MIN  GSL_DBL_MIN   /* OK? cf. ieee-utils/test.c */

#include "t-gsl_sys.h"     /* double gsl_coerce_double(double) is declared here */

