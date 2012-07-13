#ifndef USERADSCACHEADAPTER_H_
#define USERADSCACHEADAPTER_H_

#include <UserAdsCache.h>
#include <AdapterI.h>
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace useradscache {

class UserAdsCacheSchool : virtual public Ice::Object{
private:
        int id;
	std::string name;
        SchoolType type;
        int enrollyear;
        std::string department;
        std::string dorm;
public:
	UserAdsCacheSchool(SchoolInfoPtr school);
	int getId();
	std::string getName();
	SchoolType getType();
	int getEnrollYear();
	std::string getDepartment();
	std::string getDorm();
};

typedef IceUtil::Handle<UserAdsCacheSchool> UserAdsCacheSchoolPtr;

class UserAdsCacheWorkplace : virtual public Ice::Object{
private:
        std::string workplacename;
        int jobtitleid;
        int positionid;
public:
	UserAdsCacheWorkplace(WorkplaceInfoPtr workplace);
	std::string getWorkplaceName();
	int getJobTitleId();
	int getPositionId();
};

typedef IceUtil::Handle<UserAdsCacheWorkplace> UserAdsCacheWorkplacePtr;

class UserAdsCacheNetwork : virtual public Ice::Object{
private:
        int networkid;
        int stage;
        int status;
public:
	UserAdsCacheNetwork(NetworkInfoPtr network);
	int getNetworkId();
	int getStage();
	int getStatus();
};

typedef IceUtil::Handle<UserAdsCacheNetwork> UserAdsCacheNetworkPtr;

class UserAdsCache : virtual public Ice::Object{
private:
	int id;
	int stage;
	std::string sex;
	int birthyear;
	int birthmonth;
        int birthday;
        std::string homeprovince;
        std::string homecity;
        std::string seatprovince;
        std::string seatcity;
        std::string tinyurl;
	std::vector<UserAdsCacheWorkplacePtr> workplaces;
	std::vector<UserAdsCacheNetworkPtr> networks;
	std::vector<UserAdsCacheSchoolPtr> schools;
public:
	UserAdsCache(UserAdsCacheUrlDataPtr data);	
	int getId();
	int getStage();
	std::string getSex();
	int getBirthMonth();
	int getBirthYear();
	int getBirthDay();
	std::string getHomeProvince();
	std::string getHomeCity();
	std::string getSeatProvince();
	std::string getSeatCity();
	std::string getTinyUrl();
	std::vector<UserAdsCacheWorkplacePtr> getWorkplaces();
	std::vector<UserAdsCacheNetworkPtr> getNetworks();
	std::vector<UserAdsCacheSchoolPtr> getSchools();
};

typedef IceUtil::Handle<UserAdsCache> UserAdsCachePtr;

class UserAdsCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<UserAdsCacheManagerPrx>,
        public MyUtil::Singleton<UserAdsCacheAdapter> {
public:
	UserAdsCacheAdapter() : MyUtil::ReplicatedClusterAdapterI <UserAdsCacheManagerPrx> ("ControllerUserAdsCache", 120, 300){
	}
	virtual ~UserAdsCacheAdapter() {
	}
	UserAdsCachePtr getUserAdsCache(int userId);
	std::map<int,UserAdsCachePtr> getUserAdsCacheList(MyUtil::IntSeq userIds);
};
}
}

#endif
