#include "url_encrypt.h"

#include "AdUnion/gate/url_base64.h"
#include "AdUnion/gate/aes_wrapper.h"

namespace xce {
namespace ad {

static const char * kAesKey = "!7a@k!Z#@$rr98";
static int kAesKeyLen = strlen(kAesKey);

std::string UrlEncrypt(const char * plain, unsigned int len) {
  AesWrapper aw((const unsigned char *)kAesKey, kAesKeyLen);
  unsigned char * ciphertext = aw.Encrypt((unsigned char *)plain, (int*)&len);

  std::string cipher_url = url_base64_encode((ciphertext), len);
  free(ciphertext);

  return cipher_url;
}

std::string UrlDecrypt(const std::string & cipher_url) {
  std::string cipher_text = url_base64_decode(cipher_url);
  AesWrapper aw((const unsigned char *)kAesKey, kAesKeyLen);
  int sz = cipher_text.size();
  unsigned char * plain = aw.Decrypt((unsigned char *)cipher_text.c_str(), &sz);
  if (!plain) {
    return "";
  }
  std::string ret(reinterpret_cast<const char *>(plain), sz);
  free(plain);
  return ret;
}

}
}

