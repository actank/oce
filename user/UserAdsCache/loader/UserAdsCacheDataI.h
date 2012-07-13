#ifndef USERADSCACHEDATAI_H_
#define USERADSCACHEDATAI_H_

#include <UserAdsCache.h>
#include <QueryRunner.h>

namespace xce {
namespace useradscache {
class UserAdsCacheGeneralDataI: virtual public UserAdsCacheUrlData, public IceUtil::Mutex {
public:
	UserAdsCacheGeneralDataI();
	void setUserBornProperties(::mysqlpp::Row&, const char*, const char*, const char*, const char*, const char*, const char*);
	void setUserStageProperties(::mysqlpp::Row&, const char*);
	void setUserUrlProperties(::mysqlpp::Row&, const char*);
	void setWorkplaceProperties(::mysqlpp::Row&, const char*, const char* , const char*);
	void setRegionProperties(::mysqlpp::Row&, const char*, const char*);
	void setNetworkProperties(::mysqlpp::Row&, const char*, const char*, const char*);
	void setElementaryProperties(::mysqlpp::Row&, const char*, const char*, const char*);
	void setJuniorProperties(::mysqlpp::Row&, const char*, const char*, const char*);
	void setSeniorProperties(::mysqlpp::Row&, const char*, const char*, const char*);
	void setCollegeProperties(::mysqlpp::Row&, const char*, const char*, const char*, const char*);
	void setUniversityProperties(::mysqlpp::Row&, const char*, const char*, const char*, const char*, const char*);
	UserAdsCacheDataPtr toOldData();
};

typedef ::IceUtil::Handle<UserAdsCacheGeneralDataI> UserAdsCacheGeneralDataIPtr;
typedef ::std::vector<UserAdsCacheGeneralDataIPtr> UserAdsCacheGeneralDataISeq;

}
}
#endif
