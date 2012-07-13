#ifndef USERCACHEDATAI_H_
#define USERCACHEDATAI_H_

#include <UserCache.h>
#include <IceUtil/IceUtil.h>
#include <boost/lexical_cast.hpp>

namespace xce {
namespace usercache {

const int TABLE_NUMBER = 5;

class UserCacheDataI: virtual public UserCacheData3 {
public:
	UserCacheDataI() : UserCacheData3(0, 0, 0, 0, 0, "", "", 0, 0), in_tables(0) {
	}

	virtual ~UserCacheDataI() {}

	void setUserPassportProperties(mysqlpp::Row& row, const char* fieldStatus) {
		status = (int) row[fieldStatus];
	}

	void setStateProperties(mysqlpp::Row& row, const char* fieldState, const char* fieldLevel) {
		state = (int) row[fieldState];
		level = (int) row[fieldLevel];
		in_tables++;
	}

	void setUrlProperties(mysqlpp::Row& row, const char* fieldTinyUrl) {
#ifndef NEWARCH
		tinyUrl = row[fieldTinyUrl].get_string();
#else
		tinyUrl = row[fieldTinyUrl].c_str();
#endif
		in_tables++;
	}

	void setNamesProperties(mysqlpp::Row& row, const char* fieldName) {
#ifndef NEWARCH
		name = row[fieldName].get_string();
#else
		name = row[fieldName].c_str();
#endif
		in_tables++;
	}

	void setBornProperties(mysqlpp::Row& row,
			const char* fieldGender) {
		std::string tmpgender;
#ifndef NEWARCH
		tmpgender = row[fieldGender].get_string();
#else
		tmpgender = row[fieldGender].c_str();
#endif
		if (tmpgender == "男生") {
			gender = 0x0f&1;
		} else if (tmpgender == "女生") {
			gender = 0x0f&2;
		} else {
			gender = 0x0f&0;
		}
		in_tables++;
	}

	void setStageProperties(mysqlpp::Row& row,
			const char* fieldUniv) {
		univ = (int) row[fieldUniv];
		//stage = (int) row[fieldStage];
		in_tables++;
	}

	int in_tables;
};

typedef IceUtil::Handle<UserCacheDataI> UserCacheDataIPtr;
typedef std::vector<UserCacheDataIPtr> UserCacheDataISeq;

}
}

#endif /* USERCACHEDATAI_H_ */
