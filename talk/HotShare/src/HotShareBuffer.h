#ifndef _HOTSHAREBUFFER_H
#define _HOTSHAREBUFFER_H

#include <string>
#include <Ice/Ice.h>

namespace xce {
namespace hotshare {

class HotShareBuffer
{
  public:
    std::string *hotShareDataBuffer;
    int bottomBufferPos, topBufferPos;

    Ice::Long *hotShareIDBuffer;
    int bottomDBIDPos, topDBIDPos;

    Ice::Long baseValue;
    Ice::Long maxDBID;
  
  public:
    HotShareBuffer();
    ~HotShareBuffer();

    Ice::Long GetMaxDBID();
    void SetMaxDBID(Ice::Long);
    Ice::Long GetMaxLocalID();
    Ice::Long GetMaxDataBufferID();
    int GetLocalID(Ice::Long DBID);
    int GetLocalID(Ice::Long DBID, bool &insertID);

    bool IsSameSize();
    
    int AddID(Ice::Long localID, Ice::Long DBID);
    bool DeleteID(Ice::Long localID);
    bool AddXML(std::string xml, Ice::Long DBID);
    bool IsInsertID(Ice::Long dbID);

    bool GetHotShare(std::vector<std::string> &hsVec, Ice::Long lowID, Ice::Long highID,size_t size, Ice::Long &retLowID, Ice::Long &retHignID);
    bool GetHotShare(std::vector<std::string> &hsVec, Ice::Long localId, size_t size, Ice::Long &retLowID, Ice::Long &retHighID);

  private:
    int AddID(Ice::Long DBID);
    bool DoAddXML(std::string xml, int pos);
    int GetDBIDPos(Ice::Long DBID);

};

}
}

#endif
