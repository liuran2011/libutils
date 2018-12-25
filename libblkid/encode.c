
/*
 * encode.c - string convertion routines (mostly for compatibility with
 *            udev/volume_id)
 *
 * Copyright (C) 2008 Kay Sievers <kay.sievers@vrfy.org>
 * Copyright (C) 2009 Karel Zak <kzak@redhat.com>
 *
 * This file may be redistributed under the terms of the
 * GNU Lesser General Public License.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "blkidP.h"

#define UDEV_ALLOWED_CHARS_INPUT               "/ $%?,"

size_t blkid_encode_to_utf8(int enc, unsigned char *dest, size_t len,
			const unsigned char *src, size_t count)
{
	size_t i, j;
	uint16_t c;

	for (j = i = 0; i + 2 <= count; i += 2) {
		if (enc == BLKID_ENC_UTF16LE)
			c = (src[i+1] << 8) | src[i];
		else /* BLKID_ENC_UTF16BE */
			c = (src[i] << 8) | src[i+1];
		if (c == 0) {
			dest[j] = '\0';
			break;
		} else if (c < 0x80) {
			if (j+1 >= len)
				break;
			dest[j++] = (uint8_t) c;
		} else if (c < 0x800) {
			if (j+2 >= len)
				break;
			dest[j++] = (uint8_t) (0xc0 | (c >> 6));
			dest[j++] = (uint8_t) (0x80 | (c & 0x3f));
		} else {
			if (j+3 >= len)
				break;
			dest[j++] = (uint8_t) (0xe0 | (c >> 12));
			dest[j++] = (uint8_t) (0x80 | ((c >> 6) & 0x3f));
			dest[j++] = (uint8_t) (0x80 | (c & 0x3f));
		}
	}
	dest[j] = '\0';
	return j;
}
