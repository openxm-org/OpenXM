/* $OpenXM: OpenXM/src/ox_ntl/crypt/sha1/sha1.h,v 1.2 2004/03/25 13:34:19 iwane Exp $ */

#ifndef __SHA1_H__
#define __SHA1_H__

#ifdef __cplusplus
extern "C" {
#endif

int	sha1(unsigned char *, const unsigned char *, int len);
int	sha1_h(unsigned char *, const unsigned char *, int len, const unsigned int *_h);

void	sha1_md	(unsigned int *, const unsigned char *);


#ifdef _WITH_NTL_

int	ntl_sha1(ZZ &, const ZZ &);
int	ntl_sha1_h(ZZ &, const ZZ &, unsigned int *);

#endif /* _WITH_NTL */

#ifdef __cplusplus
}
#endif

#endif /* __SHA1_H__ */

