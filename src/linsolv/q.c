#include "lin.h"

Coef one_q()
{
  Coef r;

  NEWQ(r.q);
  mpq_init(r.q);
  mpq_set_si(r.q,1,1);
  return r;
}

void print_mpq(mpq_ptr a)
{
  mpq_out_str(stdout,10,a);
  printf("\n");
}

void print_q(Coef a)
{
  if ( a.q == 0 )
    printf("0");
  else
    mpq_out_str(stdout,10,a.q);
}

int zero_q(Coef t)
{
  return t.q == 0;
}

Coef mpqtoc(mpq_ptr t)
{
  Coef z;

  if ( mpq_sgn(t) == 0 ) {
    z.q = 0;
    return z;
  } else {
    NEWQ(z.q);
    z.q[0] = t[0];
    return z;
  }
}

Coef add_q(Coef a,Coef b)
{
  mpq_t t;

  if ( !a.q ) return b;
  else if ( !b.q ) return a;
  else {
    mpq_init(t); mpq_add(t,a.q,b.q);
    return mpqtoc(t);
  }
}

Coef neg_q(Coef a)
{
  mpq_t t;

  if ( a.q == 0 ) return a;
  else {
    mpq_init(t); mpq_neg(t,a.q);
    return mpqtoc(t);
  }
}

Coef sub_q(Coef a,Coef b)
{
  mpq_t t;

  if ( !a.q ) return neg_q(b);
  else if ( !b.q ) return a;
  else {
    mpq_init(t); mpq_sub(t,a.q,b.q);
    return mpqtoc(t);
  }
}

Coef mul_q(Coef a,Coef b)
{
  mpq_t t;

  if ( a.q == 0 ) return a;
  else if ( b.q == 0 ) return b;
  else {
    mpq_init(t); mpq_mul(t,a.q,b.q);
    return mpqtoc(t);
  }
}

Coef div_q(Coef a,Coef b)
{
  mpq_t t;

  if ( b.q == 0 )
    error("div_q : division by 0");
  if ( a.q == 0 ) return a;
  else {
    mpq_init(t); mpq_div(t,a.q,b.q);
    return mpqtoc(t);
  }
}

Coef ntoc_q(char *n)
{
  mpq_t t;

  mpq_init(t);
  mpq_set_str(t,n,10);
  return mpqtoc(t);
}
