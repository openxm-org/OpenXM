/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_toolkit/gmp_fake.c,v 1.1 2003/03/30 08:10:57 ohara Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gmp_fake.h"

#define DEFAULT_LIMB_SIZE 1
#define MALLOC(x) malloc((x))
#define MALLOC_ATOMIC(x) malloc((x))
#define REALLOC(p,x) realloc((p),(x))
#define ALLOCA(x) alloca((x))

#if !defined(CHAR_BIT)
#define CHAR_BIT 8
#endif

#define HAVE_UNSIGNED_LONG_LONG

#if defined(HAVE_UNSIGNED_LONG_LONG)
typedef unsigned long long ulong64;
#elif defined(SIZEOF_LONG) && SIZEOF_LONG == 8
typedef unsigned long ulong64;
#endif

#define RADIUS     ((ulong64)1 << ((CHAR_BIT)*sizeof(mp_limb_t)))
#define LL_HIGH(x) ((x)>>((CHAR_BIT)*sizeof(mp_limb_t)))
#define LL_LOW(x)  ((x) & ((RADIUS)-1))

inline static void __mpz_clear(mpz_ptr z)
{
    int size = abs(z->_mp_size);
    z->_mp_size = size;
    memset(z->_mp_d, 0, sizeof(mp_limb_t)*size);
}

void *_mpz_realloc(mpz_ptr z, size_t n)
{
    mp_limb_t *p = REALLOC(z->_mp_d, n*sizeof(mp_limb_t));
    if (p != NULL) {
        z->_mp_d = p;
    }
    return p;
}

void mpz_init(mpz_ptr z)
{
    z->_mp_size = DEFAULT_LIMB_SIZE;
    z->_mp_d    = MALLOC_ATOMIC(DEFAULT_LIMB_SIZE);
    __mpz_clear(z);
}

void mpz_init_set(mpz_ptr z, mpz_ptr src)
{
    mpz_init(z);
    mpz_set(z, src);
}

void mpz_set_si(mpz_ptr z, int i)
{
    __mpz_clear(z);
    z->_mp_size = (i < 0)? -z->_mp_size: z->_mp_size;
    z->_mp_d[0] = abs(i);
}

void mpz_set(mpz_ptr dest, mpz_ptr src)
{
    int dsize=abs(dest->_mp_size);
    int ssize=abs(src->_mp_size);
    if (dsize < ssize) {
        _mpz_realloc(dest, ssize);
        dest->_mp_size = src->_mp_size;
    }else {
        __mpz_clear(dest);
        dest->_mp_size = (src->_mp_size < 0)? -dsize: dsize;
    }       
    memcpy(dest->_mp_d, src->_mp_d, ssize*sizeof(mp_limb_t));
}

inline static void __mpz_neg(mpz_ptr z)
{
    z->_mp_size *= -1;
}

void mpz_neg(mpz_ptr dest, mpz_ptr src)
{
    mpz_set(dest, src);
    __mpz_neg(dest);
}

int  mpz_init_set_str(mpz_ptr z, char *nptr, int base)
{
    mpz_init(z);
    mpz_set_str(z, nptr, base);
    return 0;
}

/* base admits 0, 8, 10, or 16 */
char *mpz_get_str(char *s, int base, mpz_ptr src)
{
    mpz_t z;
    int i, j, len;
    mp_limb_t res;
    char *t;
    ulong64 uu;

    if (base != 10 && base != 16 && base != 8) {
        base = 10;
    }
    /* log2(8)=3 < log2(10) < log2(16)=4 */
    len = ((CHAR_BIT)*sizeof(mp_limb_t)*abs(src->_mp_size))/((base==16)? 4: 3)+4;
    mpz_init(z);
    mpz_set(z, src);
    t = ALLOCA(len+1);
    t[len] = '\0';
    for(i=len-1; i>=0; i--) {
        res = 0;
        /* res := z % base, z := z / base  */
        for(j=abs(z->_mp_size)-1; j>=0; j--) {
            uu  = RADIUS*res + z->_mp_d[j]; 
            res = uu % base;
            z->_mp_d[j] = uu / base;
        }
        t[i] = res + ((res < 10)? '0' : ('a'-10));
    }
    /* skip 0's */
    while (t[++i] == '0') {
    }
    if (t[i] == '\0' || base == 8) {
        t[--i] = '0';
    }else if (base == 16){
        t[--i] = 'x';
        t[--i] = '0';
    }
    if (src->_mp_size < 0) {
        t[--i] = '-';
    }
    len = strlen(t+i)+1;
    if (s == NULL) {
        s = MALLOC_ATOMIC(len);
    }
    memcpy(s, t+i, len);
    return s;
}

int  mpz_get_si(mpz_ptr z)
{
    return (z->_mp_size < 0)? - z->_mp_d[0]: z->_mp_d[0];
}

/* base admits 0, 8, 10, or 16 */
int mpz_set_str(mpz_ptr z, char *s, int base)
{
    unsigned char c;
    int neg = 0;
    int len, i, size;
    mp_limb_t res;
    ulong64 uu;
    char *t = s;

    do {
        c = *t++;
    } while (isspace(c));
    if (c == '-') {
        neg = 1;
        c = *t++;
    }else if (c == '+') {
        c = *t++;
    }
    if ((base == 0 || base == 16) &&
        c == '0' && (*t == 'x' || *t == 'X')) {
        c = t[1];
        t += 2;
        base = 16;
    }
    if (base == 0) {
        base = (c == '0')? 8: 10;
    }
    __mpz_clear(z);
    /* log2(8)=3 < log2(10) < log2(16)=4 */
    len = (((base == 8)? 3: 4)*(strlen(t)+1))/((CHAR_BIT)*sizeof(mp_limb_t))+1;
    _mpz_realloc(z, len);

    for( ;; c = *t++) {
        if (isdigit(c)) {
            c -= '0';
        }else if (isalpha(c)) {
            c -= isupper(c)? ('A'-10): ('a'-10);
        }else {
            break;
        }
        if (c >= base) {
            break;
        }
        size = abs(z->_mp_size);
        res  = c;
        /* z := z*base + c */
        for(i=0; i<size; i++) {
            uu  = ((ulong64)z->_mp_d[i])*base+res;
            res = LL_HIGH(uu);
            z->_mp_d[i] = LL_LOW(uu);
        }
    }
    if (neg) {
        __mpz_neg(z);
    }
    return 0;
}

#ifdef DEBUG
void dump(mpz_ptr z)
{
    int i;
    int size = abs(z->_mp_size);
    printf("_mp_size = %d\n_mp_d=%x", z->_mp_size, z->_mp_d[0]);
    for(i=1; i<size; i++) {
        printf(":%x", z->_mp_d[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    mpz_t z;
    mpz_init(z);
    mpz_set_str(z, argv[1], 0);
    printf("%s\n", mpz_get_str(NULL, 10, z));
    return 0;
}
#endif
