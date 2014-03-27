#ifndef _KO_FBD
#define _KO_FBD

extern double *ko_fbd( double **x, double *y);
/* following global variable defined in ko-fbd.c  */
extern int dim;  /* dimension of the sphere */
extern double eps_pert;
extern int flag_verbose;
extern int flag_confidence;
extern int flag_time;
/* following global variable defined in ko-fbd-ps.c  */
extern int maxdeg;
#endif
