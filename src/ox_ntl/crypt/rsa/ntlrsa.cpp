/* $OpenXM: OpenXM/src/ox_ntl/crypt/rsa/ntlrsa.cpp,v 1.1 2004/05/16 15:04:54 iwane Exp $ */
/*-- References: RFC 3447 PKCS #1: RSA Encryption Version 2.1 --*/

#include <stdio.h>
#include <string.h>

#include <NTL/ZZ.h>

#include "ntlrsa.h"

//////////////////////////////////////////////////////////////////////////////
// static variable
//////////////////////////////////////////////////////////////////////////////
const ZZ NtlRsa::dummy = to_ZZ(-1);

//////////////////////////////////////////////////////////////////////////////
// Operator
//////////////////////////////////////////////////////////////////////////////
ostream &
operator << (ostream &o, const NtlRsa &rsa)
{
	return (o << "[public[" << rsa.getPublicKey() << ",mod=" << rsa.getMod() << ",k=" << rsa.getModLen() << "],private[" << rsa.getPrivateKey() << "]");
}



//////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////////////
NtlRsa::NtlRsa(void) // sample for test
{
	setKeys(to_ZZ(1231 * 4567), to_ZZ(65537), to_ZZ(3988493));
}

NtlRsa::NtlRsa(int seed, int bit)
{
	keygen(seed, bit);
}

NtlRsa::NtlRsa(ZZ &mod, ZZ &publicKey, ZZ &privateKey) {
	setKeys(mod, publicKey, privateKey);
}

NtlRsa::NtlRsa(ZZ &mod, ZZ &publicKey) {
	setKeys(mod, publicKey);
}


void
NtlRsa::keygen(const char *seed_str, int bit)
{
	int seed = 0;
	const char *p = seed_str;

	for (; *p != '\0'; p++) {
		seed = seed << 5 + *p;
	}

	keygen(seed, bit);
}

void
NtlRsa::keygen(int seed, int bit)
{
	// at least 512 bit
	keygen(seed, bit, mod, publicKey, privateKey);
	k = getBlockSize(mod);
}

void
NtlRsa::keygen(int seed, int bit, ZZ &n, ZZ &e, ZZ &d)
{
	ZZ gcd, phi;
	ZZ p, q;

	e = to_ZZ(65537); // or 3, 11 ...
	
	SetSeed(to_ZZ(seed));

	do {
		/* select two prime number */
		p = RandomPrime_ZZ(bit / 2);
		q = RandomPrime_ZZ(bit / 2);

		mul(phi, p - 1, q - 1);

		GCD(gcd, e, phi);
	} while (gcd != 1);

	InvMod(d, e, phi);
	mul(n, p, q);
}


/****************************************************************************
 *
 * PARAM : n : positive number.
 * RETURN: octet size of n
 *
 * NOTE: NumBits()
 *   n : -3 -2 -1  0  1  2  3  4  5  6  7  8  9 ... 1022 1023 1024 1025 
 *  bit:  2  2  1  0  1  2  2  3  3  3  3  4  4 ...   10   10   11   11
 ****************************************************************************/
inline int
NtlRsa::getBlockSize(const ZZ& n)
{
	return ((int)((NumBits(n) + 7) / 8));
}


/****************************************************************************
 *
 * PARAM : O : vec
 *       : I : null-terminate string.
 *       : I : k : length of modulus in octets > NtlRsa::PS_MIN + 3
 *       :   :   : k is computed by RSAPublicKey-modulus
 *       : I : f : if (f) public_key else private_key
 * RETURN: size of encrypted data
 ****************************************************************************/
int
NtlRsa::encrypt(unsigned char *eb, const unsigned char *msg, int size, int k, const ZZ &c, const ZZ &mod, int padding) const
{
	int i;
	int s;      /* length of data */
	int ps;     /* length of padding string */
	const int DATA_MAX = k - NtlRsa::PS_MIN - 3;
	ZZ x, y, ebz;
	unsigned char uch;

	// k == ps + s + 3;
	if (size > DATA_MAX) {
		s = DATA_MAX;
		ps = NtlRsa::PS_MIN;
	} else {
		s = size;
		ps = DATA_MAX - size + NtlRsa::PS_MIN;
	}

	//--------------------------------------------------
	// Encryption-block formatting
	//--------------------------------------------------
	eb[k - 1] = 0x00;
	eb[k - 2] = padding;  

	switch (padding) {
	case NtlRsa::PADDING_PUBLIC:
		for (i = 0; i < ps; i++) {
			// 0 < random number < 255
			eb[k - i - 3] = (unsigned char)(random() % (NtlRsa::OCTET_SIZE - 1) + 1);
		}
		break;
	case NtlRsa::PADDING_PRIVATE:
		for (i = 0; i < ps; i++) {
			eb[k - i - 3] = 0xff;
		}
		break;
	case NtlRsa::PADDING_PRIVATE0: // 
		for (i = 0; i < ps; i++) {
			eb[k - i - 3] = 0x00;
		}
		break;
	default:
		return (-1);
	}

	eb[k - ps - 3] = 0x00;
	for (i = 0; i < s; i++) {
		eb[k - ps - 4 - i] = *msg++;
	}

	//--------------------------------------------------
	// Octet-string-to-integer conversion
	//--------------------------------------------------
	ZZFromBytes(x, eb, k);

	//--------------------------------------------------
	// RSA computation
	//--------------------------------------------------
	PowerMod(y, x, c, mod);

	//--------------------------------------------------
	// Integer-to-octet-string conversion
	//--------------------------------------------------
	memset(eb, 0, sizeof(k));
	BytesFromZZ(eb, y, k);

	// reverse
	for (i = 0; i < k / 2; i++) {
		uch = eb[i];
		eb[i] = eb[k - i - 1];
		eb[k - i - 1] = uch;
	}

	return (s);
}


