
#ifndef _AD_UNION_URL_BASE64_H_
#define _AD_UNION_URL_BASE64_H_

#include <string>

namespace xce {
namespace ad {

std::string url_base64_encode(unsigned char const* , unsigned int len);
std::string url_base64_decode(const std::string & s);

}
}
#endif // _AD_UNION_URL_BASE64_H_

