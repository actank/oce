#include "UserApiCacheDataI.h"
#include "ServiceI.h"

using namespace ::mysqlpp;
using namespace ::xce::userapicache;
using namespace MyUtil;
using namespace ::com::xiaonei::xce;

void UserApiCacheDataI::setUserBornProperties(Row& row, const char* fieldGender,const char* fieldBirthYear, const char* fieldBirthMonth, const char* fieldBirthDay, const char* fieldHomeProvince, const char* fieldHomeCity) {
	if(row[ fieldBirthDay ].is_null()) birthday = 0;
	else birthday = (int)row[ fieldBirthDay ];
	
	if(row[ fieldBirthMonth ].is_null()) birthmonth = 0;
	else birthmonth = (int)row[ fieldBirthMonth ];
	
	if(row[ fieldBirthYear ].is_null()) birthyear = 0;
	else birthyear = (int)row[ fieldBirthYear ];
#ifndef NEWARCH
	gender=row[fieldGender].get_string();
	homeprovince=row[fieldHomeProvince].get_string();
	homecity=row[fieldHomeCity].get_string();
#else
	gender=row[fieldGender].data();
	homeprovince=row[fieldHomeProvince].data();
	homecity=row[fieldHomeCity].data();
#endif
	tablenum_++;
}

void UserApiCacheDataI::setUserStageProperties(Row& row, const char* fieldUniv, const char* fieldStage) {
	univ =(int)row[fieldUniv];
	stage=(int)row[fieldStage];
	tablenum_++;
}

void UserApiCacheDataI::setUserConfigProperties(Row& row, const char* fieldBasicConfig, const char* fieldStatusConfig, const char* fieldProfilePrivacy) {
	basicconfig= (int)row[fieldBasicConfig];
	statusconfig=(int)row[fieldStatusConfig];
#ifndef NEWARCH
	profileprivacy=row[fieldProfilePrivacy].get_string();
#else
	profileprivacy=row[fieldProfilePrivacy].data();
#endif
	tablenum_++;
}

void UserApiCacheDataI::setUserUrlProperties(Row& row, const char* fieldMainUrl, const char* fieldHeadUrl, const char* fieldTinyUrl) {
#ifndef NEWARCH
	mainurl=row[fieldMainUrl].get_string();
	headurl=row[fieldHeadUrl].get_string();
	tinyurl=row[fieldTinyUrl].get_string();
#else
	mainurl=row[fieldMainUrl].data();
	headurl=row[fieldHeadUrl].data();
	tinyurl=row[fieldTinyUrl].data();
#endif
	tablenum_++;
}

void UserApiCacheDataI::setUserPassportProperties(Row& row,  const char* fieldId, const char* fieldStatus) {
	id=(int)row[fieldId];
	status=(int)row[fieldStatus];
	tablenum_++;
}
void UserApiCacheDataI::setUserNameProperties(Row& row, const char* fieldName) {
#ifndef NEWARCH
	name=row[fieldName].get_string();
#else
	name=row[fieldName].data();
#endif
	tablenum_++;
}

void UserApiCacheDataI::setUserRightProperties(Row& row,  const char* fieldAuth) {
	auth=(int)row[fieldAuth];
	tablenum_++;
}

void UserApiCacheDataI::setUserStateProperties(Row& row,  const char* fieldState, const char* fieldLevel) {
	state=(int)row[fieldState];
	level=(int)row[fieldLevel];
	tablenum_++;
}
