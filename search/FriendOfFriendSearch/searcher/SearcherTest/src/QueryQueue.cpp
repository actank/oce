#include "QueryQueue.h"



QueryQueue::QueryQueue()  {
}

void QueryQueue::Get(QueryUnit& data) {
  try  {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
    if(data_.size() < 1)  {
      mutex_.timedWait(IceUtil::Time::seconds(3600));
    }
    data = data_.front();
    data_.pop();
  }  catch (...) {
    MCE_WARN("QueryQueue Getdata loop exception");
  }
}

void QueryQueue::Put(QueryUnit& data)  {
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
  data_.push(data);
  if (data_.size() >= 1) {
    mutex_.notify();
  }
}


