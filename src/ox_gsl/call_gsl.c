/* $OpenXM: OpenXM/src/ox_gsl/call_gsl.c,v 1.2 2018/03/30 04:43:16 takayama Exp $ 
*/
//#include <gsl/gsl_types.h>
//#include <gsl/gsl_sys.h>
#include <unistd.h>
#include <gsl/gsl_sf_result.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_integration.h>
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

cmo *Func_x=NULL;
double f_x(double x,void *params) {
  double d;
  replace(1,"x",x);
  if (eval_cmo(Func_x,&d)==0) GSL_ERROR("eval_cmo fails in f_x",GSL_ETOL);
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

  status=gsl_integration_qags (&F, a, b, epsabs, epsrel, limit,
                               w, &result, &error); 

//  printf ("result          = % .18f\n", result);
//  printf ("estimated error = % .18f\n", error);
//  printf ("intervals       = %zu\n", w->size);

  gsl_integration_workspace_free(w);

  r[0] = (cmo *)new_cmo_double(result);
  r[1] = (cmo *)new_cmo_double(error);
  r[2] = (cmo *)new_cmo_int32(status);
  ans = (cmo *)new_cmo_list_array((void *)r,3);
  push(ans);
}  
