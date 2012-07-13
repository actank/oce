#ifndef XCE_BASE_MD5_H__
#define XCE_BASE_MD5_H__

#include "feed/base_feed/basictypes.h"

typedef xce::uint32 u32;
typedef xce::uint8 u8;

struct MD5Context {
  u32 buf[4];
  u32 bits[2];
  u8 in[64];
};

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void MD5Init(struct MD5Context *ctx);

void MD5Update(struct MD5Context *ctx, unsigned char const *buf, unsigned len);

void MD5Final(unsigned char digest[16], struct MD5Context *ctx);

#ifdef __cplusplus
}
#endif

#endif  // XCE_BASE_MD5_H__
