#ifndef _DATAPROVIDERDB_H_
#define _DATAPROVIDERDB_H_

#include <vector>
#include <Ice/Ice.h>
#include "DataProviderInfo.h"

namespace xce {
namespace dataprovider {

const size_t kHotShareReadSize = 1000;

class DataProviderDB {
  public:
    static bool GetHotShare(Ice::Long &max_db_id,  std::vector<HotShareInfo> &hotshare_vector, size_t size=kHotShareReadSize);
};

}
}
#endif
