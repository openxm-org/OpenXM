/* $OpenXM$ */

#ifndef __NTL_RSA_H__
#define __NTL_RSA_H__

#include <NTL/ZZ.h>

/***************************************************
 * NtlRsa.
 ***************************************************/
class NtlRsa {
// Member
private:
	ZZ mod;         // n = p*q : modulus
	ZZ privateKey;  // d : private exponent
	ZZ publicKey;   // e : public exponent
	int k;		// length of modulus in octets 

public:
	static const int OCTET_SIZE = 256;
	static const int PS_MIN     = 8;
	static const ZZ  dummy;

	enum {
		PADDING_PRIVATE0 = 0x00,
		PADDING_PRIVATE  = 0x01,
		PADDING_PUBLIC   = 0x02,
	};
		


// constructor 
public:
	NtlRsa(void);
	NtlRsa(int seed, int bit);
	NtlRsa(ZZ &mod, ZZ &publicKey, ZZ &privateKey);
	NtlRsa(ZZ &mod, ZZ &publicKey);
	virtual ~NtlRsa() { }

// member access
public:
	inline ZZ getPublicKey() const {
		return (publicKey);
	};
	inline ZZ getPrivateKey() const {
		return (privateKey);
	};
	inline ZZ getMod() const {
		return (mod);
	};
	inline int getModLen() const {
		return (k);
	};
	inline void setKeys(const ZZ &n, const ZZ &e, const ZZ &d) {
		mod = n;
		publicKey = e;
		privateKey = d;
		k = getBlockSize(mod);
	};

	inline void setKeys(const ZZ &n, const ZZ &e) { // dont know private key
		setKeys(n, e, dummy);
	};


// method
private:
	int getBlockSize(const ZZ &);

	void keygen(int seed, int bit, ZZ &, ZZ &, ZZ &);

	int	encrypt(unsigned char *, const unsigned char *, int size, int, const ZZ &, const ZZ &, int) const;
	int	decrypt(unsigned char *, const unsigned char *, int, const ZZ &, const ZZ &, int) const;

public:
	void keygen(int seed, int bit);
	void keygen(const char *seed_str, int bit);

	int	encryptByPublicKey(unsigned char *, const char *) const;
	int	encryptByPublicKey(unsigned char *, const unsigned char *, int len) const;
	int	encryptByPrivateKey(unsigned char *, const char *) const;
	int	encryptByPrivateKey(unsigned char *, const unsigned char *, int len) const;

	int	decryptByPublicKey(unsigned char *, const unsigned char *) const;
	int	decryptByPrivateKey(unsigned char *, const unsigned char *) const;


/// friend operator.

	friend ostream & operator << (ostream &o, const NtlRsa &rsa);

};

#endif /* __NTL_RSA_H__ */
