/* $OpenXM: OpenXM/src/ox_gsl/call_gsl.c,v 1.7 2018/06/07 01:53:33 takayama Exp $ 
*/
//#include <gsl/gsl_types.h>
//#include <gsl/gsl_sys.h>
#include <unistd.h>
#include <gsl/gsl_sf_result.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_monte.h>
#include <gsl/gsl_monte_plain.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_monte_vegas.h>
#include "ox_gsl.h"
extern int Debug;
// local prototype declarations

void  call_gsl_sf_lngamma_complex_e() {
  int argc;
  double zr;
  double zi;
  gsl_sf_result lnr;
  gsl_sf_result arg;
  int status;
  cmo *r[3];
  cmo *ans;
  //  gsl_set_error_handler_off();
  gsl_set_error_handler((gsl_error_handler_t *)myhandler);
  argc = get_i(); // number of args
  if (argc != 2) {
    pops(argc);
    push(make_error2("The argc must be 2 for gsl_sf_lngamma_complex_e.",NULL,0,-1));
    return;
  }
  zr=get_double();
  zi=get_double();
  if (Debug) printf("gsl_sf_lngamma_complex_e(zr=%lg,zi=%lg)\n",zr,zi);
  status = gsl_sf_lngamma_complex_e(zr,zi,&lnr,&arg);
  r[0] = (cmo *)new_cmo_double(lnr.val);
  r[1] = (cmo *)new_cmo_double(arg.val);
  r[2] = (cmo *)new_cmo_int32(status);
  ans = (cmo *)new_cmo_list_array((void *)r,3);
  push(ans);
}

cmo *Func_x=NULL;  // function to evaluate.
cmo *Vlist=NULL;   // list of variables (not implemented yet).
double f_x(double x,void *params) {
  double d;
  if (Debug) ox_printf("f_x\n");
  replace(1,"x",x);
  if (Debug) ox_printf("f_x after replace x=%lg\n",x);
  if (eval_cmo(Func_x,&d)==0) GSL_ERROR("eval_cmo fails in f_x",GSL_ETOL);
  if (Debug) ox_printf("f_x(%lg) -> d=%lg\n",x,d);
  return(d);
}
void  call_gsl_integration_qags() {
  int argc;
  double a;
  double b;
  int status;
  cmo *r[3];
  cmo *ans;
  gsl_integration_workspace * w 
    = gsl_integration_workspace_alloc (1000);
  double result, error;
  gsl_function F;
  double epsabs=0;
  double epsrel=1e-7;
  int limit=1000;

  //  gsl_set_error_handler_off();
  gsl_set_error_handler((gsl_error_handler_t *)myhandler);
  argc = get_i(); // number of args
  if (argc != 3) {
    pops(argc);
    push(make_error2("The argc must be 3 for gsl_integration_qags.",NULL,0,-1));
    return;
  }
  Func_x = pop();
  a = get_double();
  b = get_double();

  F.function = &f_x;
  F.params=NULL;

  status=gsl_integration_qags (&F, a, b, epsabs, epsrel, limit,
                               w, &result, &error); 

  if (Debug) ox_printf ("result          = % .18f\n", result);
//  printf ("estimated error = % .18f\n", error);
//  printf ("intervals       = %zu\n", w->size);

  gsl_integration_workspace_free(w);

  r[0] = (cmo *)new_cmo_double(result);
  r[1] = (cmo *)new_cmo_double(error);
  r[2] = (cmo *)new_cmo_int32(status);
  ans = (cmo *)new_cmo_list_array((void *)r,3);
  push(ans);
}  

#define VEC_MAX 30
double f_n(double x[],size_t dim,void *params) {
  int debug_f_n=0;
  double d;
  int i;
  char *xx[VEC_MAX]={"x0","x1","x2","x3","x4","x5","x6","x7","x8","x9",
                "x10","x11","x12","x13","x14","x15","x16","x17","x18","x19",
                "x20","x21","x22","x23","x24","x25","x26","x27","x28","x29"
               };
//  debug_f_n = Debug;
  if (dim > VEC_MAX) GSL_ERROR("f_n supports functions with args <= VEC_MAX",GSL_ETOL);
  init_dic();
  if (debug_f_n) ox_printf("f_n, dim=%d\n",dim);
  for (i=0; i<dim; i++) {
    if (debug_f_n) ox_printf("x%d=%lg, ",i,x[i]);
    register_entry(xx[i],x[i]);
  }
  if (debug_f_n) { ox_printf("\n"); fflush(NULL); }
  if (eval_cmo(Func_x,&d)==0) GSL_ERROR("eval_cmo fails in f_n",GSL_ETOL);
  if (debug_f_n) ox_printf("\nf_x(...) -> d=%lg\n",d);
  return(d);
}
void  call_gsl_monte_plain_miser_vegas_integrate(int type) {
  int argc;
  int dim;
  int dim2;
  cmo *cr[2];
  cmo *ans;
  double *xl;
  double *xu;
  gsl_monte_function G = {&f_n,0,0};

  double res, err;
  const gsl_rng_type *T;
  gsl_rng *r;
  //  size_t calls = 500000;
  size_t calls = 100000; // for test  1000
  gsl_monte_plain_state *s_p;
  gsl_monte_miser_state *s_m;
  gsl_monte_vegas_state *s_v;
  gsl_rng_env_setup ();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);

  //  gsl_set_error_handler_off();
  gsl_set_error_handler((gsl_error_handler_t *)myhandler);
  argc = get_i(); // number of args
  if (argc < 3) {
    pops(argc);
    push(make_error2("The argc must be >=3 for gsl_integration_qags.",NULL,0,-1));
    return;
  }
  Func_x = pop();
  xl = get_double_list(&dim);
  xu = get_double_list(&dim2);
  if (argc >= 4) {
    calls = (int) get_double();
  }
  if (argc >= 5) {
    Vlist = pop();  // list of variables. Not implemented yet.
  }
  if (dim != dim2) {
    push(make_error2("gsl_monte_plain: dim of interval differs.",NULL,0,-1));
    return;
  }
  G.dim=dim;
  switch(type) {
  case 0: // plain
    s_p = gsl_monte_plain_alloc (dim);
    gsl_monte_plain_integrate (&G, xl, xu, dim, calls, r, s_p, 
                               &res, &err);
    gsl_monte_plain_free (s_p);
    break;
  case 1: // miser
    s_m = gsl_monte_miser_alloc (dim);
    gsl_monte_miser_integrate (&G, xl, xu, dim, calls, r, s_m, 
                               &res, &err);
    gsl_monte_miser_free (s_m);
    break;
  default: // vegas;
    s_v = gsl_monte_vegas_alloc (dim);
    gsl_monte_vegas_integrate (&G, xl, xu, dim, calls, r, s_v, 
                               &res, &err);
    gsl_monte_vegas_free (s_v);
    break;
  }
  if (Debug) ox_printf("result = %lg\n",res);
  cr[0] = (cmo *)new_cmo_double(res);
  cr[1] = (cmo *)new_cmo_double(err);
  ans = (cmo *)new_cmo_list_array((void *)cr,2);
  push(ans);
}
