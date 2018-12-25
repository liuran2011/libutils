#ifndef NFC_MD5_H
#define NFC_MD5_H

#include <stdint.h>

#define MD5LENGTH 16

struct MD5Context {
	uint32_t buf[4];
	uint32_t bits[2];
	unsigned char in[64];
};

void MD5Init(struct MD5Context *context);
void MD5Update(struct MD5Context *context, unsigned char const *buf,
	       unsigned len);
void MD5Final(unsigned char digest[MD5LENGTH], struct MD5Context *context);

/*
 * This is needed to make RSAREF happy on some MS-DOS compilers.
 */
typedef struct MD5Context MD5_CTX;

#endif /* !MD5_H */
