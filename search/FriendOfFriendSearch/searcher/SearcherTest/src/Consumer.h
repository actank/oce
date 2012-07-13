#ifndef __CONSUMER_H__
#define __CONSUMER_H__

#include "UtilCxx/src/Singleton.h"
#include <boost/shared_ptr.hpp>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include "QueryQueue.h"


class Consumer: virtual public IceUtil::Thread, public MyUtil::Singleton<Consumer> {
  public:
    Consumer(QueryQueue& message_queue, int begin, int limit);
    virtual void run();
  private:
    QueryQueue &  message_queue_;
    int begin_;
    int limit_;
};
#endif
