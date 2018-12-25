#ifndef BASE64_H
#define BASE64_H

#include <stddef.h>
#include <stdbool.h>

#define BASE64_LENGTH(inlen) ((((inlen) + 2) / 3) * 4)

void base64_encode (const char * in, size_t inlen,
                           char * out, size_t outlen);
void base64_decode(const char * in, size_t inlen,
            char * out, size_t *outlen);

#endif
