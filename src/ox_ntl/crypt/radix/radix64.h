/* $OpenXM$ */

#ifndef __RADIX64_H__
#define __RADIX64_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef long crc24;
crc24	crc_octets(const unsigned char *, size_t);

int	radix64_enc	(const unsigned char *, int, unsigned char *, int);
int	radix64_dec	(const unsigned char *, int, unsigned char *, int);





#ifdef __cplusplus
}
#endif

#endif /* __RADIX64_H__ */

