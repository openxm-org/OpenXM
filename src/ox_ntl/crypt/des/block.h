/* $OpenXM$ */

#ifndef __BLOCK_H__
#define __BLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

int	crypt_ecb(void *key, int block, int len, const unsigned char *data, unsigned char *buf, void *);

int	crypt_enc_cbc(void *key, int, const unsigned char *, int, const unsigned char *, unsigned char *, void *);
int	crypt_dec_cbc(void *key, int, const unsigned char *, int, const unsigned char *, unsigned char *, void *);

int	crypt_cfb(void *_key, int _mode, int _bit, int _block,
		const unsigned char *_iv, int _len,
		const unsigned char *_data,
		unsigned char *_buf,void *);

int	crypt_ofb(void *_key, int _bit, int _block,
		const unsigned char *_iv, int _len,
		const unsigned char *_data,
		unsigned char *_buf, void *);


#ifdef __cplusplus
}
#endif


#endif /* __BLOCK_H__ */
