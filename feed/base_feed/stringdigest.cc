#include "feed/base_feed/common.h"
#include "feed/base_feed/md5.h"
#include "feed/base_feed/stringdigest.h"
#include "feed/base_feed/stringencode.h"

namespace xce {

std::string MD5(const std::string& data) {
  MD5Context ctx;
  MD5Init(&ctx);
  MD5Update(&ctx, const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(data.data())), static_cast<unsigned int>(data.size()));
  unsigned char digest[16];
  MD5Final(digest, &ctx);
  std::string hex_digest;
  for (int i=0; i<16; ++i) {
    hex_digest += hex_encode(digest[i] >> 4);
    hex_digest += hex_encode(digest[i] & 0xf);
  }
  return hex_digest;
}

void MD5(unsigned char* digest, const char* data, size_t length) {
  MD5Context ctx;
  MD5Init(&ctx);

  MD5Update(&ctx, const_cast<unsigned char *>(
    reinterpret_cast<const unsigned char *>(data)
    ), length);
  MD5Final(digest, &ctx);
}

}
