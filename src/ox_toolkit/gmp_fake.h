/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */

#ifndef __GMP_FAKE_H__
#define __GMP_FAKE_H__

typedef unsigned mp_limb_t;
typedef int mp_size_t;

typedef struct {
/*  int _mp_alloc; */
    int _mp_size;
    mp_limb_t *_mp_d;
} __mpz_struct; 

typedef __mpz_struct mpz_t[1];
typedef __mpz_struct *mpz_ptr;
typedef mpz_ptr mpz_srcptr;

void *_mpz_realloc(mpz_ptr, size_t);
void mpz_init(mpz_ptr);
int  mpz_init_set_str(mpz_ptr, char *, int);
void mpz_set(mpz_ptr, mpz_ptr);
void mpz_set_si(mpz_ptr, int);
int  mpz_set_str(mpz_ptr, char *, int);
char *mpz_get_str(char *, int, mpz_ptr);
int  mpz_get_si(mpz_ptr z);
void mpz_neg(mpz_ptr, mpz_ptr);

#endif /* __GMP_FAKE_H__ */
