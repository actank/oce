#include "UserAdsCacheDataI.h"
#include "ServiceI.h"

using namespace ::mysqlpp;
using namespace ::xce::useradscache;
using namespace MyUtil;
using namespace ::com::xiaonei::xce;

UserAdsCacheGeneralDataI::UserAdsCacheGeneralDataI() {
	id=0;
	stage=0;
	sex="";
	birthyear=0;
	birthmonth=0;
	birthday=0;
	homeprovince="";
	homecity="";
	seatprovince="";
	seatcity="";

	//mainurl="";
	//headurl="";
	tinyurl="";
	//largeurl="";

}
	

void UserAdsCacheGeneralDataI::setUserBornProperties(Row& row, const char* fieldGender, const char* fieldBirthYear, const char* fieldBirthMonth, const char* fieldBirthDay, const char* fieldHomeProvince, const char* fieldHomeCity) {
	birthyear=(int)row[fieldBirthYear];
	birthmonth=(int)row[fieldBirthMonth];
	birthday=(int)row[fieldBirthDay];
#ifndef NEWARCH
	sex=row[fieldGender].get_string();
	homeprovince=row[fieldHomeProvince].get_string();
	homecity=row[fieldHomeCity].get_string();
#else
	sex=row[fieldGender].data();
	homeprovince=row[fieldHomeProvince].data();
	homecity=row[fieldHomeCity].data();
#endif
}
void UserAdsCacheGeneralDataI::setUserStageProperties(Row& row, const char* fieldStage) {
	stage=(int)row[fieldStage];
}

void UserAdsCacheGeneralDataI::setUserUrlProperties(Row& row, const char* fieldTinyUrl){
#ifndef NEWARCH
	//mainurl=row[fieldMainUrl].get_string();
	//headurl=row[fieldHeadUrl].get_string();
	tinyurl=row[fieldTinyUrl].get_string();
	//largeurl=row[fieldLargeUrl].get_string();
#else
	//mainurl=row[fieldMainUrl].data();
	//headurl=row[fieldHeadUrl].data();
	tinyurl=row[fieldTinyUrl].data();
	//largeurl=row[fieldLargeUrl].data();
#endif
}

void UserAdsCacheGeneralDataI::setWorkplaceProperties(Row& row, const char* fieldWorkplaceName, const char* fieldJobTitleId, const char* fieldPositionId) {
	WorkplaceInfoPtr workplace=new WorkplaceInfo;
#ifndef NEWARCH
	workplace->workplacename=row[fieldWorkplaceName].get_string();
#else
	workplace->workplacename=row[fieldWorkplaceName].data();
#endif
	workplace->jobtitleid=(int)row[fieldJobTitleId];
	workplace->positionid=(int)row[fieldPositionId];
	workplaces.push_back(workplace);
}

void UserAdsCacheGeneralDataI::setRegionProperties(Row& row, const char* fieldProvinceName, const char* fieldCityName) {
#ifndef NEWARCH
	seatprovince=row[fieldProvinceName].get_string();
	seatcity=row[fieldCityName].get_string();
#else
	seatprovince=row[fieldProvinceName].data();
	seatcity=row[fieldCityName].data();
#endif
}

void UserAdsCacheGeneralDataI::setNetworkProperties(Row& row, const char* fieldNetworkId, const char* fieldStage, const char* fieldStatus) {
	NetworkInfoPtr network=new NetworkInfo;
	network->networkid=(int)row[fieldNetworkId];
	network->stage=(int)row[fieldStage];
	network->status=(int)row[fieldStatus];
	networks.push_back(network);
}

void UserAdsCacheGeneralDataI::setElementaryProperties(Row& row, const char* fieldElementarySchoolId, const char* fieldElementarySchoolName, const char* fieldElementarySchoolYear) {
	SchoolInfoPtr school=new SchoolInfo;
	school->type=Elementary;
	school->id=(int)row[fieldElementarySchoolId];
#ifndef NEWARCH
	school->name=row[fieldElementarySchoolName].get_string();
#else
	school->name=row[fieldElementarySchoolName].data();
#endif
	school->enrollyear=(int)row[fieldElementarySchoolYear];
	school->department="";
	school->dorm="";
	{
        IceUtil::Mutex::Lock lock(*this);
	schools.push_back(school);
	}
}

