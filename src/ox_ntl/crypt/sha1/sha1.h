/* $OpenXM$ */

#ifndef __SHA1_H__
#define __SHA1_H__

#ifdef __cplusplus
extern "C" {
#endif

int	sha1(unsigned char *, const unsigned char *, int len);
int	sha1_h(unsigned char *, const unsigned char *, int len, unsigned int *_h);


#ifdef _WITH_NTL_

int	ntl_sha1(ZZ &, const ZZ &);
int	ntl_sha1_h(ZZ &, const ZZ &, unsigned int *);

#endif /* _WITH_NTL */

#ifdef __cplusplus
}
#endif

#endif /* __SHA1_H__ */

