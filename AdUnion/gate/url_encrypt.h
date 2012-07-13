
#ifndef _AD_UNION_URL_ENCRYPT_H_
#define _AD_UNION_URL_ENCRYPT_H_

#include <string>

namespace xce {
namespace ad {

std::string UrlEncrypt(const char * plain, unsigned int len);
std::string UrlDecrypt(const std::string & s);

}
}
#endif // _AD_UNION_URL_ENCRYPT_H_

