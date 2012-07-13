#ifndef __SEARCH_STAT_H__
#define __SEARCH_STAT_H__
#include "TaskManager.h"
#include "Singleton.h"
namespace demo{
using namespace MyUtil;
using namespace xce::util;
class SearchStat: public MyUtil::Timer
{
public:
    SearchStat(Ice::Int s = 5): Timer(1000*s){} 
    virtual void handle();
    void setEntryNum();
    Ice::Int  readEntryNum();
private:
    Ice::Int _entryNum;
    IceUtil::RWRecMutex _mutex;
};
typedef IceUtil::Handle<SearchStat> SearchStatPtr;

}
#endif
