/* $OpenXM: OpenXM/src/ox_ntl/crypt/sha1/sha1.h,v 1.3 2004/05/16 15:02:39 iwane Exp $ */

#ifndef __SHA1_H__
#define __SHA1_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int	sha1(unsigned char *, const unsigned char *, size_t _len);
int	sha1_h(unsigned char *, const unsigned char *, size_t _len, const uint32_t *_h);

int	fsha1(unsigned char *, int _fd);

void	sha1_md	(uint32_t *, const unsigned char *);


#ifdef _WITH_NTL_

int	ntl_sha1(ZZ &, const ZZ &);
int	ntl_sha1_h(ZZ &, const ZZ &, uint32_t *);

#endif /* _WITH_NTL */

#ifdef __cplusplus
}
#endif

#endif /* __SHA1_H__ */

