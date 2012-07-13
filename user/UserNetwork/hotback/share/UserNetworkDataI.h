#ifndef __USER_NETWORK_DATAI_H__
#define __USER_NETWORK_DATAI_H__

#include "UserNetwork.h"
#include "ObjectCacheI.h"

#include "BinString.cpp"

#include <IceUtil/RWRecMutex.h>
#include "mysql++/mysql++.h"

namespace mop {
namespace hi {
namespace svc {
namespace model {

#define GET_INT_VALUE(x) do {\
    x = *(int*)p;\
    p+=sizeof(int);\
    total -= sizeof(int);\
    if (total < 0) { \
        MCE_WARN("GET_INT_VALUE is Error! total<0"); \
        return ;\
    }\
}while(0)

#define GET_STRING_VALUE(x) do {\
    int strlen = *(int*)p;\
    p+=sizeof(int);\
    total -= sizeof(int);\
    if (total < 0 || strlen < 0) { \
        MCE_WARN("total=" << total << "strlen =" << strlen); \
        return ;\
    }\
    total -= strlen;\
    if (total < 0) { \
        MCE_WARN("total < 0"); \
        return ;\
    }\
    x = string(p, strlen);\
    p+=strlen;\
}while(0)

class NetworkDataI: public NetworkData {
public:

	NetworkDataI() {
	}

	NetworkDataI(NetworkDataPtr data) {
		userId = data->userId;
		networks = data->networks;
		highschools = data->highschools;
		universities = data->universities;
		workspaces = data->workspaces;
		colleges = data->colleges;
		elementaries = data->elementaries;
		juniorhighschools = data->juniorhighschools;
		regions = data->regions;
	}

	NetworkDataI(::xce::binstring::BinStringPtr& bp) {
		unSerialize(bp);
	}

public:

	void setNetworks(const mysqlpp::Row& row) {
		NetworkPtr rs = new Network;

		rs->id = (int) (row["id"]);
		rs->userId = (int) (row["userid"]);
		rs->stage = (int) (row["stage"]);
		rs->infoId = (int) (row["info_id"]);
#ifndef NEWARCH
		rs->joinTime = row["join_time"].get_string();
		rs->networkName = row["network_name"].get_string();
#else
		rs->joinTime = row["join_time"].data();
		rs->networkName = row["network_name"].data();
#endif
		rs->status = (int) (row["status"]);
		rs->networkId = (int) (row["network_id"]);

		networks.push_back(rs);
	}

	void setUniversities(const mysqlpp::Row& row) {
		UniversityInfoPtr rs = new UniversityInfo;

		rs->id = (int) (row["id"]);
		rs->userId = (int) (row["userid"]);
		rs->universityId = (int) (row["university_id"]);
#ifndef NEWARCH
		rs->universityName = row["university_name"].get_string();
		rs->dorm = row["dorm"].get_string();
		rs->department = row["department"].get_string();
		rs->typeOfCourse = row["type_of_course"].get_string();
#else
		rs->universityName = row["university_name"].data();
		rs->dorm = row["dorm"].data();
		rs->department = row["department"].data();
		rs->typeOfCourse = row["type_of_course"].data();
#endif
		rs->enrollYear = (int) (row["enroll_year"]);

		universities.push_back(rs);
	}

	void setHighSchools(const mysqlpp::Row& row) {
		HighSchoolInfoPtr rs = new HighSchoolInfo;

		rs->id = (int) (row["id"]);
		rs->userId = (int) (row["userid"]);
		rs->HighSchoolId = (int) (row["high_school_id"]);
#ifndef NEWARCH
		rs->HighSchoolName = row["high_school_name"].get_string()
		rs->hClass1 = row["h_class1"].get_string();
		rs->hClass2 = row["h_class2"].get_string();
		rs->hClass3 = row["h_class3"].get_string();
#else
		rs->HighSchoolName = row["high_school_name"].data();
		rs->hClass1 = row["h_class1"].data();
		rs->hClass2 = row["h_class2"].data();
		rs->hClass3 = row["h_class3"].data();
#endif
		rs->enrollYear = (int) (row["enroll_year"]);

		highschools.push_back(rs);
	}
	void setWorkspaces(const mysqlpp::Row& row) {
		WorkspaceInfoPtr rs = new WorkspaceInfo;

		rs->id = (int) (row["id"]);
		rs->userId = (int) (row["userid"]);
		rs->workplaceId = (int) (row["workplace_id"]);
#ifndef NEWARCH
		rs->workplaceName = row["workplace_name"].get_string();
		rs->address = row["address"].get_string();
		rs->description = row["description"].get_string();
		rs->province = row["province"].get_string();
		rs->cityName = row["city_name"].get_string();
#else
		rs->workplaceName = row["workplace_name"].data();
		rs->address = row["address"].data();
		rs->description = row["description"].data();
		rs->province = row["province"].data();
		rs->cityName = row["city_name"].data();
#endif
		rs->cityId = (int) (row["city_id"]);
		rs->joinYear = (int) (row["join_year"]);
		rs->joinMonth = (int) (row["join_month"]);
		rs->quitYear = (int) (row["quit_year"]);
		rs->quitMonth = (int) (row["quit_month"]);
		rs->type = (int) (row["type"]);
		rs->jobTitleId = (int) (row["job_title_id"]);
		rs->positionId = (int) (row["position_id"]);

		workspaces.push_back(rs);
	}
	void setColleges(const mysqlpp::Row& row) {
		CollegeInfoPtr rs = new CollegeInfo;

		rs->id = (int) (row["id"]);
		rs->userId = (int) (row["userid"]);
		rs->collegeId = (int) (row["college_id"]);
#ifndef NEWARCH
		rs->collegeName = row["college_name"].get_string();
		rs->department = row["department"].get_string();
#else
		rs->collegeName = row["college_name"].data();
		rs->department = row["department"].data();
#endif
		rs->enrollYear = (int) (row["enroll_year"]);

		colleges.push_back(rs);
	}
	void setElementaries(const mysqlpp::Row& row) {
		ElementarySchoolInfoNPtr rs = new ElementarySchoolInfoN;
		rs->id = (int) (row["id"]);
		rs->userId = (int) (row["userid"]);
		rs->elementarySchoolId = (int) (row["elementary_school_id"]);
#ifndef NEWARCH
		rs->elementarySchoolName = row["elementary_school_name"].get_string();
#else
		rs->elementarySchoolName = row["elementary_school_name"].data();
#endif
		rs->elementarySchoolYear = (int) (row["elementary_school_year"]);

		elementaries.push_back(rs);
	}
	void setJuniorHighSchools(const mysqlpp::Row& row) {
		JuniorHighSchoolInfoNPtr rs = new JuniorHighSchoolInfoN;
		rs->id = (int) (row["id"]);
		rs->userId = (int) (row["userid"]);
		rs->juniorHighSchoolId = (int) (row["junior_high_school_id"]);
#ifndef NEWARCH
		rs->juniorHighSchoolName = row["junior_high_school_name"].get_string();
#else
		rs->juniorHighSchoolName = row["junior_high_school_name"].data();
#endif
		rs->juniorHighSchoolYear = (int) (row["junior_high_school_year"]);

		juniorhighschools.push_back(rs);
	}
	void setRegions(const mysqlpp::Row& row) {
		RegionInfoPtr rs = new RegionInfo;

		rs->id = (int) (row["id"]);
		rs->userId = (int) (row["userid"]);
		rs->regionId = (int) (row["region_id"]);
#ifndef NEWARCH
		rs->provinceName = row["province_name"].get_string();
		rs->cityName = row["city_name"].get_string();
#else
		rs->provinceName = row["province_name"].data();
		rs->cityName = row["city_name"].data();
#endif

		regions.push_back(rs);
	}

