#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#else
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#endif


#include "p2jd-table.h"


template <typename codepoint_t>
inline int p2jd_calcsize(const codepoint_t *src, int srclen) {
	const unsigned short *jamo_seq;
	int ret = 0;
	const codepoint_t *src_end = src + srclen;
	for (; src < src_end; ++src) {
		jamo_seq = lookup(*src);
		if (jamo_seq != NULL) {
			ret += jamo_seq[0];
		} else {
			ret += 1;
		}
	}
	return ret;
}


template <typename codepoint_t>
inline int p2jd_encode(const codepoint_t *src, int srclen, codepoint_t *dst) {
	const unsigned short *jamo_seq;
	int jamo_len;
	const codepoint_t *src_end = src + srclen;
	const codepoint_t *dst_start = dst;
	for (; src < src_end; ++src) {
		jamo_seq = lookup(*src);
		if (jamo_seq == NULL) {
			*(dst++) = *src;
		} else {
			jamo_len = *(jamo_seq++);
			while (jamo_len-- > 0) {
				*(dst++) = *(jamo_seq++);
			}
		}
	}
	return dst - dst_start;
}


extern "C" int hypua_p2jd_ucs2_calcsize(const uint16_t *src, int srclen) {
	return p2jd_calcsize(src, srclen);
}


extern "C" int hypua_p2jd_ucs2_encode(const uint16_t *src, int srclen, uint16_t *dst) {
	return p2jd_encode(src, srclen, dst);
}


extern "C" int hypua_p2jd_ucs4_calcsize(const uint32_t *src, int srclen) {
	return p2jd_calcsize(src, srclen);
}


extern "C" int hypua_p2jd_ucs4_encode(const uint32_t *src, int srclen, uint32_t *dst) {
	return p2jd_encode(src, srclen, dst);
}


extern "C" void hypua_p2jd_ucs2_debug() {
	uint16_t src = 0xF7CA;

	printf("p2jd_F7CA: 0x%p len=%d\n", p2jd_F7CA, p2jd_F7CA[0]);
	const uint16_t *p2jd = lookup(src);
	printf("p2jdfound: 0x%p len=%d\n", p2jd, p2jd[0]);

	int buf_size = p2jd_calcsize(&src, 1);
	printf("p2jd_calcsize: size=%d\n", buf_size);
	uint16_t *buf = (uint16_t *) malloc(buf_size * sizeof(uint16_t));
	int n_encoded = p2jd_encode(&src, 1, (uint16_t*)buf);
	free(buf);
	printf("p2jd_encode: encoded=%d\n", n_encoded);
}