int
NtlRsa::encryptByPublicKey(unsigned char *eb, const char *msg) const
{
	return (encryptByPublicKey(eb, (const unsigned char *)msg, strlen(msg) + 1));
}


int
NtlRsa::encryptByPublicKey(unsigned char *eb, const unsigned char *msg, int len) const
{
	return (encrypt(eb, msg, len, k, publicKey, mod, PADDING_PUBLIC));
}

int
NtlRsa::encryptByPrivateKey(unsigned char *eb, const char *msg) const
{
	return (encryptByPrivateKey(eb, (unsigned char *)msg, strlen(msg) + 1));
}

int
NtlRsa::encryptByPrivateKey(unsigned char *eb, const unsigned char *msg, int len) const
{
	return (encrypt(eb, msg, len, k, privateKey, mod, PADDING_PRIVATE));
}




int
NtlRsa::decryptByPrivateKey(unsigned char *buf, const unsigned char *eb) const
{
	return (decrypt(buf, eb, k, privateKey, mod, PADDING_PRIVATE));
}

int
NtlRsa::decryptByPublicKey(unsigned char *buf, const unsigned char *eb) const
{
	return (decrypt(buf, eb, k, publicKey, mod, PADDING_PUBLIC));
}


/***
 *
 * PARAM  : O : d : data :: buffer size >= k.
 * RETURN : data size
 */
int
NtlRsa::decrypt(unsigned char *d, const unsigned char *eb, int k, const ZZ &exp, const ZZ &mod, int padding) const
{
	ZZ x, y;
	int i, n;
	int bt;
	unsigned char uch;

	//--------------------------------------------------
	// Octet-string-to-integer
	//--------------------------------------------------

	// reverse
	for (i = 0; i < k; i++) {
		d[k - i - 1] = eb[i];
	}

	ZZFromBytes(y, d, k);

	// error check.
	if (y >= mod || sign(y) < 0) {
		cerr << "error1: " << sign(y) << endl;
		return (-1);
	}

	//--------------------------------------------------
	// RSA computation
	//--------------------------------------------------
	PowerMod(x, y, exp, mod);

	//--------------------------------------------------
	// Integer-to-octet-string
	//--------------------------------------------------
	BytesFromZZ(d, x, k);

	//--------------------------------------------------
	// Encryption-block parsing
	// EB = D || 00 || PS || BT || 00 <== attention
	//--------------------------------------------------
	if (d[k - 1] != 0x00) {
		return (-2);
	}


	if (padding == PADDING_PUBLIC) { // public key
		bt = d[k - 2];
		if (bt != 0x00 && bt != 0x01)
			return (-3);

		if (bt == 0x00) {
			for (i = 0; i < k - 3; i++) {
				if (d[k - 3 - i] != 0x00) {
					i--;
					break;
				}
			}
		} else {
			for (i = 0; i < k - 3; i++) {
				if (d[k - 3 - i] != 0xff)
					break;
			}
		}

	} else { // private key
		if (d[k - 2] != 0x02)
			return (-4);

		for (i = 0; i < k - 3; i++) {
			if (d[k - 3 - i] == 0x00)
				break;
		}
	}

	if (d[k - 3 - i] != 0x00) {
		return (-5);
	}
	if (i < NtlRsa::PS_MIN) // too short
		return (-6);

	if (i == k - 3)             // parse error.
		return (-7);


	// reverse 
	n = k - 3 - i;
	for (i = 0; i < n / 2; i++) {
		uch = d[i];
		d[i] = d[n - i - 1];
		d[n - i - 1] = uch;
	}

	return (n);	// data size.
}


