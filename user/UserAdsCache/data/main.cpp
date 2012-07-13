#include <iostream>
#include <ServiceI.h>
#include <UserAdsCacheAdapter.h>
#include <boost/lexical_cast.hpp>

/*using namespace std;
using namespace MyUtil;
using namespace ::xce::useradscache;
using namespace ::xce::usercache;
*/
int main(int argc, char *argv[])
{
	
	std::cout<< "fuck " << xce::useradscache::UserAdsCacheAdapter::instance().getUserAdsCache(boost::lexical_cast<int>(argv[1]))->getStage()<<std::endl;
	std::cout<< "fuck " << xce::useradscache::UserAdsCacheAdapter::instance().getUserAdsCache(boost::lexical_cast<int>(argv[1]))->getTinyUrl()<<std::endl;
	std::cout<< "fuck " << xce::useradscache::UserAdsCacheAdapter::instance().getUserAdsCache(boost::lexical_cast<int>(argv[1]))->getSex()<<std::endl;
	return 0;
/*	vector<int> ids;
	ids.push_back(24496255);
	ids.push_back(68126);
	Str2StrMapSeq m = UserCacheAdapter::instance().getUserCacheList(ids);
	int count = m.size();
	cout << "size: " << count << endl;
	for(Str2StrMapSeq::iterator i=m.begin();i!=m.end();++i){
		cout << "sub size: " << i->size() << endl;
		for(Str2StrMap::iterator it = i->begin(); it != i->end(); ++it)
		{
			cout << it->first << ": " << it->second << endl;
		}
	}
*/
/*
	int count = BuddyCacheAdapter::instance().getFriendCount(68126);
	cout << "count: " << count << endl;
	IntSeq ids = BuddyCacheAdapter::instance().getFriendList(68126);
	cout << "size: " << ids.size() << endl;
	for (IntSeq::iterator it = ids.begin(); it != ids.end(); ++it)
	{
		cout << *it << endl;
	}
*/
/*	UserAdsCachePtr data;
	try{
		data=UserAdsCacheAdapter::instance().getUserAdsCache(68126);
		if (data==0){
			return -1;
		}
		cout << "id: "<<data->getId() << endl;
		cout << "stage: "<<data->getStage() << endl;
		cout << "sex: "<<data->getSex() << endl;
		cout << "birthmonth: "<<data->getBirthMonth() << endl;
		cout << "birthyear: "<<data->getBirthYear() << endl;
		cout << "birthday: "<<data->getBirthDay() << endl;
		cout << "homeprovince: "<<data->getHomeProvince() << endl;
		cout << "homecity: "<<data->getHomeCity() << endl;
		cout << "seatprovince: "<<data->getSeatProvince() <<endl;
		cout << "seatcity: "<<data->getSeatCity() << endl <<endl;
		vector<UserAdsCacheWorkplacePtr> workplaces=data->getWorkplaces();
		cout << "workplace size: "<<workplaces.size() <<endl;
		for(vector<UserAdsCacheWorkplacePtr>::iterator i=workplaces.begin();i!=workplaces.end();++i){
			cout << "workplacename: "<<(*i)->getWorkplaceName()<<endl;
			cout << "jobtitleid: "<<(*i)->getJobTitleId()<<endl;
			cout << "positionid: "<<(*i)->getPositionId()<<endl;
		}
		vector<UserAdsCacheNetworkPtr> networks = data->getNetworks();
		cout << "network size: "<<networks.size()<<endl;
		for(vector<UserAdsCacheNetworkPtr>::iterator j=networks.begin();j!=networks.end();++j){
			cout << "networkid: "<<(*j)->getNetworkId()<<endl;
			cout << "stage: "<<(*j)->getStage()<<endl;
			cout << "status: "<<(*j)->getStatus()<<endl;
		}
		vector<UserAdsCacheSchoolPtr> schools=data->getSchools();
		cout << "school size: "<<schools.size()<<endl;
		for (vector<UserAdsCacheSchoolPtr>::iterator k=schools.begin();k!=schools.end();++k){
			cout << "id: "<<(*k)->getId()<<endl;
			cout << "name: "<<(*k)->getName()<<endl;
			cout << "type: "<<(*k)->getType()<<endl;
			cout << "enrollyear: "<<(*k)->getEnrollYear()<<endl;
			cout << "department: "<<(*k)->getDepartment()<<endl;
			cout << "dorm: "<<(*k)->getDorm()<<endl;
		}
	}
	catch(Ice::Exception &e){
		cout << e << " " << e.what() << endl;
	}

*/
	return 0;
}