	::xce::binstring::BinStringPtr serialize() const {
		::xce::binstring::BinStringPtr binString = new ::xce::binstring::BinString();

		binString->add(userId);

		binString->add((int) networks.size());
		for (NetworkSeq::const_iterator it = networks.begin(); it != networks.end(); ++it) {
			binString->add((*it)->id);
			binString->add((*it)->stage);
			binString->add((*it)->infoId);
			binString->add((*it)->joinTime);
			binString->add((*it)->status);
			binString->add((*it)->networkName);
			binString->add((*it)->infoType);
			binString->add((*it)->networkId);
		}

		binString->add((int) highschools.size());
		for (HighSchoolInfoSeq::const_iterator it = highschools.begin(); it != highschools.end(); ++it) {
			binString->add((*it)->id);
			binString->add((*it)->HighSchoolId);
			binString->add((*it)->HighSchoolName);
			binString->add((*it)->enrollYear);
			binString->add((*it)->hClass1);
			binString->add((*it)->hClass2);
			binString->add((*it)->hClass3);
		}

		binString->add((int) universities.size());
		for (UniversityInfoSeq::const_iterator it = universities.begin(); it != universities.end(); ++it) {
			binString->add((*it)->id);
			binString->add((*it)->universityId);
			binString->add((*it)->universityName);
			binString->add((*it)->dorm);
			binString->add((*it)->department);
			binString->add((*it)->typeOfCourse);
			binString->add((*it)->enrollYear);
		}

		binString->add((int) workspaces.size());
		for (WorkspaceInfoSeq::const_iterator it = workspaces.begin(); it != workspaces.end(); ++it) {
			binString->add((*it)->id);
			binString->add((*it)->workplaceId);
			binString->add((*it)->workplaceName);
			binString->add((*it)->address);
			binString->add((*it)->description);
			binString->add((*it)->province);
			binString->add((*it)->cityId);
			binString->add((*it)->cityName);
			binString->add((*it)->joinYear);
			binString->add((*it)->joinMonth);
			binString->add((*it)->quitYear);
			binString->add((*it)->quitMonth);
			binString->add((*it)->type);
			binString->add((*it)->jobTitleId);
			binString->add((*it)->positionId);
		}

		binString->add((int) colleges.size());
		for (CollegeInfoSeq::const_iterator it = colleges.begin(); it != colleges.end(); ++it) {
			binString->add((*it)->id);
			binString->add((*it)->collegeId);
			binString->add((*it)->collegeName);
			binString->add((*it)->department);
			binString->add((*it)->enrollYear);
		}

		binString->add((int) elementaries.size());
		for (ElementarySchoolInfoSeq::const_iterator it = elementaries.begin(); it != elementaries.end(); ++it) {
			binString->add(ElementarySchoolInfoNPtr::dynamicCast(*it)->id);
			binString->add((*it)->elementarySchoolId);
			binString->add((*it)->elementarySchoolName);
			binString->add((*it)->elementarySchoolYear);
		}

		binString->add((int) juniorhighschools.size());
		for (JuniorHighSchoolInfoSeq::const_iterator it = juniorhighschools.begin(); it != juniorhighschools.end(); ++it) {
			binString->add(JuniorHighSchoolInfoNPtr::dynamicCast(*it)->id);
			binString->add((*it)->juniorHighSchoolId);
			binString->add((*it)->juniorHighSchoolName);
			binString->add((*it)->juniorHighSchoolYear);
		}

		binString->add((int) regions.size());
		for (RegionInfoSeq::const_iterator it = regions.begin(); it != regions.end(); ++it) {
			binString->add((*it)->id);
			binString->add((*it)->regionId);
			binString->add((*it)->provinceName);
			binString->add((*it)->cityName);
		}

		binString->compress();

		return binString;

	}

