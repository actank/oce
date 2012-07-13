#ifndef _MEMC_PROXY_UTIL_
#define _MEMC_PROXY_UTIL_

namespace xce {
namespace feed {

// 读出一个command line, 返回值指向'\r\n' 的'\n'字符
char * GetLineEnd(char * buf, size_t len);

}
}

#endif // _MEMC_PROXY_UTIL_

