#ifndef __DIST_USERAPICACHE_ADAPTER_H__
#define __DIST_USERAPICACHE_ADAPTER_H__

#include "util/cpp/TimeCost.h"
#include "ServiceI.h"
#include "XceDistCacheAdapter.h"
#include "UserApiCache.pb.h"
#include <sys/time.h>
#include <set>
#include <map>
#include <vector> 
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include "util/cpp/TimeCost.h"
#include <QueryRunner.h>

namespace xce
{
namespace distcache
{
namespace userapicache
{

class UserApiCacheMemData : public xce::distcache::userapicache::UserApiCacheData, public Ice::Object {
public:
	UserApiCacheMemData() 
	{
	}
				
	void setUserPassportProperties(mysqlpp::Row& row, const char* fieldStatus) {
		set_status((int)row[fieldStatus]);
	}
	
	void setNameProperties(mysqlpp::Row& row, const char* fieldName) {
#ifndef NEWARCH
		set_name(row[fieldName].get_string());
#else
		set_name(row[fieldName].c_str());
#endif
	}

	void setRightProperties(mysqlpp::Row& row, const char* fieldAuth) {
		set_auth((int)row[fieldAuth]);
	}

	void setStateProperties(mysqlpp::Row& row, const char* fieldState, const char* fieldLevel) {
		set_state((int)row[fieldState]);
		set_level((int) row[fieldLevel]);
	}

	void setUrlProperties(mysqlpp::Row& row, const char* fieldMainUrl, const char* fieldHeadUrl, const char* fieldTinyUrl) {
#ifndef NEWARCH
		set_mainurl(row[fieldMainUrl].get_string());
#else
		set_mainurl(row[fieldMainUrl].c_str());
#endif
#ifndef NEWARCH
		set_headurl(row[fieldHeadUrl].get_string());
#else
		set_headurl(row[fieldHeadUrl].c_str());
#endif
#ifndef NEWARCH
		set_tinyurl(row[fieldTinyUrl].get_string());
#else
		set_tinyurl(row[fieldTinyUrl].c_str());
#endif
	}
	
	void setConfigProperties(mysqlpp::Row& row, const char* fieldBasicConfig, const char* fieldStatusConfig, const char* fieldProfilePrivacy) {
		set_basicconfig((int)row[fieldBasicConfig]);
		set_statusconfig((int) row[fieldStatusConfig]);
#ifndef NEWARCH
		set_profileprivacy(row[fieldProfilePrivacy].get_string());
#else
		set_profileprivacy(row[fieldProfilePrivacy].c_str());
#endif
	}

			
	void setBornProperties(mysqlpp::Row& row, const char* fieldGender, const char* fieldBirthYear, const char* fieldBirthMonth, const char* fieldBirthDay, const char* fieldHomeProvince, const char* fieldHomeCity) {
#ifndef NEWARCH
		set_gender(row[fieldGender].get_string());
#else
		set_gender(row[fieldGender].c_str());
#endif
#ifndef NEWARCH
		set_homeprovince(row[fieldHomeProvince].get_string());
#else
		set_homeprovince(row[fieldHomeProvince].c_str());
#endif
#ifndef NEWARCH
		set_homecity(row[fieldHomeCity].get_string());
#else
		set_homecity(row[fieldHomeCity].c_str());
#endif
		set_birthyear((int)row[fieldBirthYear]);
		set_birthmonth((int)row[fieldBirthMonth]);
		set_birthday((int)row[fieldBirthDay]);
	}

	void setStageProperties(mysqlpp::Row& row, const char* fieldUniv, const char* fieldStage) {
		set_univ((int) row[fieldUniv]);
		set_stage((int) row[fieldStage]);
	}

};

typedef IceUtil::Handle<UserApiCacheMemData> UserApiCacheMemDataPtr;
	
class DistUserApiCacheAdapter : public xce::distcache::XceDistCacheRWAdapter<UserApiCacheMemData, UserApiCacheMemDataPtr>, public MyUtil::Singleton<DistUserApiCacheAdapter> {
public:
	void initialize() {
    std::string name("UserApiCacheD");
    init(name);
	}
};

}
}
}
#endif