	void unSerialize(::xce::binstring::BinStringPtr& bp) {
		char* p = 0;

		int total = 0;

		bp->unCompress(&p, &total);

		if (p == 0 || total == 0) {
			MCE_WARN("NetworkDataI::unSerialize can't locate data from memery!");
			return;
		}

		char *temp = p;

		GET_INT_VALUE(userId);

		int vector_size = 0;

		GET_INT_VALUE(vector_size);
		for (int i = 0; i < vector_size; ++i) {
			NetworkPtr network = new Network();
			network->userId = userId;
			GET_INT_VALUE(network->id);
			GET_INT_VALUE(network->stage);
			GET_INT_VALUE(network->infoId);
			GET_STRING_VALUE(network->joinTime);
			GET_INT_VALUE(network->status);
			GET_STRING_VALUE(network->networkName);
			GET_INT_VALUE(network->infoType);
			GET_INT_VALUE(network->networkId);
			networks.push_back(network);
		}

		GET_INT_VALUE(vector_size);
		for (int i = 0; i < vector_size; ++i) {
			HighSchoolInfoPtr highSchool = new HighSchoolInfo;
			highSchool->userId = userId;
			GET_INT_VALUE(highSchool->id);
			GET_INT_VALUE(highSchool->HighSchoolId);
			GET_STRING_VALUE(highSchool->HighSchoolName);
			GET_INT_VALUE(highSchool->enrollYear);
			GET_STRING_VALUE(highSchool->hClass1);
			GET_STRING_VALUE(highSchool->hClass2);
			GET_STRING_VALUE(highSchool->hClass3);
			highschools.push_back(highSchool);
		}

		GET_INT_VALUE(vector_size);
		for (int i = 0; i < vector_size; ++i) {
			UniversityInfoPtr university = new UniversityInfo;
			university->userId = userId;
			GET_INT_VALUE(university->id);
			GET_INT_VALUE(university->universityId);
			GET_STRING_VALUE(university->universityName);
			GET_STRING_VALUE(university->dorm);
			GET_STRING_VALUE(university->department);
			GET_STRING_VALUE(university->typeOfCourse);
			GET_INT_VALUE(university->enrollYear);
			universities.push_back(university);
		}

		GET_INT_VALUE(vector_size);
		for (int i = 0; i < vector_size; ++i) {
			WorkspaceInfoPtr workspace = new WorkspaceInfo;
			workspace->userId = userId;
			GET_INT_VALUE(workspace->id);
			GET_INT_VALUE(workspace->workplaceId);
			GET_STRING_VALUE(workspace->workplaceName);
			GET_STRING_VALUE(workspace->address);
			GET_STRING_VALUE(workspace->description);
			GET_STRING_VALUE(workspace->province);
			GET_INT_VALUE(workspace->cityId);
			GET_STRING_VALUE(workspace->cityName);
			GET_INT_VALUE(workspace->joinYear);
			GET_INT_VALUE(workspace->joinMonth);
			GET_INT_VALUE(workspace->quitYear);
			GET_INT_VALUE(workspace->quitMonth);
			GET_INT_VALUE(workspace->type);
			GET_INT_VALUE(workspace->jobTitleId);
			GET_INT_VALUE(workspace->positionId);
			workspaces.push_back(workspace);
		}

		GET_INT_VALUE(vector_size);
		for (int i = 0; i < vector_size; ++i) {
			CollegeInfoPtr college = new CollegeInfo;
			college->userId = userId;
			GET_INT_VALUE(college->id);
			GET_INT_VALUE(college->collegeId);
			GET_STRING_VALUE(college->collegeName);
			GET_STRING_VALUE(college->department);
			GET_INT_VALUE(college->enrollYear);
			colleges.push_back(college);
		}

		GET_INT_VALUE(vector_size);
		for (int i = 0; i < vector_size; ++i) {
			ElementarySchoolInfoNPtr elementary = new ElementarySchoolInfoN;
			elementary->userId = userId;
			GET_INT_VALUE(elementary->id);
			GET_INT_VALUE(elementary->elementarySchoolId);
			GET_STRING_VALUE(elementary->elementarySchoolName);
			GET_INT_VALUE(elementary->elementarySchoolYear);
			elementaries.push_back(elementary);
		}

		GET_INT_VALUE(vector_size);
		for (int i = 0; i < vector_size; ++i) {
			JuniorHighSchoolInfoNPtr juniorHighSchool = new JuniorHighSchoolInfoN;
			juniorHighSchool->userId = userId;
			GET_INT_VALUE(juniorHighSchool->id);
			GET_INT_VALUE(juniorHighSchool->juniorHighSchoolId);
			GET_STRING_VALUE(juniorHighSchool->juniorHighSchoolName);
			GET_INT_VALUE(juniorHighSchool->juniorHighSchoolYear);
			juniorhighschools.push_back(juniorHighSchool);
		}

		GET_INT_VALUE(vector_size);
		for (int i = 0; i < vector_size; ++i) {
			RegionInfoPtr region = new RegionInfo;
			region->userId = userId;
			GET_INT_VALUE(region->id);
			GET_INT_VALUE(region->regionId);
			GET_STRING_VALUE(region->provinceName);
			GET_STRING_VALUE(region->cityName);
			regions.push_back(region);
		}

		if (temp != 0) {
			delete temp;
			temp = 0;
		}
	}
};

typedef IceUtil::Handle<NetworkDataI> NetworkDataIPtr;

enum INFO_TYPE {
	NETWORKINFO = 0, HIGHSCHOOLINFO, UNIVERSITYINFO, WORKSPACEINFO, COLLEGEINFO, ELEMENTARYSCHOOLINFO, JUNIORHIGHSCHOOLINFO, REGIONINFO
};

enum CHANGE_TYPE {
	INFOTYPEID, INFOID, INFOSTAGE, INFOALL
};

class NetworkCacheData: public Ice::Object {

public:

