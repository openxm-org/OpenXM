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

void  call_gsl_sf_lngamma_complex_e() {
  cmo *c;
  double zr;
  double zi;
  gsl_sf_result lnr;
  gsl_sf_result arg;
  int status;
  cmo *r[3];
  cmo *ans;
  //  gsl_set_error_handler_off();
  gsl_set_error_handler((gsl_error_handler_t *)myhandler);
  c = pop(); // number of args
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
