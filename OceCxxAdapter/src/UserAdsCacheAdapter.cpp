#include <UserAdsCacheAdapter.h>

using namespace ::xce::useradscache;
using namespace ::xce::clusterstate;
using namespace MyUtil;
using namespace std;

UserAdsCacheSchool::UserAdsCacheSchool(SchoolInfoPtr school) {
	id=school->id;
	type=school->type;
	name=school->name;
	enrollyear=school->enrollyear;
	department=school->department;
	dorm=school->dorm;
}

string UserAdsCacheSchool::getName(){
	return name;
}

SchoolType UserAdsCacheSchool::getType(){
	return type;
}

int UserAdsCacheSchool::getId(){
	return id;
}

int UserAdsCacheSchool::getEnrollYear(){
	return enrollyear;
}

string UserAdsCacheSchool::getDepartment(){
	return department;
}

string UserAdsCacheSchool::getDorm(){
	return dorm;
}

UserAdsCacheWorkplace::UserAdsCacheWorkplace(WorkplaceInfoPtr workplace) {
	workplacename=workplace->workplacename;
	jobtitleid=workplace->jobtitleid;
	positionid=workplace->positionid;
}

string UserAdsCacheWorkplace::getWorkplaceName() {
	return workplacename;
}

int UserAdsCacheWorkplace::getJobTitleId() {
	return jobtitleid;
}

int UserAdsCacheWorkplace::getPositionId() {
	return positionid;
}

UserAdsCacheNetwork::UserAdsCacheNetwork(NetworkInfoPtr network) {
	networkid=network->networkid;
	stage=network->stage;
	status=network->status;
}

int UserAdsCacheNetwork::getNetworkId() {
	return networkid;
}

int UserAdsCacheNetwork::getStage() {
	return stage;
}

int UserAdsCacheNetwork::getStatus() {
	return status;
}

UserAdsCache::UserAdsCache(UserAdsCacheUrlDataPtr data) {
	id=data->id;
	stage=data->stage;
	sex=data->sex;
	birthyear=data->birthyear;
	birthmonth=data->birthmonth;
	birthday=data->birthday;
	homeprovince=data->homeprovince;
	homecity=data->homecity;
	seatprovince=data->seatprovince;
	seatcity=data->seatcity;
	tinyurl=data->tinyurl;
	for(vector<SchoolInfoPtr>::iterator i1=data->schools.begin(); i1!=data->schools.end();++i1) {
		schools.push_back(new UserAdsCacheSchool(*i1));
	}
	for(vector<WorkplaceInfoPtr>::iterator i2=data->workplaces.begin(); i2!=data->workplaces.end();++i2) {
		workplaces.push_back(new UserAdsCacheWorkplace(*i2));
	}
	for(vector<NetworkInfoPtr>::iterator i3=data->networks.begin(); i3!=data->networks.end(); ++i3) {
		networks.push_back(new UserAdsCacheNetwork(*i3));
	}
}

int UserAdsCache::getId(){
	return id;
}

int UserAdsCache::getStage() {
	return stage;
}

string UserAdsCache::getSex() {
	return sex;
}

int UserAdsCache::getBirthYear() {
	return birthyear;
}

int UserAdsCache::getBirthMonth() {
	return birthmonth;
}

int UserAdsCache::getBirthDay() {
	return birthday;
}

string UserAdsCache::getHomeProvince() {
	return homeprovince;
}

string UserAdsCache::getHomeCity() {
	return homecity;
}

string UserAdsCache::getSeatProvince() {
	return seatprovince;
}

string UserAdsCache::getSeatCity() {
	return seatcity;
}

string UserAdsCache::getTinyUrl() {
	return tinyurl;
}

vector<UserAdsCacheWorkplacePtr> UserAdsCache::getWorkplaces() {
	return workplaces;
}

vector<UserAdsCacheNetworkPtr> UserAdsCache::getNetworks() {
	return networks;
}

vector<UserAdsCacheSchoolPtr> UserAdsCache::getSchools() {
	return schools;
}

UserAdsCachePtr UserAdsCacheAdapter::getUserAdsCache(int userId) {
	try {
			UserAdsCacheManagerPrx prx=getProxy(userId);
			UserAdsCacheUrlDataPtr data=prx->getUserAdsCacheUrl(userId);
			return new UserAdsCache(data);
        } catch (Ice::TimeoutException &e) {
                cerr << "UserAdsCacheAdapter::getUserAdsCache TimeoutException " << e << endl;;
        } catch (Ice::Exception &e) {
                cerr << "UserAdsCacheAdapter::getUserAdsCache Ice::Exception " << e << endl;;
        } catch (...) {
                cerr << "UserAdsCacheAdapter::getUserAdsCache exception" << endl;;
        }
	return NULL;
}

map<int,UserAdsCachePtr> UserAdsCacheAdapter::getUserAdsCacheList(MyUtil::IntSeq userIds) {
	if (userIds.empty()) {
		return map<int,UserAdsCachePtr>();
	}
	size_t numCluster = getCluster();
	if (numCluster == 0 || numCluster == 1) {
                try {
						map<int,UserAdsCachePtr> result;
                        Int2UserAdsCacheUrlDataMap someResult=getProxy(userIds.at(0))->getUserAdsCacheUrlList(userIds);
						for (Int2UserAdsCacheUrlDataMap::iterator it=someResult.begin();it!=someResult.end();++it) {
							result[it->first]=new UserAdsCache(it->second);
						}
						return result;
                } catch (Ice::TimeoutException &e) {
					    cerr << "UserAdsCacheAdapter::getUserAdsCache TimeoutException " << e << endl;;
                        return map<int,UserAdsCachePtr>();
                }
        } else {
                vector<vector<int> > seperatedIds;
                seperatedIds.resize(numCluster);
                for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it
                                != userIds.end(); ++it) {
                        seperatedIds.at(*it % numCluster).push_back(*it);
                }
                try {
						map<int,UserAdsCachePtr> result;
//                        MyUtil::Str2StrMapSeq result;
                        for (size_t posCluster = 0; posCluster < numCluster; ++posCluster) {
                                Int2UserAdsCacheUrlDataMap someResult = getProxy(posCluster)
                                        ->getUserAdsCacheUrlList(seperatedIds.at(posCluster));
							for (Int2UserAdsCacheUrlDataMap::iterator it = someResult.begin(); it != someResult.end(); ++it) {
								if( it->second )
								{
									result[it->first]=new UserAdsCache(it->second);
								}
							}
                        }
                        return result;
                } catch (Ice::TimeoutException &e) {
					    cerr << "UserAdsCacheAdapter::getUserAdsCache TimeoutException " << e << endl;;
                        return map<int,UserAdsCachePtr>();
                }
        }
}

