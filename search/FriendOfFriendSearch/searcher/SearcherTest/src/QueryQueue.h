#ifndef __QUERYQUEUE_H__
#define __QUERYQUEUE_H__

#include "UtilCxx/src/Singleton.h"
#include <boost/shared_ptr.hpp>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <queue>

#define MAX 2000
using namespace std;


struct QueryUnit  {
  string id;
  string query;
};

typedef queue<QueryUnit> QueryUnitSeq;

class QueryQueue: public MyUtil::Singleton<QueryQueue> {
  public:
    QueryQueue();
    void Get(QueryUnit& data);
    void Put(QueryUnit& data);
  private:
    QueryUnitSeq data_;
    IceUtil::Monitor<IceUtil::Mutex> mutex_;
};
#endif
