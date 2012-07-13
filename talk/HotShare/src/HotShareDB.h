#ifndef _HOTSHAREDB_H_
#define _HOTSHAREDB_H_

#include <vector>
#include <Ice/Ice.h>
#include "IDInfo.h"
#include "HotShareInfo.h"

namespace xce {
namespace hotshare {

class HotShareDB
{
  public:
    bool InsertID(Ice::Long localID, Ice::Long srcID);
    bool DeleteID( Ice::Long localID);
    bool GetID(std::vector<IDInfo> &IDInfoVec, Ice::Long maxLocalID, size_t size);
    bool GetHotShare(std::vector<HotShareInfo> &hsVec, Ice::Long maxDBID, size_t size, Ice::Long &retMaxDBID);
};

}
}
#endif
