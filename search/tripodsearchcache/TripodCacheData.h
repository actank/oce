#ifndef __TRIPOD_CACHE_DATA_H__
#define __TRIPOD_CACHE_DATA_H__

#include <IceUtil/IceUtil.h>
#include "TripodSearchCache.pb.h"
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
namespace xce{
  namespace search{
    namespace tripodcachedata{
      class TripodCacheData : public oce::tripodsearchcache::TripodSearchCacheData, public Ice::Object {
        public:
          TripodCacheData() { }
          std::string splitProfilePrivacy(std::string profilePrivacy);
          void setElementarySchoolPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
          void setJuniorSchoolPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
          void setHighSchoolPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
          void setCollegePropertiesAccordingVector(std::vector<mysqlpp::Row>&);
          void setUniversityPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
          void setWorkplacePropertiesAccordingVector(std::vector<mysqlpp::Row>&);
          void setRegionPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
          void setNetworkPropertiesAccordingVector(std::vector<mysqlpp::Row>&);

          void setUserPassportProperties(mysqlpp::Row& row);
          void setConfigProperties(mysqlpp::Row& row);
          void setUserUrlProperties(mysqlpp::Row& row);
          void setUserStateProperties(mysqlpp::Row& row);
          void setUserStageProperties(mysqlpp::Row& row);
          void setUserNameProperties(mysqlpp::Row& row);
          void setUserBornProperties(mysqlpp::Row& row);

          void setViewCountProperties(mysqlpp::Row& row);
          void setLastlogintimeProperties(mysqlpp::Row& row);
          void setUserFondProperties(mysqlpp::Row& row);

          IceUtil::RWRecMutex _dataMutex;

      };

      typedef IceUtil::Handle<TripodCacheData> TripodCacheDataPtr;
    }
  }
}
#endif
