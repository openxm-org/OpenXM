#include "lin.h"

Coef one_ff()
{
  Coef c;
  c.f = 1;
  return c;
}

void print_ff(Coef a)
{
  printf("%lld",a.f);
}

int zero_ff(Coef t)
{
  return t.f == 0;
}

Coef add_ff(Coef a,Coef b)
{
  LONG c,mod;
  Coef r;

  mod = CurrentRing->chr;
  c = (a.f+b.f)%mod;
  if ( c < 0 ) c += mod;
  r.f = c;
  return r;
}

Coef neg_ff(Coef a)
{
  LONG c,mod;
  Coef r;

  mod = CurrentRing->chr;
  c = (mod-a.f)%mod;
  if ( c < 0 ) c += mod;
  r.f = c;
  return r;
}

Coef sub_ff(Coef a,Coef b)
{
  LONG c,mod;
  Coef r;

  mod = CurrentRing->chr;
  c = (a.f-b.f)%mod;
  if ( c < 0 ) c += mod;
  r.f = c;
  return r;
}

Coef mul_ff(Coef a,Coef b)
{
  LONG c,mod;
  Coef r;

  if ( a.f == 0 || b.f == 0 ) return a;
  mod = CurrentRing->chr;
  c = (a.f*b.f)%mod;
  if ( c < 0 ) c += mod;
  r.f = c;
  return r;
}

// c-a*b
Coef mulsub_ff(Coef c,Coef a,Coef b)
{
  LONG d,mod;
  Coef r;

  if ( a.f == 0 || b.f == 0 ) return c;
  mod = CurrentRing->chr;
  d = (c.f-a.f*b.f) % mod;
  if ( d < 0 ) d += mod;
  r.f = d;
  return r;
}

Coef inv_ff(Coef a)
{
  LONG f1,f2,a1,a2,q,r,mod;
  Coef u;

  mod = CurrentRing->chr;
  f1 = a.f; f2 = mod;
  a1 = 1; a2 = 0;
  while ( 1 ) {
    q = f1/f2; r = f1 - f2*q; 
    f1 = f2; f2 = r;
    if ( r == 0 ) 
      break;
    r = (a2*q)%mod;
    r = a1-r;
    if ( r < 0 ) r += mod;
    a1 = a2; a2 = r;
  }
  u.f = a2;
  return u;
}

Coef div_ff(Coef a,Coef b)
{
  LONG c,mod;
  Coef inv,r;

  if ( b.f == 0 )
    error("div_ff : division by 0");
  if ( a.f == 0 ) return a;
  inv = inv_ff(b);
  mod = CurrentRing->chr;
  c = (a.f*inv.f)%mod;
  if ( c < 0 ) c += mod;
  r.f = c;
  return r;
}


Coef ntoc_ff(char *n)
{
  LONG a,mod;
  Coef r;

  mod = CurrentRing->chr;
  a = strtol(n,0,10)%mod;
  if ( a < 0 ) a += mod;
  r.f = a;
  return r;
}