	NetworkCacheData(::xce::binstring::BinStringPtr bp, Ice::Byte loadFlag = (Ice::Byte) 0xff) :
		_bp(bp), _loadFlag(loadFlag) {
	}

public:

	NetworkDataIPtr getData() {
		IceUtil::RWRecMutex::RLock lock(_mutex);
		return new NetworkDataI(_bp);
	}

	void addInfo(const MyUtil::Str2StrMap& data, INFO_TYPE type) {

		IceUtil::RWRecMutex::WLock lock(_mutex);

		NetworkDataIPtr source = new NetworkDataI(_bp);

		switch (type) {
		case NETWORKINFO: {
			NetworkPtr rs = new Network;
			rs->userId = 0;
			rs->stage = 0;
			rs->infoId = 0;
			rs->joinTime = "1970-01-01 00:00:00";
			rs->networkName = "";
			rs->status = 0;
			rs->networkId = 0;
			setNetworkInfo(rs, data);

			NetworkSeq::iterator findIt = source->networks.begin();
			for (; findIt != source->networks.end(); ++findIt) {
				if ((*findIt)->id == rs->id) {
					break;
				}
			}

			if (findIt == source->networks.end()) {
				source->networks.push_back(rs);
			}
		}
			break;
		case HIGHSCHOOLINFO: {
			HighSchoolInfoPtr rs = new HighSchoolInfo;
			rs->userId = 0;
			rs->HighSchoolId = 0;
			rs->HighSchoolName = "";
			rs->hClass1 = "";
			rs->hClass2 = "";
			rs->hClass3 = "";
			rs->enrollYear = 0;
			setHighSchoolInfo(rs, data);

			HighSchoolInfoSeq::iterator findIt = source->highschools.begin();
			for (; findIt != source->highschools.end(); ++findIt) {
				if ((*findIt)->id == rs->id) {
					break;
				}
			}

			if (findIt == source->highschools.end()) {
				source->highschools.push_back(rs);
			}

		}
			break;
		case UNIVERSITYINFO: {
			UniversityInfoPtr rs = new UniversityInfo;
			rs->userId = 0;
			rs->universityId = 0;
			rs->universityName = "";
			rs->dorm = "";
			rs->department = "";
			rs->typeOfCourse = "";
			rs->enrollYear = 0;
			setUniversityInfo(rs, data);

			UniversityInfoSeq::iterator findIt = source->universities.begin();
			for (; findIt != source->universities.end(); ++findIt) {
				if ((*findIt)->id == rs->id) {
					break;
				}
			}

			if (findIt == source->universities.end()) {
				source->universities.push_back(rs);
			}

		}
			break;
		case WORKSPACEINFO: {
			WorkspaceInfoPtr rs = new WorkspaceInfo;
			rs->userId = 0;
			rs->workplaceId = 0;
			rs->workplaceName = "";
			rs->address = "";
			rs->description = "";
			rs->province = "";
			rs->cityName = "";
			rs->cityId = 0;
			rs->joinYear = 0;
			rs->joinMonth = 0;
			rs->quitYear = 0;
			rs->quitMonth = 0;
			rs->type = 1;
			rs->jobTitleId = 0;
			rs->positionId = 0;
			setWorkspaceInfo(rs, data);

			WorkspaceInfoSeq::iterator findIt = source->workspaces.begin();
			for (; findIt != source->workspaces.end(); ++findIt) {
				if ((*findIt)->id == rs->id) {
					break;
				}
			}

			if (findIt == source->workspaces.end()) {
				source->workspaces.push_back(rs);
			}

		}
			break;
		case COLLEGEINFO: {
			CollegeInfoPtr rs = new CollegeInfo;
			rs->userId = 0;
			rs->collegeId = 0;
			rs->collegeName = "";
			rs->department = "";
			rs->enrollYear = 0;
			setCollegeInfo(rs, data);

			CollegeInfoSeq::iterator findIt = source->colleges.begin();
			for (; findIt != source->colleges.end(); ++findIt) {
				if ((*findIt)->id == rs->id) {
					break;
				}
			}

			if (findIt == source->colleges.end()) {
				source->colleges.push_back(rs);
			}

		}
			break;
		case ELEMENTARYSCHOOLINFO: {
			ElementarySchoolInfoPtr rs = new ElementarySchoolInfoN;
			rs->userId = 0;
			rs->elementarySchoolId = 0;
			rs->elementarySchoolName = "";
			rs->elementarySchoolYear = 0;
			setElementarySchoolInfo(rs, data);

			ElementarySchoolInfoSeq::iterator findIt = source->elementaries.begin();
			for (; findIt != source->elementaries.end(); ++findIt) {
				if (ElementarySchoolInfoNPtr::dynamicCast(*findIt)->id == ElementarySchoolInfoNPtr::dynamicCast(rs)->id) {
					break;
				}
			}

			if (findIt == source->elementaries.end()) {
				source->elementaries.push_back(rs);
			}

		}
			break;
		case JUNIORHIGHSCHOOLINFO: {
			JuniorHighSchoolInfoPtr rs = new JuniorHighSchoolInfoN;
			rs->userId = 0;
			rs->juniorHighSchoolId = 0;
			rs->juniorHighSchoolName = "";
			rs->juniorHighSchoolYear = 0;
			setJuniorHighSchoolInfo(rs, data);

			JuniorHighSchoolInfoSeq::iterator findIt = source->juniorhighschools.begin();
			for (; findIt != source->juniorhighschools.end(); ++findIt) {
				if (JuniorHighSchoolInfoNPtr::dynamicCast(*findIt)->userId == JuniorHighSchoolInfoNPtr::dynamicCast(rs)->id) {
					break;
				}
			}

			if (findIt == source->juniorhighschools.end()) {
				source->juniorhighschools.push_back(rs);
			}

		}
			break;
		case REGIONINFO: {
			RegionInfoPtr rs = new RegionInfo;
			rs->userId = 0;
			rs->regionId = 0;
			rs->provinceName = "";
			rs->cityName = "";
			setRegionInfo(rs, data);

			RegionInfoSeq::iterator findIt = source->regions.begin();
			for (; findIt != source->regions.end(); ++findIt) {
				if ((*findIt)->id == rs->id) {
					break;
				}
			}

			if (findIt == source->regions.end()) {
				source->regions.push_back(rs);
			}

		}
			break;
		default:
			break;
		}

		setBP(source->serialize(), type);

	}

