#include "ox_pari.h"

GEN cmo_int32_to_GEN(cmo_int32 *c)
{
  GEN z;
  int i,sgn;

  i = c->i;
  if ( !i ) return gen_0;
  z = cgeti(3);
  sgn = 1;
  if ( i < 0 ) {
    i = -i;
    sgn = -1;
  }
  z[2] = i;
  setsigne(z,sgn);
  setlgefint(z,lg(z));
  return z;
}

GEN cmo_string_to_GEN(cmo_string *c)
{
  GEN z;
  int l;

  l = strlen(c->s);
  z = cgetg(l+1,t_STR);
  strcpy(GSTR(z),c->s);
  return z;
}

GEN cmo_zz_to_GEN(cmo_zz *c)
{
  mpz_ptr mpz;
  GEN z;
  long *ptr;
  int j,sgn,len;

  mpz = c->mpz;
  sgn = mpz_sgn(mpz);
  len = ABSIZ(mpz);
  ptr = (long *)PTR(mpz);
  z = cgeti(len+2);
  for ( j = 0; j < len; j++ )
    z[len-j+1] = ptr[j];
  setsigne(z,sgn);
  setlgefint(z,lg(z));
  return z;
}

GEN cmo_qq_to_GEN(cmo_qq *c)
{
  GEN z,nm,den;
  
  z = cgetg(3,t_FRAC);
  nm = cmo_zz_to_GEN(new_cmo_zz_set_mpz(mpq_numref(c->mpq)));
  den = cmo_zz_to_GEN(new_cmo_zz_set_mpz(mpq_denref(c->mpq)));
  z[1] = (long)nm;
  z[2] = (long)den;
  return z;
}

GEN cmo_bf_to_GEN(cmo_bf *c)
{
  mpfr_ptr mpfr;
  GEN z;
  int sgn,len,j;
  long exp;
  long *ptr;

  mpfr = c->mpfr;
  sgn = MPFR_SIGN(mpfr);
  exp = MPFR_EXP(mpfr)-1;
  len = MPFR_LIMB_SIZE(mpfr);
  ptr = (long *)MPFR_MANT(mpfr);
  z = cgetr(len+2);
  for ( j = 0; j < len; j++ )
    z[len-j+1] = ptr[j];
  z[1] = evalsigne(sgn)|evalexpo(exp);
  setsigne(z,sgn);
  return z;
}

/* list->vector */

GEN cmo_list_to_GEN(cmo_list *c)
{
  GEN z;
  int i;
  cell *cell;

  z = cgetg(c->length+1,t_VEC);
  for ( i = 0, cell = c->head->next; cell != c->head; cell = cell->next, i++ ) {
    z[i+1] = (long)cmo_to_GEN(cell->cmo);
  }
  return z;
}

GEN cmo_complex_to_GEN(cmo_complex *c)
{
  GEN z;

  z = cgetg(3,t_COMPLEX);
  z[1] = (long)cmo_to_GEN(c->re);
  z[2] = (long)cmo_to_GEN(c->im);
  return z;
}

GEN cmo_up_to_GEN(cmo_polynomial_in_one_variable *c)
{
  GEN z;
  int d,i;
  cell *cell;

  d = c->head->next->exp;
  z = cgetg(d+3,t_POL);
  setsigne(z,1);
  setvarn(z,c->var);
  setlg(z,d+3);
  for ( i = 2; i <= d+2; i++ )
    z[i] = (long)gen_0;
  for ( cell = c->head->next; cell != c->head; cell = cell->next ) {
    z[2+cell->exp] = (long)cmo_to_GEN(cell->cmo);
  }
  return z;
}

cmo_list *current_ringdef;

void register_variables(cmo_list *ringdef)
{
  current_ringdef = ringdef;
}

GEN cmo_rp_to_GEN(cmo_recursive_polynomial *c)
{
  register_variables(c->ringdef);
  switch ( c->coef->tag ) {
  case CMO_ZERO:
  case CMO_NULL:
    return gen_0;
  case CMO_INT32:
    return cmo_int32_to_GEN((cmo_int32 *)c->coef);
  case CMO_ZZ:
    return cmo_zz_to_GEN((cmo_zz *)c->coef);
  case CMO_QQ:
    return cmo_qq_to_GEN((cmo_qq *)c->coef);
  case CMO_POLYNOMIAL_IN_ONE_VARIABLE:
    return cmo_up_to_GEN((cmo_polynomial_in_one_variable *)c->coef);
  default:
    return 0;
  }
}

cmo_zz *GEN_to_cmo_string(GEN z)
{
  cmo_string *c;

  c = new_cmo_string(GSTR(z));
  return (cmo_zz *)c;
}

cmo_zz *GEN_to_cmo_zz(GEN z)
{
  cmo_zz *c;

  c = new_cmo_zz();
  mpz_import(c->mpz,lg(z)-2,1,sizeof(long),0,0,&z[2]);
  if ( signe(z) < 0 )
    mpz_neg(c->mpz,c->mpz);
  return c;
}

