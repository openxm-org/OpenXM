/* $OpenXM$
*/
//#include <gsl/gsl_types.h>
//#include <gsl/gsl_sys.h>
#include <unistd.h>
#include <gsl/gsl_sf_result.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv.h>
#include "ox_gsl.h"
extern int Debug;
// local prototype declarations

int GSL_ode_dim=1;
#define VEC_MAX 30
cmo *GSL_ode_func[VEC_MAX];
int f_ode(double x, const double y[], double f[], void *params)
{
  int debug_f_ode=0;
  int dim;
  int i;
  double d;
  char *yy[VEC_MAX]={"y0","y1","y2","y3","y4","y5","y6","y7","y8","y9",
                "y10","y11","y12","y13","y14","y15","y16","y17","y18","y19",
                "y20","y21","y22","y23","y24","y25","y26","y27","y28","y29"
               };
  /* result is stored in f */
  dim = GSL_ode_dim;
  if (dim > VEC_MAX) GSL_ERROR("f_n supports functions with args <= VEC_MAX",GSL_ETOL);
  init_dic();
  register_entry("x",x);
  for (i=0; i<dim; i++) {
    if (debug_f_ode) ox_printf("y%d=%lg, ",i,y[i]);
    register_entry(yy[i],y[i]);
  }
  if (debug_f_ode) { ox_printf("\n"); fflush(NULL); }
  for (i=0; i<dim; i++) {
    if (eval_cmo(GSL_ode_func[i],&d)==0) GSL_ERROR("eval_cmo fails in f_ode",GSL_ETOL);
    if (debug_f_ode) ox_printf("\nGSL_ode_func[%d](...) -> d=%lg\n",i,d);
    f[i] = d;
  }
  return GSL_SUCCESS;
}

void  call_gsl_odeiv_step(char *type) {
  int argc;
  int dim;
  cmo *f;
  cmo *y;
  double x0,x1,h;
  double abs_err = 1e-6;
  double rel_err = 1e-5;
  cmo *err;
  char *method;
  cmo **cr;
  int i;
  cmo *ans;
  const gsl_odeiv_step_type *T;
  method = type;
  //  gsl_set_error_handler_off();
  gsl_set_error_handler((gsl_error_handler_t *)myhandler);
  argc = get_i(); // number of args
  if (argc < 5) {
    pops(argc);
    push(make_error2("The argc must be >=5 for gsl_odeiv_step.",NULL,0,-1));
    return;
  }
  f = pop(); /* ff is a list of cmo's y'=f(x,y); f is a vector valued function */
  dim = get_length(f);
  GSL_ode_dim=dim;
  if (dim < 1) {
    push(make_error2("gsl_odeiv_step: the first argument must be a list",NULL,0,-1));
    return;
  }
  for (i=0; i<dim; i++) {
    GSL_ode_func[i] = element_of_at(f,i);
  }
  y = pop(); /* initial value of y (vector) */
  if (get_length(y) != dim) {
    push(make_error2("gsl_odeiv_step: dim f != dim y (initial value).",NULL,0,-1));
    return;
  }
  x0 = get_double(); // start
  x1 = get_double(); // end
  h  = get_double(); // step
  if (argc >= 6) {
    err = pop();
    // not implemented. set 
  }
  if (argc >= 7) {
    method = get_string();
  }

  if (strcmp(method,"rk4")==0) {
    T = gsl_odeiv_step_rk4;
  }else{ // default
    T = gsl_odeiv_step_rk4;
  }
  gsl_odeiv_step *s = gsl_odeiv_step_alloc(T, 2);

  gsl_odeiv_control *c = gsl_odeiv_control_y_new(abs_err, rel_err);

  gsl_odeiv_evolve *e = gsl_odeiv_evolve_alloc(dim);
  gsl_odeiv_system sys = {f_ode, NULL, dim, NULL};
  /* x : start, x1 : goal */
  double *yy;
  int tmp_len;
  yy = cmo2double_list(&tmp_len,y);

  while (x0 < x1) {
    int status = gsl_odeiv_evolve_apply(e, c, s, &sys, &x0, x1, &h, yy);
    if (status != GSL_SUCCESS) break;
    ox_printf("x=%.5e, yy[0]=%.5e, h=%lg,\n", x0, yy[0], h);
    // Todo, store intermediate values.
  }

  gsl_odeiv_evolve_free(e);
  gsl_odeiv_control_free(c);
  gsl_odeiv_step_free(s);

  cr = (cmo **) GC_malloc(sizeof(cmo *)*(dim+1));
  cr[0] = (cmo *)new_cmo_double(x0);
  for (i=1; i<=dim; i++) {
    cr[i] = (cmo *)new_cmo_double(yy[i-1]);
  }
  ans = (cmo *)new_cmo_list_array((void *)cr,dim+1);
  push(ans);
}