	void removeInfo(const int typeId, INFO_TYPE type, CHANGE_TYPE changeType) {

		IceUtil::RWRecMutex::WLock lock(_mutex);

		NetworkDataIPtr source = new NetworkDataI(_bp);

		switch (type) {
		case NETWORKINFO: {
			NetworkSeq des;

			removeNetworkInfo(typeId, des, source, changeType);
		}
			break;
		case HIGHSCHOOLINFO: {
			HighSchoolInfoSeq des;

			removeHighSchoolInfo(typeId, des, source, changeType);
		}
			break;
		case UNIVERSITYINFO: {
			UniversityInfoSeq des;

			removeUniversityInfo(typeId, des, source, changeType);
		}
			break;
		case WORKSPACEINFO: {
			WorkspaceInfoSeq des;

			removeWorkspaceInfo(typeId, des, source, changeType);
		}
			break;
		case COLLEGEINFO: {
			CollegeInfoSeq des;

			removeCollegeInfo(typeId, des, source, changeType);
		}
			break;
		case ELEMENTARYSCHOOLINFO: {
			ElementarySchoolInfoSeq des;
			removeElementarySchoolInfo(typeId, des, source, changeType);
			//source->elementaries.swap(des);
		}
			break;
		case JUNIORHIGHSCHOOLINFO: {
			JuniorHighSchoolInfoSeq des;
			removeJuniorHighSchoolInfo(typeId, des, source, changeType);
			//source->juniorhighschools.swap(des);
		}
			break;
		case REGIONINFO: {
			RegionInfoSeq des;

			removeRegionInfo(typeId, des, source, changeType);
		}
			break;
		default:
			break;
		}

		setBP(source->serialize(), type);

	}

	void setInfo(const int typeId, const MyUtil::Str2StrMap& data, INFO_TYPE type) {

		IceUtil::RWRecMutex::WLock lock(_mutex);

		NetworkDataIPtr source = new NetworkDataI(_bp);

		switch (type) {
		case NETWORKINFO:
			for (NetworkSeq::iterator it = source->networks.begin(); it != source->networks.end(); ++it) {
				if ((*it)->networkId == typeId) {
					setNetworkInfo((*it), data);
				}
			}

			break;
		case HIGHSCHOOLINFO:
			for (HighSchoolInfoSeq::iterator it = source->highschools.begin(); it != source->highschools.end(); ++it) {
				if ((*it)->id == typeId) {
					setHighSchoolInfo((*it), data);
				}
			}

			break;
		case UNIVERSITYINFO:
			for (UniversityInfoSeq::iterator it = source->universities.begin(); it != source->universities.end(); ++it) {
				if ((*it)->id == typeId) {
					setUniversityInfo((*it), data);
				}
			}

			break;
		case WORKSPACEINFO:
			for (WorkspaceInfoSeq::iterator it = source->workspaces.begin(); it != source->workspaces.end(); ++it) {
				if ((*it)->id == typeId) {
					setWorkspaceInfo((*it), data);
				}
			}

			break;
		case COLLEGEINFO:
			for (CollegeInfoSeq::iterator it = source->colleges.begin(); it != source->colleges.end(); ++it) {
				if ((*it)->id == typeId) {
					setCollegeInfo((*it), data);
				}
			}

			break;
		case REGIONINFO:
			for (RegionInfoSeq::iterator it = source->regions.begin(); it != source->regions.end(); ++it) {
				if ((*it)->regionId == typeId) {
					setRegionInfo((*it), data);
				}
			}

			break;
		case ELEMENTARYSCHOOLINFO:
			for (ElementarySchoolInfoSeq::iterator it = source->elementaries.begin(); it != source->elementaries.end(); ++it) {
				if (ElementarySchoolInfoNPtr::dynamicCast(*it)->id == typeId) {
					setElementarySchoolInfo(*it, data);
				}
			}
			break;
		case JUNIORHIGHSCHOOLINFO:
			for (JuniorHighSchoolInfoSeq::iterator it = source->juniorhighschools.begin(); it != source->juniorhighschools.end(); ++it) {
				if (JuniorHighSchoolInfoNPtr::dynamicCast(*it)->id == typeId) {
					setJuniorHighSchoolInfo(*it, data);
				}
			}
			break;
		default:
			break;
		}

		setBP(source->serialize(), type);

	}

	void setBP(::xce::binstring::BinStringPtr bp, INFO_TYPE type) {
		IceUtil::RWRecMutex::WLock lock(_mutex);
		_loadFlag = (_loadFlag | ((Ice::Byte) 0x01 << type));
		_bp = bp;
	}