void UserAdsCacheGeneralDataI::setJuniorProperties(Row& row, const char* fieldJuniorHighSchoolId, const char* fieldJuniorHighSchoolName, const char* fieldJuniorHighSchoolYear) {
	SchoolInfoPtr school=new SchoolInfo;
	school->type=Junior;
	school->id=(int)row[fieldJuniorHighSchoolId];
#ifndef NEWARCH
	school->name=row[fieldJuniorHighSchoolName].get_string();
#else
	school->name=row[fieldJuniorHighSchoolName].data();
#endif
	school->enrollyear=(int)row[fieldJuniorHighSchoolYear];
	school->department="";
	school->dorm="";
	{
        IceUtil::Mutex::Lock lock(*this);
	schools.push_back(school);
	}
}

void UserAdsCacheGeneralDataI::setSeniorProperties(Row& row, const char* fieldHighSchoolId, const char* fieldHighSchoolName, const char* fieldEnrollYear) {
	SchoolInfoPtr school=new SchoolInfo;
	school->type=High;
	school->id=(int)row[fieldHighSchoolId];
#ifndef NEWARCH
	school->name=row[fieldHighSchoolName].get_string();
#else
	school->name=row[fieldHighSchoolName].data();
#endif
	school->enrollyear=(int)row[fieldEnrollYear];
	school->department="";
	school->dorm="";
	{
        IceUtil::Mutex::Lock lock(*this);
	schools.push_back(school);
	}
}

void UserAdsCacheGeneralDataI::setCollegeProperties(Row& row, const char* fieldCollegeId, const char* fieldCollegeName, const char* fieldEnrollYear,const char* fieldDepartment) {
	SchoolInfoPtr school=new SchoolInfo;
	school->type=College;
	school->id=(int)row[fieldCollegeId];
	school->enrollyear=(int)row[fieldEnrollYear];
	school->dorm="";
#ifndef NEWARCH
	school->name=row[fieldCollegeName].get_string();
	school->department=row[fieldDepartment].get_string();
#else
	school->name=row[fieldCollegeName].data();
	school->department=row[fieldDepartment].data();
#endif
	{
        IceUtil::Mutex::Lock lock(*this);
	schools.push_back(school);
	}
}

void UserAdsCacheGeneralDataI::setUniversityProperties(Row& row, const char* fieldUniversityId, const char* fieldUniversityName, const char* fieldEnrollYear, const char* fieldDepartment, const char* fieldDorm) {
	SchoolInfoPtr school=new SchoolInfo;
	school->type=University;
	school->id=(int)row[fieldUniversityId];
	school->enrollyear=(int)row[fieldEnrollYear];
#ifndef NEWARCH
	school->name=row[fieldUniversityName].get_string();
	school->department=row[fieldDepartment].get_string();
	school->dorm=row[fieldDorm].get_string();
#else
	school->name=row[fieldUniversityName].data();
	school->department=row[fieldDepartment].data();
	school->dorm=row[fieldDorm].data();
#endif
	{
        IceUtil::Mutex::Lock lock(*this);
	schools.push_back(school);
	}
}

UserAdsCacheDataPtr UserAdsCacheGeneralDataI::toOldData(){
	UserAdsCacheDataPtr oldData = new UserAdsCacheData;
	
	oldData->id = id;
	oldData->stage = stage;
	oldData->sex = sex;
	oldData->birthyear = birthyear;
	oldData->birthmonth = birthmonth;
	oldData->birthday = birthday;
	oldData->homeprovince = homeprovince;
	oldData->homecity = homecity;
	oldData->seatprovince = seatprovince;
	oldData->seatcity = seatcity;
	
	oldData->schools = schools;
	oldData->workplaces = workplaces;
	oldData->networks = networks;
	return oldData;
}