cmo_qq *GEN_to_cmo_qq(GEN z)
{
  cmo_qq *c;
  GEN num,den;

  num = (GEN)z[1];
  den = (GEN)z[2];
  c = new_cmo_qq();
  mpz_import(mpq_numref(c->mpq),lg(num)-2,1,sizeof(long),0,0,&num[2]);
  mpz_import(mpq_denref(c->mpq),lg(den)-2,1,sizeof(long),0,0,&den[2]);
  if ( signe(num)*signe(den) < 0 )
    mpz_neg(mpq_numref(c->mpq),mpq_numref(c->mpq));
  return c;
}


cmo_bf *GEN_to_cmo_bf(GEN z)
{
  cmo_bf *c;
  int len,prec,j;
  long *ptr;

  c = new_cmo_bf();
  len = lg(z)-2;
  prec = len*sizeof(long)*8;
  mpfr_init2(c->mpfr,prec);
  ptr = (long *)MPFR_MANT(c->mpfr);
  for ( j = 0; j < len; j++ )
    ptr[j] = z[len-j+1];
  MPFR_EXP(c->mpfr) = (long long)(expo(z)+1);
  MPFR_SIGN(c->mpfr) = gsigne(z);
  if ( !mpfr_sgn(c->mpfr) ) c = (cmo_bf *)new_cmo_zero();
  return c;
}


cmo_list *GEN_to_cmo_list(GEN z)
{
  cmo_list *c;
  cmo *ob;
  int i,len;

  c = new_cmo_list();
  len = lg(z)-1;
  for ( i = 1; i <= len; i++ ) {
    ob = GEN_to_cmo((GEN)z[i]);
    c = list_append(c,ob);
  }
  return c;
}

cmo_complex *GEN_to_cmo_complex(GEN z)
{
  cmo_complex *c;
  cmo_bf *re,*im;

  re = (cmo_bf *)GEN_to_cmo((GEN)z[1]);
  im = (cmo_bf *)GEN_to_cmo((GEN)z[2]);
  if ( im->tag == CMO_ZERO )
    return (cmo_complex *)re;
  else {
    c = new_cmo_complex();
    c->re = (cmo *)re; c->im = (cmo *)im;
    return c;
  }
}

cmo_polynomial_in_one_variable *GEN_to_cmo_up(GEN z)
{
  cmo_polynomial_in_one_variable *c;
  int i;
  cmo *coef;

  c = new_cmo_polynomial_in_one_variable(varn(z));
  for ( i = lg(z)-1; i >= 2; i-- )
    if ( (GEN)z[i] != gen_0 ) {
      coef = GEN_to_cmo((GEN)z[i]);
      list_append_monomial((cmo_list *)c, coef, i-2);
    }
  return c;
}

cmo_recursive_polynomial *GEN_to_cmo_rp(GEN z)
{
  cmo_recursive_polynomial *c;

  if ( !signe(z) ) return (cmo_recursive_polynomial *)new_cmo_zero();
  c = new_cmo_recursive_polynomial(current_ringdef,(cmo *)GEN_to_cmo_up(z));
  return c;
}

GEN cmo_to_GEN(cmo *c)
{
  switch ( c->tag ) {
  case CMO_ZERO:
  case CMO_NULL:
    return gen_0;
  case CMO_ZZ: /* int */
    return cmo_zz_to_GEN((cmo_zz *)c);
  case CMO_QQ:
    return cmo_qq_to_GEN((cmo_qq *)c);
  case CMO_COMPLEX: /* complex */
    return cmo_complex_to_GEN((cmo_complex *)c);
  case CMO_IEEE_DOUBLE_FLOAT:
     return dbltor(((cmo_double *)c)->d);
  case CMO_BIGFLOAT32: /* bigfloat */
    return cmo_bf_to_GEN((cmo_bf *)c);
  case CMO_LIST:
    return cmo_list_to_GEN((cmo_list *)c);
  case CMO_RECURSIVE_POLYNOMIAL:
    return cmo_rp_to_GEN((cmo_recursive_polynomial *)c);
  case CMO_POLYNOMIAL_IN_ONE_VARIABLE:
    return cmo_up_to_GEN((cmo_polynomial_in_one_variable *)c);
  case CMO_STRING:
    return cmo_string_to_GEN((cmo_string *)c);
  default:
    return 0;
  }
}

cmo *GEN_to_cmo(GEN z)
{
  char buf[BUFSIZ];

  if ( gcmp0(z) )
    return new_cmo_zero();
  switch ( typ(z) ) {
  case t_INT: /* int */
    return (cmo *)GEN_to_cmo_zz(z);
  case t_REAL: /* bigfloat */
    return (cmo *)GEN_to_cmo_bf(z);
  case t_FRAC: /* rational number */
    return (cmo *)GEN_to_cmo_qq(z);
  case t_COMPLEX: /* complex */
    return (cmo *)GEN_to_cmo_complex(z);
  case t_POL:
    return (cmo *)GEN_to_cmo_rp(z);
  case t_VEC: case t_COL: /* vector */
    return (cmo *)GEN_to_cmo_list(z);
  case t_MAT: /* matrix */
    return (cmo *)GEN_to_cmo_list(shallowtrans(z));
  case t_STR: /* string */
    return (cmo *)GEN_to_cmo_string(z);
  default:
    sprintf(buf,"GEN_to_cmo : unsupported type=%d",(int)typ(z));
    return (cmo *)make_error2(buf);
  }
}