	void setData(const NetworkDataIPtr& data, INFO_TYPE type) {
		IceUtil::RWRecMutex::WLock lock(_mutex);

		_loadFlag = (_loadFlag | ((Ice::Byte) 0x01 << type));

		NetworkDataIPtr source = new NetworkDataI(_bp);

		switch (type) {
		case NETWORKINFO:
			source->networks.swap(data->networks);
			break;
		case HIGHSCHOOLINFO:
			source->highschools.swap(data->highschools);
			break;
		case UNIVERSITYINFO:
			source->universities.swap(data->universities);
			break;
		case WORKSPACEINFO:
			source->workspaces.swap(data->workspaces);
			break;
		case COLLEGEINFO:
			source->colleges.swap(data->colleges);
			break;
		case ELEMENTARYSCHOOLINFO:
			source->elementaries.swap(data->elementaries);
			break;
		case JUNIORHIGHSCHOOLINFO:
			source->juniorhighschools.swap(data->juniorhighschools);
			break;
		case REGIONINFO:
			source->regions.swap(data->regions);
			break;
		}

		setBP(source->serialize(), type);
	}

	bool getLoadFlag(INFO_TYPE type) {
		IceUtil::RWRecMutex::RLock lock(_mutex);
		bool isLoad = ((_loadFlag & ((Ice::Byte) 0x01 << type)) == ((Ice::Byte) 0x01 << type)) ? true : false;
		return isLoad;
	}

private:

	void removeNetworkInfo(const int typeId, NetworkSeq& des, NetworkDataIPtr& source, CHANGE_TYPE changeType) {
		switch (changeType) {
		case INFOTYPEID:
			for (NetworkSeq::iterator it = source->networks.begin(); it != source->networks.end(); ++it) {
				if ((*it)->networkId != typeId) {
					des.push_back(*it);
				}
			}
			break;
		case INFOSTAGE:
			for (NetworkSeq::iterator it = source->networks.begin(); it != source->networks.end(); ++it) {
				if ((*it)->stage != typeId) {
					des.push_back(*it);
				}
			}
			break;
		default:
			break;
		}

		source->networks.swap(des);
	}

	void removeHighSchoolInfo(const int typeId, HighSchoolInfoSeq& des, NetworkDataIPtr& source, CHANGE_TYPE changeType) {
		switch (changeType) {
		case INFOID:
			for (HighSchoolInfoSeq::iterator it = source->highschools.begin(); it != source->highschools.end(); ++it) {
				if ((*it)->id != typeId) {
					des.push_back(*it);
				}
			}
			break;
		default:
			break;
		}

		source->highschools.swap(des);
	}

	void removeUniversityInfo(const int typeId, UniversityInfoSeq& des, NetworkDataIPtr& source, CHANGE_TYPE changeType) {
		switch (changeType) {
		case INFOID:
			for (UniversityInfoSeq::iterator it = source->universities.begin(); it != source->universities.end(); ++it) {
				if ((*it)->id != typeId) {
					des.push_back(*it);
				}
			}
			break;
		case INFOTYPEID:
			for (UniversityInfoSeq::iterator it = source->universities.begin(); it != source->universities.end(); ++it) {
				if ((*it)->universityId != typeId) {
					des.push_back(*it);
				}
			}
			break;
		default:
			break;
		}

		source->universities.swap(des);
	}

	void removeWorkspaceInfo(const int typeId, WorkspaceInfoSeq& des, NetworkDataIPtr& source, CHANGE_TYPE changeType) {
		switch (changeType) {
		case INFOID:
			for (WorkspaceInfoSeq::iterator it = source->workspaces.begin(); it != source->workspaces.end(); ++it) {
				if ((*it)->id != typeId) {
					des.push_back(*it);
				}
			}
			break;
		case INFOTYPEID:
			for (WorkspaceInfoSeq::iterator it = source->workspaces.begin(); it != source->workspaces.end(); ++it) {
				if ((*it)->workplaceId != typeId) {
					des.push_back(*it);
				}
			}
			break;
		default:
			break;
		}

		source->workspaces.swap(des);
	}

	void removeCollegeInfo(const int typeId, CollegeInfoSeq& des, NetworkDataIPtr& source, CHANGE_TYPE changeType) {
		switch (changeType) {
		case INFOID:
			for (CollegeInfoSeq::iterator it = source->colleges.begin(); it != source->colleges.end(); ++it) {
				if ((*it)->id != typeId) {
					des.push_back(*it);
				}
			}
			break;
		default:
			break;
		}

		source->colleges.swap(des);
	}

	void removeRegionInfo(const int typeId, RegionInfoSeq& des, NetworkDataIPtr& source, CHANGE_TYPE changeType) {
		switch (changeType) {
		case INFOTYPEID:
			for (RegionInfoSeq::iterator it = source->regions.begin(); it != source->regions.end(); ++it) {
				if ((*it)->regionId != typeId) {
					des.push_back(*it);
				}
			}
			break;
		default:
			break;
		}
		source->regions.swap(des);
	}

	void removeElementarySchoolInfo(const int typeId, ElementarySchoolInfoSeq& des, NetworkDataIPtr& source, CHANGE_TYPE changeType) {
		switch (changeType) {
		case INFOTYPEID:
			for (ElementarySchoolInfoSeq::iterator it = source->elementaries.begin(); it != source->elementaries.end(); ++it) {
				if (ElementarySchoolInfoNPtr::dynamicCast(*it)->elementarySchoolId != typeId) {
					des.push_back(*it);
				}
			}
			break;
		case INFOID:
			for (ElementarySchoolInfoSeq::iterator it = source->elementaries.begin(); it != source->elementaries.end(); ++it) {
				if (ElementarySchoolInfoNPtr::dynamicCast(*it)->id != typeId) {
					des.push_back(*it);
				}
			}
			break;
		default:
			break;
		}
		source->elementaries.swap(des);
	}

