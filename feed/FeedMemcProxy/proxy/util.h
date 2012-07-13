#ifndef _MEMC_PROXY_UTIL_
#define _MEMC_PROXY_UTIL_

namespace xce {
namespace feed {

int GetAndRemoveCmdEnd(char * buf, size_t len);


char * GetLineEnd(char * buf, size_t len);

}
}

#endif // _MEMC_PROXY_UTIL_

