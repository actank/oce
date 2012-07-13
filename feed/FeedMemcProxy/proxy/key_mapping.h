#ifndef _MEMC_PROXY_KEY_MAPPING_
#define _MEMC_PROXY_KEY_MAPPING_

#include <string>
#include <boost/asio.hpp>

namespace xce {
namespace feed {

bool InitCacheClusters();
boost::asio::ip::tcp::endpoint GetEndpointByKey(const char * key, size_t len);

}
}

#endif //_MEMC_PROXY_KEY_MAPPING_
