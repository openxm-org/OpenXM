/* $OpenXM$
*/
//#include <gsl/gsl_types.h>
//#include <gsl/gsl_sys.h>
#include <unistd.h>
#include <gsl/gsl_sf_result.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf_gamma.h>
#include "ox_gsl.h"
extern int Debug;
// local prototype declarations
#include "call_gsl_sf.h"

void  call_gsl_sf_gamma_inc() {
  int argc;
  double a;
  double x;
  double val;
  cmo *ans;
  //  gsl_set_error_handler_off();
  gsl_set_error_handler((gsl_error_handler_t *)myhandler);
  argc = get_i(); // number of args
  if (argc != 2) {
    pops(argc);
    push(make_error2("The argc must be 2 for gsl_sf_gamma_inc.",NULL,0,-1));
    return;
  }
  a=get_double();
  x=get_double();
  if (Debug) printf("gsl_sf_gamma_inc(a=%lg,x=%lg)\n",a,x);
  val = gsl_sf_gamma_inc(a,x);
  ans = (cmo *)new_cmo_double(val);
  push(ans);
}
