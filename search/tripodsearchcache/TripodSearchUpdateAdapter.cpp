#include "TripodSearchUpdateAdapter.h"
#include <boost/lexical_cast.hpp>
#include "util/cpp/TimeCost.h"

using namespace search::tripodupdate;
using namespace com::renren::tripod;

TripodSearchUpdateAdapter::TripodSearchUpdateAdapter() {
  _tClient = new TripodClient("UserZooKeeper1:2181,UserZooKeeper2:2181,UserZooKeeper3:2181,UserZooKeeper4:2181,UserZooKeeper5:2181/Tripod", "ne0", "SearchCache");
  start();
}

void TripodSearchUpdateAdapter::invoke(long userId) {
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
  if (_buffer.size() <= 2000) {
    _buffer.push_back(userId);
  }
  if (_buffer.size() >= MaxUserId) {
    _mutex.notify();
  }
}

void TripodSearchUpdateAdapter::run() {
  while(true) {
    std::vector<long> userIds;
    {
      IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
      if (_buffer.size() < MaxUserId) {
        MCE_INFO("TripodSearchUpdateAdapter::run wait 5s");
        _mutex.timedWait(IceUtil::Time::seconds(5));
      }
      MCE_INFO("TripodSearchUpdateAdapter::run _buffer.size:" << _buffer.size());
      userIds.swap(_buffer);
    }

    if (!userIds.empty()) {
      MCE_INFO("TripodSearchUpdateAdapter::run userIds.size:" << userIds.size());
      MyUtil::TimeCost tc = MyUtil::TimeCost::create("TripodSearchUpdateAdapter::run update tripodcache",1);
      sort(userIds.begin(), userIds.end());
      std::vector<long>::iterator end = unique(userIds.begin(), userIds.end());   //去重，只有通过排序后才能进行此操作
      userIds.resize(end - userIds.begin());
      tc.step("unique");
      std::vector<std::string> userIdsStr;
      for (std::vector<long>::iterator iter = userIds.begin(); iter != userIds.end(); iter++) {
        userIdsStr.push_back(boost::lexical_cast<std::string>(*iter));
      }
      tc.step("prepare");
      bool loadResult = _tClient->load(userIdsStr, INT_MAX, false);
      MCE_DEBUG("loadResult:" << loadResult);
      tc.step("execute");
    }
  }
}

