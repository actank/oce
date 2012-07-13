#ifndef __TRIPOD_SEARCH_UPDATE_ADAPTER_H__
#define __TRIPOD_SEARCH_UPDATE_ADAPTER_H__
#include <IceUtil/IceUtil.h>
#include <sys/time.h>
#include <set>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <QueryRunner.h>
#include "tripod-new/src/cpp/include/TripodClient.h"

namespace search {
  namespace tripodupdate {
    const unsigned int MaxUserId = 200;

    class TripodSearchUpdateAdapter : public MyUtil::Singleton<TripodSearchUpdateAdapter>, IceUtil::Thread {
      public :
        TripodSearchUpdateAdapter();
        void invoke(long userId);
        void run();
      private :
        std::vector<long> _buffer;
        IceUtil::Monitor<IceUtil::Mutex> _mutex;
        com::renren::tripod::TripodClient* _tClient;
    };
  }
}

#endif
