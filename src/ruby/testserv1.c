
/* $OpenXM: OpenXM/src/ruby/testserv1.c,v 1.2 2000/01/28 08:24:42 ogino Exp $ */

#include <stdio.h>
/* #include <pari.h> */
#include "ox_toolkit.h"

int fd_rw = 3;

static int stack_p = 0;
static cmo *stack[100];

int explain_cmo(cmo *);

int push(cmo* m)
{
  stack[stack_p] = m;
  stack_p++;
}

cmo* pop()
{
  if (stack_p > 0) {
    stack_p--;
    return stack[stack_p];
  }
  return new_cmo_null();
}

void pops(int n)
{
  stack_p -= n;
  if (stack_p < 0) {
    stack_p = 0;
  }
}

int sm_popCMO()
{
  cmo* m = pop();

  if (m != NULL) {
    send_ox_cmo(fd_rw, m);
    return 0;
  }
  return SM_popCMO;
}

cmo_error2 *make_error2(int code)
{
  return new_cmo_int32(-1);
}

int get_i()
{
  cmo *c = pop();
  if (c->tag == CMO_INT32) {
    return ((cmo_int32 *)c)->i;
  }else if (c->tag == CMO_ZZ) {
    return mpz_get_si(((cmo_zz *)c)->mpz);
  }
  make_error2(-1);
  return 0;
}

/*
int getdisc()
{
  GEN d, x;
  int n;
  n = get_i();
fprintf(stderr, "    getdisc of %d ", n);
  x = stoi(n);
  d = quaddisc(x);
  n = itos(d);
fprintf(stderr, " = %d\n", n);
  push(new_cmo_int32(n));
}
*/
int prms[] = {
  2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997
};

int sqfr(n)
{
  int i;
  for (i = 0; prms[i] < sqrt(n); i++) {
    while(n%(prms[i]*prms[i]) == 0)
      n = n / (prms[i]*prms[i]);
  }
  return n;    
}

int getdisc(n)
{
  int sg = 1;
  if (n < 0) { sg = -1; n = -n;}
  n = sqfr(n);
  if ((n % 4 == 1) && (sg == 1))
    return n;
  else if ((n % 4 == 3) && (sg == -1))
    return -n;
  else
    return 4*n*sg;
}

int pushdisc()
{
  if (get_i() != 1) {
    push(make_error2(0));
    return -1;
  }
  push(new_cmo_int32(getdisc(get_i())));
  return 0;
}

int sm_executeFunction()
{
  cmo_string *func = (cmo_string *)pop();
  if (func->tag != CMO_STRING) {
    push(make_error2(0));
    return -1;
  }
  if (strcmp(func->s, "disc") == 0) {
    /* fprintf(stderr, "%s\n", "   getdisc"); */
    pushdisc();
  }else {
    push(make_error2(0));
    return -1;
  }
}


int receive_and_execute_sm_command()
{
  int code = receive_int32(fd_rw);
  switch(code) {
  case SM_popCMO:
    fprintf(stderr, "%s\n", " SM_popCMO");
    sm_popCMO();
    break;
  case SM_executeFunction:
    fprintf(stderr, "%s\n", " SM_executeFunction");
    sm_executeFunction();
    break;
  default:
  }
}

int receive()
{
  int tag;

  tag = receive_ox_tag(fd_rw);
  switch(tag) {
  case OX_DATA:
    push(receive_cmo(fd_rw));
    break;
  case OX_COMMAND:
    receive_and_execute_sm_command();
    break;
  default:
  }
  return 0;
}

int main()
{
  decideByteOrderServer(fd_rw, 0);

  /*    pari_init (200000, 2000); */
  while(1) {
    receive();
  }
}