	void removeJuniorHighSchoolInfo(const int typeId, JuniorHighSchoolInfoSeq& des, NetworkDataIPtr& source, CHANGE_TYPE changeType) {
		switch (changeType) {
		case INFOTYPEID:
			for (JuniorHighSchoolInfoSeq::iterator it = source->juniorhighschools.begin(); it != source->juniorhighschools.end(); ++it) {
				if ((*it)->juniorHighSchoolId != typeId) {
					des.push_back(*it);
				}
			}
			break;
		case INFOID:
			for (JuniorHighSchoolInfoSeq::iterator it = source->juniorhighschools.begin(); it != source->juniorhighschools.end(); ++it) {
				if (JuniorHighSchoolInfoNPtr::dynamicCast(*it)->id != typeId) {
					des.push_back(*it);
				}
			}
			break;
		default:
			break;
		}
		source->juniorhighschools.swap(des);
	}

	void setNetworkInfo(NetworkPtr& data, MyUtil::Str2StrMap str) {
		for (MyUtil::Str2StrMap::const_iterator it = str.begin(); it != str.end(); ++it) {
			setNetworkProperty(data, it->first, it->second);
		}
	}

	void setNetworkProperty(NetworkPtr& data, const string& key, const string& value) {
		if (!strcasecmp(key.c_str(), NetColID.c_str())) {
			data->id = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), NetColUSERID.c_str())) {
			data->userId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), NetColSTAGE.c_str())) {
			data->stage = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), NetColINFOID.c_str())) {
			data->infoId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), NetColJOINTIME.c_str())) {
			data->joinTime = value;
		} else if (!strcasecmp(key.c_str(), NetColSTATUS.c_str())) {
			data->status = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), NetColNetworkName.c_str())) {
			data->networkName = value;
		} else if (!strcasecmp(key.c_str(), NetColNetworkId.c_str())) {
			data->networkId = boost::lexical_cast<Ice::Int>(value);
		}
	}

	void setHighSchoolInfo(HighSchoolInfoPtr& data, MyUtil::Str2StrMap str) {
		for (MyUtil::Str2StrMap::const_iterator it = str.begin(); it != str.end(); ++it) {
			setHighSchoolProperty(data, it->first, it->second);
		}
	}
	void setHighSchoolProperty(HighSchoolInfoPtr& data, const string& key, const string& value) {
		if (!strcasecmp(key.c_str(), HighColID.c_str())) {
			data->id = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), HighColUSERID.c_str())) {
			data->userId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), HighColHighSchoolID.c_str())) {
			data->HighSchoolId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), HighColHighSchoolNAME.c_str())) {
			data->HighSchoolName = value;
		} else if (!strcasecmp(key.c_str(), HighColENROLLYEAR.c_str())) {
			data->enrollYear = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), HighColHCLASS1.c_str())) {
			data->hClass1 = value;
		} else if (!strcasecmp(key.c_str(), HighColHCLASS2.c_str())) {
			data->hClass2 = value;
		} else if (!strcasecmp(key.c_str(), HighColHCLASS3.c_str())) {
			data->hClass3 = value;
		}
	}

	void setUniversityInfo(UniversityInfoPtr& data, MyUtil::Str2StrMap str) {
		for (MyUtil::Str2StrMap::const_iterator it = str.begin(); it != str.end(); ++it) {
			setUniversityProperty(data, it->first, it->second);
		}
	}
	void setUniversityProperty(UniversityInfoPtr& data, const string& key, const string& value) {
		if (!strcasecmp(key.c_str(), UniColID.c_str())) {
			data->id = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), UniColUSERID.c_str())) {
			data->userId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), UniColUNIVERSITYID.c_str())) {
			data->universityId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), UniColUNIVERSITYNAME.c_str())) {
			data->universityName = value;
		} else if (!strcasecmp(key.c_str(), UniColDORM.c_str())) {
			data->dorm = value;
		} else if (!strcasecmp(key.c_str(), UniColDEPARTMENT.c_str())) {
			data->department = value;
		} else if (!strcasecmp(key.c_str(), UniColTYPEOFCOURSE.c_str())) {
			data->typeOfCourse = value;
		} else if (!strcasecmp(key.c_str(), UniColTYPEENROLLYEAR.c_str())) {
			data->enrollYear = boost::lexical_cast<Ice::Int>(value);
		}
	}

	void setWorkspaceInfo(WorkspaceInfoPtr& data, MyUtil::Str2StrMap str) {
		for (MyUtil::Str2StrMap::const_iterator it = str.begin(); it != str.end(); ++it) {
			setWorkspaceProperty(data, it->first, it->second);
		}
	}
	void setWorkspaceProperty(WorkspaceInfoPtr& data, const string& key, const string& value) {
		if (!strcasecmp(key.c_str(), WorkColID.c_str())) {
			data->id = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), WorkColUSERID.c_str())) {
			data->userId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), WorkColWORKPLACEID.c_str())) {
			data->workplaceId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), WorkColWORKPLACENAME.c_str())) {
			data->workplaceName = value;
		} else if (!strcasecmp(key.c_str(), WorkColADDRESS.c_str())) {
			data->address = value;
		} else if (!strcasecmp(key.c_str(), WorkColDESCRIPTION.c_str())) {
			data->description = value;
		} else if (!strcasecmp(key.c_str(), WorkColPROVINCE.c_str())) {
			data->province = value;
		} else if (!strcasecmp(key.c_str(), WorkColCITYID.c_str())) {
			data->cityId = boost::lexical_cast<int>(value);
		} else if (!strcasecmp(key.c_str(), WorkColCITYNAME.c_str())) {
			data->cityName = value;
		} else if (!strcasecmp(key.c_str(), WorkColJOINYEAR.c_str())) {
			data->joinYear = boost::lexical_cast<int>(value);
		} else if (!strcasecmp(key.c_str(), WorkColJOINMONTH.c_str())) {
			data->joinMonth = boost::lexical_cast<int>(value);
		} else if (!strcasecmp(key.c_str(), WorkColQUITYEAR.c_str())) {
			data->quitYear = boost::lexical_cast<int>(value);
		} else if (!strcasecmp(key.c_str(), WorkColQUITMONTH.c_str())) {
			data->quitMonth = boost::lexical_cast<int>(value);
		} else if (!strcasecmp(key.c_str(), WorkColTYPE.c_str())) {
			data->type = boost::lexical_cast<int>(value);
		} else if (!strcasecmp(key.c_str(), WorkColJOBTITLEID.c_str())) {
			data->jobTitleId = boost::lexical_cast<int>(value);
		} else if (!strcasecmp(key.c_str(), WorkColPOSITIONID.c_str())) {
			data->positionId = boost::lexical_cast<int>(value);
		}
	}

	void setCollegeInfo(CollegeInfoPtr& data, MyUtil::Str2StrMap str) {
		for (MyUtil::Str2StrMap::const_iterator it = str.begin(); it != str.end(); ++it) {
			setCollegeProperty(data, it->first, it->second);
		}
	}
	void setCollegeProperty(CollegeInfoPtr& data, const string& key, const string& value) {
		if (!strcasecmp(key.c_str(), CollegeColID.c_str())) {
			data->id = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), CollegeColUSERID.c_str())) {
			data->userId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), CollegeColCOLLEGEID.c_str())) {
			data->collegeId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), CollegeColCOLLEGENAME.c_str())) {
			data->collegeName = value;
		} else if (!strcasecmp(key.c_str(), CollegeColDEPARTMENT.c_str())) {
			data->department = value;
		} else if (!strcasecmp(key.c_str(), CollegeColENROLLYEAR.c_str())) {
			data->enrollYear = boost::lexical_cast<Ice::Int>(value);
		}
	}

	void setElementarySchoolInfo(ElementarySchoolInfoPtr& data, MyUtil::Str2StrMap str) {
		for (MyUtil::Str2StrMap::const_iterator it = str.begin(); it != str.end(); ++it) {
			setElementarySchoolProperty(data, it->first, it->second);
		}
	}
	void setElementarySchoolProperty(ElementarySchoolInfoPtr& data, const string& key, const string& value) {
		if (!strcasecmp(key.c_str(), ElemColID.c_str())) {
			ElementarySchoolInfoNPtr::dynamicCast(data)->id = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), ElemColUSERID.c_str())) {
			data->userId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), ElemColELEMENTARYSCHOOLID.c_str())) {
			data->elementarySchoolId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), ElemColELEMENTARYSCHOOLNAME.c_str())) {
			data->elementarySchoolName = value;
		} else if (!strcasecmp(key.c_str(), ElemColELEMENTARYSCHOOLYEAR.c_str())) {
			data->elementarySchoolYear = boost::lexical_cast<Ice::Int>(value);
		}
	}

	void setJuniorHighSchoolInfo(JuniorHighSchoolInfoPtr& data, MyUtil::Str2StrMap str) {
		for (MyUtil::Str2StrMap::const_iterator it = str.begin(); it != str.end(); ++it) {
			setJuniorHighSchoolProperty(data, it->first, it->second);
		}
	}
	void setJuniorHighSchoolProperty(JuniorHighSchoolInfoPtr& data, const string& key, const string& value) {
		if (!strcasecmp(key.c_str(), JunHighColID.c_str())) {
			JuniorHighSchoolInfoNPtr::dynamicCast(data)->id = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), JunHighColUSERID.c_str())) {
			data->userId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), JunHighColJUNIORHIGHSCHOOLID.c_str())) {
			data->juniorHighSchoolId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), JunHighColJUNIORHIGHSCHOOLNAME.c_str())) {
			data->juniorHighSchoolName = value;
		} else if (!strcasecmp(key.c_str(), JunHighColJUNIORHIGHSCHOOLYEAR.c_str())) {
			data->juniorHighSchoolYear = boost::lexical_cast<Ice::Int>(value);
		}
	}

	void setRegionInfo(RegionInfoPtr& data, MyUtil::Str2StrMap str) {
		for (MyUtil::Str2StrMap::const_iterator it = str.begin(); it != str.end(); ++it) {
			setRegionProperty(data, it->first, it->second);
		}
	}
	void setRegionProperty(RegionInfoPtr& data, const string& key, const string& value) {
		if (!strcasecmp(key.c_str(), RegionColID.c_str())) {
			data->id = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), RegionColUSERID.c_str())) {
			data->userId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), RegionColREGIONID.c_str())) {
			data->regionId = boost::lexical_cast<Ice::Int>(value);
		} else if (!strcasecmp(key.c_str(), RegionColPROVINCENAME.c_str())) {
			data->provinceName = value;
		} else if (!strcasecmp(key.c_str(), RegionColCITYNAME.c_str())) {
			data->cityName = value;
		}
	}
private:

	::xce::binstring::BinStringPtr _bp;
	Ice::Byte _loadFlag;
	IceUtil::RWRecMutex _mutex;
};

typedef IceUtil::Handle<NetworkCacheData> NetworkCacheDataPtr;

}
;
}
;
}
;
}
;
#endif
