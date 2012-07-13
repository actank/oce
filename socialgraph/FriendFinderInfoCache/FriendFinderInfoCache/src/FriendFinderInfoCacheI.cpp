#include "ServiceI.h"
#include "FriendFinderInfoCacheI.h"
#include <BuddyByIdReplicaAdapter.h>
#include <FriendFinderByCommonReplicaAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace ::MyUtil;
using namespace ::xce::friendfinder;
using namespace ::xce::friendfinder::adapter;
using namespace ::xce::buddy::adapter;
using namespace ::xce::md5;
using namespace xce::serverstate;
using namespace xce::socialgraph;

void MyUtil::initialize() {
    ServiceI& service = ServiceI::instance();

    service.registerObjectCache(WORKPLACE_DATA, "WDC", new AbstractFactory);
    service.registerObjectCache(UNIV_DATA, "UDC", new AbstractFactory);
    service.registerObjectCache(HIGHSCHOOL_DATA, "HDC", new AbstractFactory);
    service.registerObjectCache(JUNIORSCHOOL_DATA, "JDC", new AbstractFactory);
    service.registerObjectCache(ELEMENTARYSCHOOL_DATA, "EDC", new AbstractFactory);

	service.registerObjectCache(NETWORK_DATA, "NWD", new NetworkDataFactory);
	service.registerObjectCache(BIRTHDAY_DATA, "BD", new AbstractFactory);
	service.registerObjectCache(BIRTHDAY_DATA_BACKEND, "BDB", new AbstractFactory);
	service.registerObjectCache(SCHOOL_AREA, "SA", new SchoolAreaFactory);

    service.getAdapter()->add(&FriendFinderInfoCacheI::instance(), service.createIdentity("FFIC", ""));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendFinderInfoCacheReplica.Mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendFinderInfoCacheReplica.Interval", 5);
	// 向Controller注册
	ServerStateSubscriber::instance().initialize("ControllerFriendFinderInfoCache", &FriendFinderInfoCacheI::instance(), mod, sInterval);              
}

//*************************************************************************************************

NetworkInfo FriendFinderInfoCacheI::getNetworkInfo(::Ice::Int userId, const Ice::Current& current) {
	struct timeval tvStart,tvEnd;
    double linStart = 0,linEnd = 0,lTime = 0;
    gettimeofday(&tvStart, NULL);

	NetworkInfo result;

	getUnivItemSeq(userId, result.univInfo); 
	getHighschoolItemSeq(userId, result.highschoolInfo); 
	getJuniorschoolItemSeq(userId, result.juniorschoolInfo); 
	getElementaryschoolItemSeq(userId, result.elementaryschoolInfo);

    gettimeofday(&tvEnd, NULL);
    linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);  //unit S
    linEnd = ((double)tvEnd.tv_sec * 1000000 + (double)tvEnd.tv_usec);        //unit S
    lTime = linEnd - linStart;
    lTime = lTime/1000000;

	MCE_INFO("[FriendFinderInfoCacheI] getNetworkInfo userId(" << userId << ") run time : " << lTime << " second!" );

	return result;
}

NetworkInfo FriendFinderInfoCacheI::getNetworkInfoWithHostage(::Ice::Int userId, const Ice::Current& current) {
	struct timeval tvStart,tvEnd;
    double linStart = 0,linEnd = 0,lTime = 0;
    gettimeofday(&tvStart, NULL);

	NetworkInfo result;

	getUnivItemSeqWithHostage(userId, result.univInfo);
	getHighschoolItemSeq(userId, result.highschoolInfo);
	getJuniorschoolItemSeq(userId, result.juniorschoolInfo);
	getElementaryschoolItemSeq(userId, result.elementaryschoolInfo);

    gettimeofday(&tvEnd, NULL);
    linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);  //unit S
    linEnd = ((double)tvEnd.tv_sec * 1000000 + (double)tvEnd.tv_usec);        //unit S
    lTime = linEnd - linStart;
    lTime = lTime/1000000;

	MCE_INFO("[FriendFinderInfoCacheI] getNetworkInfoWithHostage userId(" << userId << ") " << 
			current.con->toString().substr(current.con->toString().find("remote",25)) << " run time : " << lTime << " second!");

	return result;
}

NetworkInfo FriendFinderInfoCacheI::getNetworkInfoWithHostageFromDB(::Ice::Int userId, const Ice::Current& current) {
	MCE_INFO("[FriendFinderInfoCacheI] getNetworkInfoWithHostageFromDB userId(" << userId << ")");
	NetworkDataPtr obj = ServiceI::instance().findObject<NetworkDataPtr>(NETWORK_DATA, userId);
	if (obj) {
		ServiceI::instance().removeObject(NETWORK_DATA, userId);	
		//MCE_DEBUG("[FriendFinderInfoCacheI] getNetworkInfoWithHostageFromDB has remove id(" << userId << ")");
	}
	return getNetworkInfoWithHostage(userId, current);
}

HighSchoolInfo FriendFinderInfoCacheI::getHighSchoolInfo(::Ice::Int userId, const Ice::Current& current) {
	MCE_INFO("[FriendFinderInfoCacheI] getHighSchoolInfo userId(" << userId << ")");
	HighSchoolInfo result;
	SchoolAreaDataPtr obj = ServiceI::instance().locateObject<SchoolAreaDataPtr>(SCHOOL_AREA, userId);
	if (!obj) {
		return result;
	}
	Md5Info highschool = obj->GetHighschoolInfo();
	Md5Info juniorschool = obj->GetJuniorschoolInfo();
	Md5Info college = obj->GetCollegeInfo();	
	MCE_INFO(highschool.condition.size() << ":" << highschool.md5 << ";" << juniorschool.condition.size() 
			<< ":" << juniorschool.md5 << ";" << college.condition.size() << ":" << college.md5);

	std::vector<PItem> highschool_list = SameAreaPeerAdapter::instance().getAreaPeer(highschool.md5, EACH_ITEM_SIZE);
	std::vector<PItem> juniorschool_list = SameAreaPeerAdapter::instance().getAreaPeer(juniorschool.md5, EACH_ITEM_SIZE);
	std::vector<PItem> college_list = SameAreaPeerAdapter::instance().getAreaPeer(college.md5, EACH_ITEM_SIZE);

	MCE_INFO(highschool_list.size() << " : " << juniorschool_list.size() << " : " << college_list.size());

	Transfer(highschool_list, result.highschoolseq);
	Transfer(juniorschool_list, result.juniorschoolseq);
	Transfer(college_list, result.collegeseq);
	
	return result;
}

MyUtil::IntSeq FriendFinderInfoCacheI::getWorkplaceData(const MyUtil::StrSeq& condition, const Ice::Current& current) {
	long md5 = MD5Util::stringSeqToMD5(condition);
	MCE_INFO("[FriendFinderInfoCacheI] getWorkplaceData(" << md5 << ") " << current.con->toString().substr(current.con->toString().find("remote",25)));
	InfoDataPtr obj = ServiceI::instance().findObject<InfoDataPtr>(WORKPLACE_DATA, md5);
	if(obj) {
		//MCE_DEBUG("[FriendFinderInfoCacheI] WorkplaceData obj->idSeq size : " << obj->idSeq.size());
		return obj->idSeq;
	}

	MyUtil::IntSeq tmp;
	return tmp; 
}

MyUtil::IntSeq FriendFinderInfoCacheI::getUnivData(const MyUtil::StrSeq& condition, const Ice::Current& current) {
	long md5 = MD5Util::stringSeqToMD5(condition);
	MCE_INFO("[FriendFinderInfoCacheI] getUnivData(" << md5 << ") " << current.con->toString().substr(current.con->toString().find("remote",25)));
	InfoDataPtr obj = ServiceI::instance().findObject<InfoDataPtr>(UNIV_DATA, md5);
	if(obj) {
		//MCE_DEBUG("[FriendFinderInfoCacheI] UnivData obj->idSeq size : " << obj->idSeq.size());
		return obj->idSeq;
	}

	MyUtil::IntSeq tmp;
	return tmp; 
}

MyUtil::IntSeq FriendFinderInfoCacheI::getHighschoolData(const MyUtil::StrSeq& condition, const Ice::Current& current) {
	long md5 = MD5Util::stringSeqToMD5(condition);
	MCE_INFO("[FriendFinderInfoCacheI] getHighschoolData(" << md5 << ") " << current.con->toString().substr(current.con->toString().find("remote",25)));
	InfoDataPtr obj = ServiceI::instance().findObject<InfoDataPtr>(HIGHSCHOOL_DATA, md5);
	if(obj) {
		//MCE_DEBUG("[FriendFinderInfoCacheI] HighschoolData obj->idSeq size : " << obj->idSeq.size());
		return obj->idSeq;
	}

	MyUtil::IntSeq tmp;
	return tmp; 
}

MyUtil::IntSeq FriendFinderInfoCacheI::getJuniorschoolData(const MyUtil::StrSeq& condition, const Ice::Current& current) {
	long md5 = MD5Util::stringSeqToMD5(condition);
	MCE_INFO("[FriendFinderInfoCacheI] getJuniorschoolData(" << md5 << ") " << current.con->toString().substr(current.con->toString().find("remote",25)));
	InfoDataPtr obj = ServiceI::instance().findObject<InfoDataPtr>(JUNIORSCHOOL_DATA, md5);
	if(obj) {
		//MCE_DEBUG("[FriendFinderInfoCacheI] JuniorschoolData obj->idSeq size : " << obj->idSeq.size());
		return obj->idSeq;
	}

	MyUtil::IntSeq tmp;
	return tmp; 
}

MyUtil::IntSeq FriendFinderInfoCacheI::getElementaryschoolData(const MyUtil::StrSeq& condition, const Ice::Current& current) {
	long md5 = MD5Util::stringSeqToMD5(condition);
	MCE_INFO("[FriendFinderInfoCacheI] getElementaryschoolData(" << md5 << ") " << current.con->toString().substr(current.con->toString().find("remote",25)));
	InfoDataPtr obj = ServiceI::instance().findObject<InfoDataPtr>(ELEMENTARYSCHOOL_DATA, md5);
	if(obj) {
		//MCE_DEBUG("[FriendFinderInfoCacheI] ElementaryschoolData obj->idSeq size : " << obj->idSeq.size());
		return obj->idSeq;
	}

	MyUtil::IntSeq tmp;
	return tmp; 
}

int FriendFinderInfoCacheI::getBirthdayData(::Ice::Int userId, const Ice::Current& current) {
	MCE_INFO("[FriendFinderInfoCacheI] getBirthdayData userId(" << userId << ") " << current.con->toString().substr(current.con->toString().find("remote",25)));
	BirthDataPtr obj = ServiceI::instance().findObject<BirthDataPtr>(BIRTHDAY_DATA, userId);

	if(obj) {
		return obj->birthday;
	}
	return 0;
}

void FriendFinderInfoCacheI::setWorkplaceDataWithMask(const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq, const Ice::Current& current) {
	for(LongSeq::const_iterator iter=md5Seq.begin(); iter!=md5Seq.end(); ++iter) {
		int pos = iter - md5Seq.begin();
		vector<int>( dataSeq[pos]->idSeq ).swap( dataSeq[pos]->idSeq );
		ServiceI::instance().addObject(dataSeq[pos], WORKPLACE_DATA, *iter);
	}
	MCE_INFO( "[FriendFinderInfoCacheI] set WorkplaceCache size : " 
		<< ServiceI::instance().getObjectCache()->locateCategory(WORKPLACE_DATA)->size() );
}

void FriendFinderInfoCacheI::setUnivDataWithMask(const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq, const Ice::Current& current) {
	for(LongSeq::const_iterator iter=md5Seq.begin(); iter!=md5Seq.end(); ++iter) {
		int pos = iter - md5Seq.begin();
		vector<int>( dataSeq[pos]->idSeq ).swap( dataSeq[pos]->idSeq );
		ServiceI::instance().addObject(dataSeq[pos], UNIV_DATA, *iter);
	}
	MCE_INFO( "[FriendFinderInfoCacheI] set UnivCache size : " 
		<< ServiceI::instance().getObjectCache()->locateCategory(UNIV_DATA)->size() );
}

void FriendFinderInfoCacheI::setHighschoolDataWithMask(const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq, const Ice::Current& current) {
	for(LongSeq::const_iterator iter=md5Seq.begin(); iter!=md5Seq.end(); ++iter) {
		int pos = iter - md5Seq.begin();
		vector<int>( dataSeq[pos]->idSeq ).swap( dataSeq[pos]->idSeq );
		ServiceI::instance().addObject(dataSeq[pos], HIGHSCHOOL_DATA, *iter);
	}
	MCE_INFO( "[FriendFinderInfoCacheI] set HighschoolCache size : " 
		<< ServiceI::instance().getObjectCache()->locateCategory(HIGHSCHOOL_DATA)->size() );
}

void FriendFinderInfoCacheI::setJuniorschoolDataWithMask(const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq, const Ice::Current& current) {
	for(LongSeq::const_iterator iter=md5Seq.begin(); iter!=md5Seq.end(); ++iter) {
		int pos = iter - md5Seq.begin();
		vector<int>( dataSeq[pos]->idSeq ).swap( dataSeq[pos]->idSeq );
		ServiceI::instance().addObject(dataSeq[pos], JUNIORSCHOOL_DATA, *iter);
	}
	MCE_INFO( "[FriendFinderInfoCacheI] set JuniorschoolCache size : " 
		<< ServiceI::instance().getObjectCache()->locateCategory(JUNIORSCHOOL_DATA)->size() );
}

void FriendFinderInfoCacheI::setElementaryschoolDataWithMask(const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq, const Ice::Current& current) {
	for(LongSeq::const_iterator iter=md5Seq.begin(); iter!=md5Seq.end(); ++iter) {
		int pos = iter - md5Seq.begin();
		vector<int>( dataSeq[pos]->idSeq ).swap( dataSeq[pos]->idSeq );
		ServiceI::instance().addObject(dataSeq[pos], ELEMENTARYSCHOOL_DATA, *iter);
	}
	MCE_INFO( "[FriendFinderInfoCacheI] set ElementaryschoolCache size : " 
		<< ServiceI::instance().getObjectCache()->locateCategory(ELEMENTARYSCHOOL_DATA)->size() );

}

void FriendFinderInfoCacheI::setBirthdayData(::Ice::Int birthYear, const MyUtil::IntSeq& orderedIds, const Ice::Current& current) {
	//MCE_INFO("[FriendFinderInfoCacheI] setBirthdayData birthYear(" << birthYear << ") orderedIds size(" << orderedIds.size() << ")");
	BirthdayInfoDataPtr obj = ServiceI::instance().findObject<BirthdayInfoDataPtr>(BIRTHDAY_DATA_BACKEND, BIRTHDAY_INDEX_BACKEND);

	if (birthYear == 0 && orderedIds.empty() && obj) {          //判断加载结束条件
		//MCE_INFO("[FriendFinderInfoCacheI] setBirthdayData set new BirthdayData");
		ServiceI::instance().addObject(obj, BIRTHDAY_DATA, BIRTHDAY_INDEX);  //更新使用的cache

		ServiceI::instance().removeObject(BIRTHDAY_DATA_BACKEND, BIRTHDAY_INDEX_BACKEND);
		return;
	}

	if( !obj ) {
		//MCE_INFO("[FriendFinderInfoCacheI] setBirthdayData no such object!");
		obj = new BirthdayInfoData();
	}
	obj->set(birthYear, orderedIds);
	ServiceI::instance().addObject(obj, BIRTHDAY_DATA_BACKEND, BIRTHDAY_INDEX_BACKEND);

	MCE_INFO("[FriendFinderInfoCacheI] set BirthdayData cache size : "
		<< ServiceI::instance().getObjectCache()->locateCategory(BIRTHDAY_DATA_BACKEND)->size());
}

void FriendFinderInfoCacheI::rebuild(::Ice::Int userId, const Ice::Current& current) {
	MCE_INFO("[FriendFinderInfoCacheI] rebuild userId(" << userId << ")");
	TaskManager::instance().execute(new RebuildNetworkTask(userId));
}

void FriendFinderInfoCacheI::getUnivItemSeq(int userId, ItemSeq& items) {
	//MCE_INFO("[FriendFinderInfoCacheI] getUnivItemSeq userId(" << userId << ")");
	InfoSeq univList = locateNetworkData(userId)->getUnivSeq();
	
	if( univList.empty() ) {
		return;
	}	

	for(InfoSeq::iterator iter=univList.begin(); iter!=univList.end(); ++iter) {
		Item univItem;
		univItem.condition = iter->condition;
		univItem.md5 = iter->md5;

		ostringstream nn;
		for (StrSeq::iterator it = univItem.condition.begin();
				it != univItem.condition.end(); ++it) {
			nn << *it << "  ";
		}
		//MCE_INFO(nn.str() << " md5=" << univItem.md5);

		InfoDataPtr obj = ServiceI::instance().findObject<InfoDataPtr>(UNIV_DATA, univItem.md5);
		if(obj) {
			//MCE_DEBUG("[FriendFinderInfoCacheI] UnivData obj->idSeq size : " << obj->idSeq.size());
			univItem.idSeq = obj->idSeq;
		} else {
			//MCE_DEBUG("[FriendFinderInfoCacheI] no UnivData");	
			univItem.idSeq = MyUtil::IntSeq();
		}

		items.push_back(univItem);
	}
}

void FriendFinderInfoCacheI::getUnivItemSeqWithHostage(int userId, ItemSeq& items) {
	InfoSeq univList = locateNetworkData(userId)->getUnivSeq();

	BirthdayInfoDataPtr birthdayObj = ServiceI::instance().findObject<BirthdayInfoDataPtr>(BIRTHDAY_DATA, BIRTHDAY_INDEX);
	if (!birthdayObj) {
		//MCE_INFO("no birthday data");
		birthdayObj = new BirthdayInfoData();
	}	

	int userBirthyear = birthdayObj->getUserBirthyear(userId);

	//MCE_INFO("[FriendFinderInfoCacheI] getUnivItemSeqWithHostage userId(" << userId << 
			//") userbirth(" << userBirthyear << ") univList size(" << univList.size() << ")");

	if ( univList.empty() ) {
		return;
	}	
		
	for (InfoSeq::iterator iter=univList.begin(); iter!=univList.end(); ++iter) {
		Item univItem;
		univItem.condition = iter->condition;
		univItem.md5 = iter->md5;
	
		ostringstream nn;
		for (StrSeq::iterator it = univItem.condition.begin();
				it != univItem.condition.end(); ++it) {
			nn << *it << "  ";
		}
		//MCE_INFO(nn.str() << " md5=" << univItem.md5);
	
		InfoDataPtr obj = ServiceI::instance().findObject<InfoDataPtr>(UNIV_DATA, univItem.md5);
		if (obj) {
			univItem.idSeq = birthdayObj->getBySimilarYear(userBirthyear, obj->idSeq);
			//MCE_DEBUG("[FriendFinderInfoCacheI] UnivData obj->idSeq size : " << obj->idSeq.size() << 
			//		" similiarYear UnivData idSeq size : " << univItem.idSeq.size());
		} else {
			//MCE_DEBUG("[FriendFinderInfoCacheI] no UnivData");	
			univItem.idSeq = MyUtil::IntSeq();
		}

		items.push_back(univItem);
	}
}

void FriendFinderInfoCacheI::getFriendsWithAge(int userId, MyUtil::IntSeq& resultSeq, const MyUtil::IntSeq& sourceSeq) {
	int ageThreshold = 3;
	MyUtil::IntSeq tmpSeq( sourceSeq ); 		//调用数据库策略
	tmpSeq.push_back(userId);

	Int2IntMap ageMap;
	getPeopleAgeBatchFromDB(tmpSeq, ageMap);

	int hostAge = ageMap[userId];

	MCE_INFO("[FriendFinderInfoCacheI] getFriendsWithAge userId(" << userId << ") hostAge : " << hostAge);

	if(hostAge == 0) {
		resultSeq = sourceSeq;
		return;
	}
	
	for(MyUtil::IntSeq::const_iterator iter=sourceSeq.begin(); iter!=sourceSeq.end(); ++iter) {
		int friendAge = ageMap[*iter];
		if( abs(hostAge - friendAge) < ageThreshold ) {
			resultSeq.push_back(*iter);
		}
	}

	//MCE_DEBUG("[FriendFinderInfoCacheI] resultSeq size : " << resultSeq.size());
}

void FriendFinderInfoCacheI::getHighschoolItemSeq(int userId, ItemSeq& items) {
	//MCE_INFO("[FriendFinderInfoCacheI] getHighschoolItemSeq userId(" << userId << ")");
	InfoSeq highschoolList = locateNetworkData(userId)->getHighschoolSeq();
	
	if( highschoolList.empty() ) {
		return;
	}	

	for(InfoSeq::iterator iter=highschoolList.begin(); iter!=highschoolList.end(); ++iter) {
		Item highschoolItem;
		highschoolItem.condition = iter->condition;
		highschoolItem.md5 = iter->md5;

		ostringstream nn;
		for (StrSeq::iterator it = highschoolItem.condition.begin();
				it != highschoolItem.condition.end(); ++it) {
			nn << *it << "  ";
		}
		//MCE_INFO(nn.str() << " md5=" << highschoolItem.md5);

		InfoDataPtr obj = ServiceI::instance().findObject<InfoDataPtr>(HIGHSCHOOL_DATA, highschoolItem.md5);
		if(obj) {
			//MCE_DEBUG("[FriendFinderInfoCacheI] HighschoolData obj->idSeq size : " << obj->idSeq.size());
			highschoolItem.idSeq = obj->idSeq;
		} else {
			//MCE_DEBUG("[FriendFinderInfoCacheI] no UnivData");	
			highschoolItem.idSeq = MyUtil::IntSeq();
		}

		items.push_back(highschoolItem);
	}
}

void FriendFinderInfoCacheI::getJuniorschoolItemSeq(int userId, ItemSeq& items) {
	//MCE_INFO("[FriendFinderInfoCacheI] getJuniorschoolItemSeq userId(" << userId << ")");
	InfoSeq juniorschoolList = locateNetworkData(userId)->getJuniorschoolSeq();
	
	if( juniorschoolList.empty() ) {
		return;
	}	

	for(InfoSeq::iterator iter=juniorschoolList.begin(); iter!=juniorschoolList.end(); ++iter) {
		Item juniorschoolItem;
		juniorschoolItem.condition = iter->condition;
		juniorschoolItem.md5 = iter->md5;

		//MCE_INFO("juniorschoolItem condition size : " << juniorschoolItem.condition.size());	
		ostringstream nn;
		for (StrSeq::iterator it = juniorschoolItem.condition.begin();
				it != juniorschoolItem.condition.end(); ++it) {
			nn << *it << "  ";
		}
		//MCE_INFO(nn.str() << " md5=" << juniorschoolItem.md5);
	
		InfoDataPtr obj = ServiceI::instance().findObject<InfoDataPtr>(JUNIORSCHOOL_DATA, juniorschoolItem.md5);
		if(obj) {
			//MCE_DEBUG("[FriendFinderInfoCacheI] JuniorschoolData obj->idSeq size : " << obj->idSeq.size());
			juniorschoolItem.idSeq = obj->idSeq;
		} else {
			//MCE_DEBUG("[FriendFinderInfoCacheI] no JuniorschoolData");
			juniorschoolItem.idSeq = MyUtil::IntSeq();
		}
		
		items.push_back(juniorschoolItem);	
	}
}

void FriendFinderInfoCacheI::getElementaryschoolItemSeq(int userId, ItemSeq& items) {
	//MCE_INFO("[FriendFinderInfoCacheI] getElementaryschoolItemSeq userId(" << userId << ")");
	InfoSeq elementaryschoolList = locateNetworkData(userId)->getElementaryschoolSeq();
	
	if( elementaryschoolList.empty() ) {
		return;
	}	

	for(InfoSeq::iterator iter=elementaryschoolList.begin(); iter!=elementaryschoolList.end(); ++iter) {
		Item elementaryschoolItem;
		elementaryschoolItem.condition = iter->condition;
		elementaryschoolItem.md5 = iter->md5;

		//MCE_INFO("elementaryschoolItem condition size : " << elementaryschoolItem.condition.size());	
		ostringstream nn;
		for (StrSeq::iterator it = elementaryschoolItem.condition.begin();
				it != elementaryschoolItem.condition.end(); ++it) {
			nn << *it << "  ";
		}
		//MCE_INFO(nn.str() << " md5=" << elementaryschoolItem.md5);
	
		InfoDataPtr obj = ServiceI::instance().findObject<InfoDataPtr>(ELEMENTARYSCHOOL_DATA, elementaryschoolItem.md5);
		if(obj) {
			//MCE_DEBUG("[FriendFinderInfoCacheI] ElementaryschoolData obj->idSeq size : " << obj->idSeq.size());
			elementaryschoolItem.idSeq = obj->idSeq;
		} else {
			//MCE_DEBUG("[FriendFinderInfoCacheI] no ElementaryschoolData");
			elementaryschoolItem.idSeq = MyUtil::IntSeq();
		}

		items.push_back(elementaryschoolItem);
	}
}

void FriendFinderInfoCacheI::getPeopleAge(int userId, int& userAge) {
	MCE_INFO("[FriendFinderInfoCacheI] getPeopleAge userId(" << userId << ")");
	BirthDataPtr obj = ServiceI::instance().findObject<BirthDataPtr>(BIRTHDAY_DATA, userId);
	if(obj) {
		userAge = obj->birthday;
	} else {
		//MCE_DEBUG("[FriendFinderInfoCacheI] getPeopleAge no birthday info");
		userAge = 0;
	}
}

void FriendFinderInfoCacheI::getPeopleAgeBatch(const MyUtil::IntSeq& userIds, MyUtil::Int2IntMap& userAgeMap) {
	MCE_INFO("[FriendFinderInfoCacheI] getPeopleAgeBatch");
	for(IntSeq::const_iterator iter=userIds.begin(); iter!=userIds.end(); ++iter) {
		BirthDataPtr obj = ServiceI::instance().findObject<BirthDataPtr>(BIRTHDAY_DATA, *iter);
		if(obj) {
			userAgeMap[*iter] = obj->birthday;
		} else {
			userAgeMap[*iter] = 0; 
		}
	}
}

void FriendFinderInfoCacheI::getPeopleAgeFromDB(int userId, int& userAge) {
	MCE_INFO("[FriendFinderInfoCacheI] getPeopleAgeFromDB userId(" << userId << ")");
    NetworkCollector reader;
	reader.getUserAge(userId, userAge);
}


void FriendFinderInfoCacheI::getPeopleAgeBatchFromDB(const MyUtil::IntSeq& userIds, MyUtil::Int2IntMap& userAgeMap) {
	MCE_INFO("[FriendFinderInfoCacheI] getPeopleAgeBatchFromDB");
    NetworkCollector reader;
	reader.getUserAgeBatch(userIds, userAgeMap);
}

NetworkDataPtr FriendFinderInfoCacheI::locateNetworkData(::Ice::Int id) {
	return ServiceI::instance().locateObject<NetworkDataPtr>(NETWORK_DATA, id);
}

//********************************************************************************************************

void FriendFinderInfoCacheI::calc_shares(map<int,int>& m,const MyUtil::IntSeq& s, const MyUtil::IntSeq& f){
		for (IntSeq::const_iterator iter = s.begin();
				iter != s.end(); ++iter) {
			IntSeq::const_iterator fIt = lower_bound(f.begin(), f.end(), *iter);
			if (fIt != f.end() && *iter == *fIt) {
				m[*fIt]++;
			}
		}
}

void FriendFinderInfoCacheI::calc_match( NetworkInfo& infos, const vector<int>& friends, map<int,int>& sameInfoFriends ){
        for(ItemSeq::iterator it=infos.univInfo.begin(); it!=infos.univInfo.end(); ++it) {
                calc_shares(sameInfoFriends, it->idSeq, friends);
        }

        for(ItemSeq::iterator it=infos.highschoolInfo.begin(); it!=infos.highschoolInfo.end(); ++it) {
                calc_shares(sameInfoFriends, it->idSeq, friends);
        }

        for(ItemSeq::iterator it=infos.juniorschoolInfo.begin(); it!=infos.juniorschoolInfo.end(); ++it) {
                calc_shares(sameInfoFriends, it->idSeq, friends);
        }

        for(ItemSeq::iterator it=infos.elementaryschoolInfo.begin(); it!=infos.elementaryschoolInfo.end(); ++it) {
                calc_shares(sameInfoFriends, it->idSeq, friends);
        }
}

multimap<int,int> FriendFinderInfoCacheI::cal_friend_byinfo( ::Ice::Int hostId, ::Ice::Int guestId ) {
        vector<int> friends;
        try {
                friends = BuddyByIdCacheAdapter::instance().getFriendListAsc(hostId, -1);
        } catch( Ice::Exception e ){
                MCE_WARN("[cal_friend_byinfo::getFriendListAsc] hostId=" << hostId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[cal_friend_byinfo::getFriendListAsc] hostId=" << hostId << " found unknown exception" );
        }

        NetworkInfo infos;
        try{
                infos = getNetworkInfo(guestId,Ice::Current() );
        } catch( mysqlpp::Exception e){
                MCE_WARN("[cal_friend_byinfo::getNetworkInfo] guestId=" << guestId << " found mysqlpp::Exception" << e.what() );
        } catch( std::exception e){
                MCE_WARN("[cal_friend_byinfo::getNetworkInfo] guestId=" << guestId << " found std::exception" << e.what() );
        } catch( ... ){
                MCE_WARN("[cal_friend_byinfo::getNetworkInfo] guestId=" << guestId << " found unknown exception" );
        }

        map<int,int> sameInfoFriends;
        for( vector<int>::iterator fit = friends.begin(); fit != friends.end(); ++fit ){
                sameInfoFriends.insert( make_pair<int,int>( *fit, 0 ) );
        }

	calc_match( infos, friends, sameInfoFriends );

        multimap<int,int> res;
        for(map<int,int>::iterator mit=sameInfoFriends.begin(); mit!=sameInfoFriends.end(); ++mit) {
                res.insert( make_pair<int,int>( mit->second, mit->first ) );
        }

        return res;
}

multimap<int,int> FriendFinderInfoCacheI::cal_friend_byinfo_batch( const ::MyUtil::IntSeq& hostIds, ::Ice::Int guestId ) {
	MyUtil::Int2IntSeqMap friendmap;
        try {
                friendmap = BuddyByIdCacheAdapter::instance().getFriendLists(hostIds);
        } catch( Ice::Exception e ){
                MCE_WARN("[cal_friend_byinfo::getFriendListAsc] hostIds.size()=" << hostIds.size() << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[cal_friend_byinfo::getFriendListAsc] hostIds.size()=" << hostIds.size() << " found unknown exception" );
        }

        NetworkInfo infos;
        try{
                infos = getNetworkInfo(guestId,Ice::Current() );
        } catch( ... ){
                MCE_WARN("[cal_friend_byinfo::getNetworkInfo] guestId=" << guestId << " found unknown exception" );
        }

	map<int,int> sameInfoFriends;

        for( MyUtil::Int2IntSeqMap::iterator fmit = friendmap.begin(); fmit != friendmap.end(); ++fmit ){
		for( vector<int>::iterator fit = fmit->second.begin(); fit != fmit->second.end(); ++fit ){
			if( sameInfoFriends.find( *fit ) != sameInfoFriends.end() ){
                                sameInfoFriends[ *fit ]++;
			} else{
				sameInfoFriends.insert( make_pair<int,int>( *fit, 0 ) );
			}
		}
		calc_match( infos, fmit->second, sameInfoFriends );
        }
	
        multimap<int,int> res;
        for(map<int,int>::iterator mit=sameInfoFriends.begin(); mit!=sameInfoFriends.end(); ++mit) {
                res.insert( make_pair<int,int>( mit->second, mit->first ) );
        }

        return res;
}

multimap<int,int> FriendFinderInfoCacheI::cal_common_friend_byinfo( ::Ice::Int userId, ::Ice::Int minsharecount ) {
        CommonFriendItemSeq common_friends;

        try {
                common_friends = FriendFinderByCommonAdapter::instance().getCommonFriendItemSeqById( userId, -1 );
        } catch( Ice::Exception e ){
                MCE_WARN("[cal_common_friend_byinfo::getCommonFriendItemSeq] userId=" << userId << " found Ice::Exception : " << e.what() );
        } catch( ... ){
                MCE_WARN("[cal_common_friend_byinfo::getCommonFriendItemSeq] userId=" << userId << " found unknown exception" );
        }

        NetworkInfo infos;
        try{
                infos = getNetworkInfo( userId, Ice::Current() );
        } catch( ... ){
                MCE_WARN("[cal_common_friend_byinfo::getNetworkInfo] userId=" << userId << " found unknown exception" );
        }

        vector<int> friends;
        map<int,int> sameInfoFriends;
        for( CommonFriendItemSeq::iterator cfit = common_friends.begin(); cfit != common_friends.end(); ++cfit ){
		if( cfit->shareCount > minsharecount ){
                	sameInfoFriends.insert( make_pair<int,int>( cfit->userId, 1 ) );
		} else{
                	sameInfoFriends.insert( make_pair<int,int>( cfit->userId, 0 ) );
		}
		friends.push_back( cfit->userId );
        }

	calc_match( infos, friends, sameInfoFriends );

        multimap<int,int> res;
        for(map<int,int>::iterator mit=sameInfoFriends.begin(); mit!=sameInfoFriends.end(); ++mit) {
                res.insert( make_pair<int,int>( mit->second, mit->first ) );
        }

        return res;
}

::MyUtil::IntSeq FriendFinderInfoCacheI::getFriendListByInfo(
                ::Ice::Int userId, ::Ice::Int limit, const Ice::Current& current) {
        multimap<int,int> ret = cal_friend_byinfo( userId, userId);
        MyUtil::IntSeq res;
        for(multimap<int,int>::reverse_iterator mmit=ret.rbegin(); mmit!=ret.rend(); ++mmit) {
                res.push_back( mmit->second );
        }
        if( limit < 0 ){
        	MCE_INFO( "[getFriendListByInfo] userId=" << userId << " res.size()=" << res.size() << " limit=" << limit << " " << current.con->toString().substr(current.con->toString().find("remote",25)) );
                return res;
        }
        MCE_INFO( "[getFriendListByInfo] userId=" << userId << " res.size()=" << res.size() << " limit=" << limit << " " << current.con->toString().substr(current.con->toString().find("remote",25)) );
        return ( res.end() - res.begin() > limit ) ? MyUtil::IntSeq( res.begin(), res.begin()+limit ) : res;
}

::MyUtil::IntSeq FriendFinderInfoCacheI::getInfoMatchFriendList(
                ::Ice::Int userId,::Ice::Int limit, const Ice::Current& current) {
        multimap<int,int> ret = cal_friend_byinfo( userId,userId );
        MyUtil::IntSeq res;
        for(multimap<int,int>::reverse_iterator mmit=ret.rbegin(); mmit!=ret.rend(); ++mmit) {
                if( mmit->first == 0 ){
                        break;
                }
                res.push_back( mmit->second );
        }
        if( limit < 0 ){
        	MCE_INFO( "[getInfoMatchFriendList] userId=" << userId << " map.size()=" << ret.size() << " res.size()=" << res.size() << " limit=" << limit << " " << current.con->toString().substr(current.con->toString().find("remote",25)) );
                return res;
        }
        MCE_INFO( "[getInfoMatchFriendList] userId=" << userId << " map.size()=" << ret.size() << " res.size()=" << res.size() << " limit=" << limit << " " << current.con->toString().substr(current.con->toString().find("remote",25)) );
        return ( res.end() - res.begin() > limit ) ? MyUtil::IntSeq( res.begin(), res.begin()+limit ) : res;
}

::MyUtil::IntSeq FriendFinderInfoCacheI::getFriendListByGuestInfo(
                ::Ice::Int hostId, ::Ice::Int guestId, ::Ice::Int limit, const Ice::Current& current) {
        multimap<int,int> ret = cal_friend_byinfo( hostId, guestId );
        MyUtil::IntSeq res;
        for(multimap<int,int>::reverse_iterator mmit=ret.rbegin(); mmit!=ret.rend(); ++mmit) {
                res.push_back( mmit->second );
        }
        if( limit < 0 ){
        	MCE_INFO( "[getFriendListByGuestInfo] hostId=" << hostId << " guestId=" << guestId << " res.size()=" << res.size() << " limit=" << limit << " " << current.con->toString().substr(current.con->toString().find("remote",25)) );
                return res;
        }
        MCE_INFO( "[getFriendListByGuestInfo] hostId=" << hostId << " guestId=" << guestId << " res.size()=" << res.size() << " limit=" << limit << " " << current.con->toString().substr(current.con->toString().find("remote",25)) );
        return ( res.end() - res.begin() > limit ) ? MyUtil::IntSeq( res.begin(), res.begin()+limit ) : res;
}

::MyUtil::IntSeq FriendFinderInfoCacheI::getGuestInfoMatchFriendList(
                ::Ice::Int hostId, ::Ice::Int guestId, ::Ice::Int limit, const Ice::Current& current) {
        multimap<int,int> ret = cal_friend_byinfo( hostId,guestId);
        MyUtil::IntSeq res;
        for(multimap<int,int>::reverse_iterator mmit=ret.rbegin(); mmit!=ret.rend(); ++mmit) {
                if( mmit->first == 0 ){
                        break;
                }
                res.push_back( mmit->second );
        }
        if( limit < 0 ){
        	MCE_INFO( "[getGuestInfoMatchFriendList] hostId=" << hostId << " guestId=" << guestId << " res.size()=" << res.size() << " limit=" << limit << " " << current.con->toString().substr(current.con->toString().find("remote",25)) );
                return res;
        }
        MCE_INFO( "[getGuestInfoMatchFriendList] hostId=" << hostId << " guestId=" << guestId << " res.size()=" << res.size() << " limit=" << limit << " " << current.con->toString().substr(current.con->toString().find("remote",25)) );
        return ( res.end() - res.begin() > limit ) ? MyUtil::IntSeq( res.begin(), res.begin()+limit ) : res;
}

::MyUtil::IntSeq FriendFinderInfoCacheI::getGuestInfoMatchFriendListBatch(
		const ::MyUtil::IntSeq& hostIds, ::Ice::Int guestId, const Ice::Current& current) {
        multimap<int,int> ret = cal_friend_byinfo_batch( hostIds,guestId );
        MyUtil::IntSeq res;
        for(multimap<int,int>::reverse_iterator mmit=ret.rbegin(); mmit!=ret.rend(); ++mmit) {
                if( mmit->first == 0 ){
                        break;
                }
                res.push_back( mmit->second );
        }
        MCE_INFO( "[getGuestInfoMatchFriendList] hostIds.size()=" << hostIds.size() << " guestId=" << guestId << " res.size()=" << res.size() << " " << current.con->toString().substr(current.con->toString().find("remote",25)) );
        return res;
}

::MyUtil::IntSeq FriendFinderInfoCacheI::getCommonFriendListByGuestInfo(
                ::Ice::Int userId, ::Ice::Int minsharecount, ::Ice::Int limit, const Ice::Current& current) {
        multimap<int,int> ret = cal_common_friend_byinfo( userId, minsharecount );
        MyUtil::IntSeq res;
        for(multimap<int,int>::reverse_iterator mmit=ret.rbegin(); mmit!=ret.rend(); ++mmit) {
                if( mmit->first == 0 ){
                        break;
                }
                res.push_back( mmit->second );
        }
        if( limit < 0 ){
        	MCE_INFO( "[getCommonFriendListByGuestInfo] userId=" << userId << " minsharecount=" << minsharecount << " res.size()=" << res.size() << " limit=" << limit << " " << current.con->toString().substr(current.con->toString().find("remote",25)) );
                return res;
        }
        MCE_INFO( "[getCommonFriendListByGuestInfo] userId=" << userId << " minsharecount=" << minsharecount << " res.size()=" << res.size() << " limit=" << limit << " " << current.con->toString().substr(current.con->toString().find("remote",25)) );
        return ( res.end() - res.begin() > limit ) ? MyUtil::IntSeq( res.begin(), res.begin()+limit ) : res;
}

void FriendFinderInfoCacheI::Transfer(const std::vector<PItem>& resource, RecommendItemSeq& dest) {
	MCE_INFO("[FriendFinderInfoCacheI] Transfer resource:" << resource.size());
	for (std::vector<PItem>::const_iterator iter = resource.begin();
			iter != resource.end(); ++iter) {
		RecommendItem item;
		item.uid = iter->uid_;
		item.gender = iter->gender_;
		dest.push_back(item);
	}
}
//********************************************************************************************************

void AutoLoadTask::handle() {
	sleep(30);               //半分钟后开始加载
	MCE_INFO("[AutoLoadTask] handle");
	FriendFinderInfoWorkerReplicaAdapter::instance().createDataFromDB(0);
}

//********************************************************************************************************

void GetUnivThread::run() {
	MCE_INFO("[GetUnivThread] _userId(" << _userId << ")");
	vector< vector<string> >  univInfo;
    NetworkCollector reader;

    reader.getUnivInfo(_userId, univInfo);

	for(vector< vector<string> >::iterator iter=univInfo.begin(); iter!=univInfo.end(); ++iter) {
		Md5Info item;	
		int signal = 0;
        for(vector<string>::iterator it=iter->begin(); it!=iter->end(); ++it) {
            if(*it == "") {
				signal = 1;
                break;
            }
		}
		if(signal == 1) {
			continue;
		}

		item.condition = *iter;
		item.md5 = MD5Util::stringSeqToMD5( *iter );
	
		ostringstream sr;	
		for (vector<string>::iterator it = item.condition.begin();
				it != item.condition.end(); ++it) {
			sr << *it << " ";
		}
		//MCE_INFO("[GetUnivThread] " << sr.str());

		_item.push_back(item);
	}
}

//********************************************************************************************************

void GetHighschoolThread::run() {
	MCE_INFO("[GetHighschoolThread] _userId(" << _userId << ")");
	vector< vector<string> >  highschoolInfo;
    NetworkCollector reader;

	reader.getHighschoolInfo(_userId, highschoolInfo);

	for(vector< vector<string> >::iterator iter=highschoolInfo.begin(); iter!=highschoolInfo.end(); ++iter) {
		Md5Info item;	
		int signal = 0;
        for(vector<string>::iterator it=iter->begin(); it!=iter->end(); ++it) {
            if(*it == "") {
				signal = 1;
                break;
            }
		}
		if(signal == 1) {
			continue;
		}

		item.condition = *iter;
		item.md5 = MD5Util::stringSeqToMD5( *iter );

		ostringstream sr;	
		for (vector<string>::iterator it = item.condition.begin();
				it != item.condition.end(); ++it) {
			sr << *it << " ";
		}
		//MCE_INFO("[GetHighschoolThread] " << sr.str());

		_item.push_back(item);
	}
}

//********************************************************************************************************

void GetJuniorschoolThread::run() {
	MCE_INFO("[GetJuniorschoolThread] _userId(" << _userId << ")");
	vector< vector<string> >  juniorschoolInfo;
    NetworkCollector reader;

	reader.getJuniorschoolInfo(_userId, juniorschoolInfo);

	for(vector< vector<string> >::iterator iter=juniorschoolInfo.begin(); iter!=juniorschoolInfo.end(); ++iter) {
		Md5Info item;	
		int signal = 0;
        for(vector<string>::iterator it=iter->begin(); it!=iter->end(); ++it) {
            if(*it == "") {
				signal = 1;
                break;
            }
		}
		if(signal == 1) {
			continue;
		}

		item.condition = *iter;
		item.md5 = MD5Util::stringSeqToMD5( *iter );

		ostringstream sr;	
		for (vector<string>::iterator it = item.condition.begin();
				it != item.condition.end(); ++it) {
			sr << *it << " ";
		}
		//MCE_INFO("[GetJuniorschoolThread] " << sr.str());

		_item.push_back(item);
	}
}

//********************************************************************************************************

void GetElementaryschoolThread::run() {
	MCE_INFO("[GetElementaryschoolThread] _userId(" << _userId << ")");
	vector< vector<string> >  elementaryschoolInfo;
    NetworkCollector reader;

	reader.getElementaryschoolInfo(_userId, elementaryschoolInfo);

	for(vector< vector<string> >::iterator iter=elementaryschoolInfo.begin(); iter!=elementaryschoolInfo.end(); ++iter) {
		Md5Info item;	
		int signal = 0;
        for(vector<string>::iterator it=iter->begin(); it!=iter->end(); ++it) {
            if(*it == "") {
				signal = 1;
                break;
            }
		}
		if(signal == 1) {
			continue;
		}

		item.condition = *iter;
		item.md5 = MD5Util::stringSeqToMD5( *iter );

		ostringstream sr;	
		for (vector<string>::iterator it = item.condition.begin();
				it != item.condition.end(); ++it) {
			sr << *it << " ";
		}
		//MCE_INFO("[GetElementaryschoolThread] " << sr.str());

		_item.push_back(item);
	}
}

//********************************************************************************************************

Ice::ObjectPtr NetworkDataFactory::create(::Ice::Int id) {
	MCE_INFO("[NetworkDataFactory] id(" << id << ")");
	Network result;
	vector<IceUtil::ThreadControl> threadsCtrl;
	
	IceUtil::Handle<GetUnivThread> GetUnivThreadPtr = new GetUnivThread(id, result.univInfoSeq);
	threadsCtrl.push_back( GetUnivThreadPtr->start() );

	IceUtil::Handle<GetHighschoolThread> GetHighschoolThreadPtr = new GetHighschoolThread(id, result.highschoolInfoSeq);
	threadsCtrl.push_back( GetHighschoolThreadPtr->start() );

	IceUtil::Handle<GetJuniorschoolThread> GetJuniorschoolThreadPtr = new GetJuniorschoolThread(id, result.juniorschoolInfoSeq);
	threadsCtrl.push_back( GetJuniorschoolThreadPtr->start() );

	IceUtil::Handle<GetElementaryschoolThread> GetElementaryschoolThreadPtr = new GetElementaryschoolThread(id, result.elementaryschoolInfoSeq);
	threadsCtrl.push_back( GetElementaryschoolThreadPtr->start() );

	for(vector<IceUtil::ThreadControl>::iterator i=threadsCtrl.begin(); i!=threadsCtrl.end(); ++i) {
		i->join();
	}
	return new NetworkData(id, result); 
}

//********************************************************************************************************

Ice::ObjectPtr SchoolAreaFactory::create(::Ice::Int id) {
	MCE_INFO("[SchoolAreaFactory] create id(" << id << ")");
	SchoolArea result;
	std::vector<IceUtil::ThreadControl> threadsCtrl;

	IceUtil::Handle<LoadHighSchoolPeerThread> LoadHighSchoolPeerThreadPtr = 
		new LoadHighSchoolPeerThread(id, result.highschool);
	threadsCtrl.push_back(LoadHighSchoolPeerThreadPtr->start());

	IceUtil::Handle<LoadJuniorHighSchoolPeerThread > LoadJuniorSchoolPeerThreadPtr = 
		new LoadJuniorHighSchoolPeerThread(id, result.juniorschool);
	threadsCtrl.push_back(LoadJuniorSchoolPeerThreadPtr->start());

	IceUtil::Handle<LoadSpecialSchoolPeerThread > LoadSpecialSchoolPeerThreadPtr = 
		new LoadSpecialSchoolPeerThread(id, result.college);
	threadsCtrl.push_back(LoadSpecialSchoolPeerThreadPtr->start());

	for(vector<IceUtil::ThreadControl>::iterator i=threadsCtrl.begin(); i!=threadsCtrl.end(); ++i) {
		i->join();
	}

	std::ostringstream oss;
	for (MyUtil::StrSeq::iterator iter = result.highschool.condition.begin();
			iter != result.highschool.condition.end(); ++iter) {
		oss << *iter;
	}
	oss << result.highschool.md5;
	oss<< ":";
	for (MyUtil::StrSeq::iterator iter = result.juniorschool.condition.begin();
			iter != result.juniorschool.condition.end(); ++iter) {
		oss << *iter;
	}
	oss << result.juniorschool.md5;
	oss<< ":";
	for (MyUtil::StrSeq::iterator iter = result.college.condition.begin();
			iter != result.college.condition.end(); ++iter) {
		oss << *iter;
	}
	oss << result.college.md5;
	MCE_DEBUG(oss.str());
	return new SchoolAreaData(id, result);
}

//********************************************************************************************************

void BirthdayInfoData::set(int birthYear, const std::vector<int>& orderedIds) {
	MCE_INFO("[BirthdayInfoData] set birthYear(" << birthYear << ") orderedIds size(" << orderedIds.size() << ")");
	IceUtil::RWRecMutex::WLock lock(_init_flag_mutex);	

	int mapLength = (int)_birthdayMap[birthYear].size();
	int vecLength = (int)orderedIds.size();
	int length = mapLength + vecLength;
	_birthdayMap[birthYear].resize(length);
	//for(vector<int>::const_iterator iter=orderedIds.begin(); iter!=orderedIds.end(); ++iter) {
	//	_birthdayMap[birthYear].push_back(*iter);
	//}	
	for (int i = mapLength; i < length; ++i) {
		_birthdayMap[birthYear][i] = orderedIds.at(i - mapLength);
	}
}

std::vector<int> BirthdayInfoData::getBySimilarYear(int birthYear, const std::vector<int>& friendIds) {
	MCE_INFO("[BirthdayInfoData] getBySimilarYear birthYear(" << birthYear << ") friendIds size(" << friendIds.size() << ")");
	IceUtil::RWRecMutex::RLock lock(_init_flag_mutex);

	struct timeval tvStart,tvEnd;
    double linStart = 0,linEnd = 0,lTime = 0;
    gettimeofday(&tvStart, NULL);

	vector<int> result;
	if(birthYear <= 0) {
		return friendIds;
	}
	int lessOneYear = birthYear - 1;
	int sameYear = birthYear;
	int moreOneYear = birthYear + 1;	

	for(vector<int>::const_iterator iter=friendIds.begin(); iter!=friendIds.end(); ++iter) {
		vector<int>::iterator fLessIt = lower_bound(_birthdayMap[lessOneYear].begin(), 
													_birthdayMap[lessOneYear].end(), 
													*iter);
		if(fLessIt != _birthdayMap[lessOneYear].end() && 
				*fLessIt == *iter) {
			result.push_back(*iter);
			continue;
		}

		vector<int>::iterator fSameIt = lower_bound(_birthdayMap[sameYear].begin(), 
													_birthdayMap[sameYear].end(), 
													*iter);
		if(fSameIt != _birthdayMap[sameYear].end() && 
				*fSameIt == *iter) {
			result.push_back(*iter);
			continue;
		}
		
		vector<int>::iterator fMoreIt = lower_bound(_birthdayMap[moreOneYear].begin(), 
													_birthdayMap[moreOneYear].end(), 
													*iter);
		if(fMoreIt != _birthdayMap[moreOneYear].end() && 
				*fMoreIt == *iter) {
			result.push_back(*iter);
			continue;
		}
	}

    gettimeofday(&tvEnd, NULL);
    linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);  //unit S
    linEnd = ((double)tvEnd.tv_sec * 1000000 + (double)tvEnd.tv_usec);        //unit S
    lTime = linEnd - linStart;
    lTime = lTime/1000000;
	MCE_INFO("[BirthdayInfoData] getBySimilarYear run time (gettimeofday) : " << lTime << " second!");

	return result;
}

int BirthdayInfoData::getUserBirthyear(int userId) {
	MCE_INFO("[BirthdayInfoData] getUserBirthyear userId(" << userId << ")");
	IceUtil::RWRecMutex::RLock lock(_init_flag_mutex);

	struct timeval tvStart,tvEnd;
    double linStart = 0,linEnd = 0,lTime = 0;
    gettimeofday(&tvStart, NULL);

	//int year = 1986;
	//MCE_INFO("[BirthdayInfoData] 1986 size : " << _birthdayMap[year].size());
	//ostringstream tr;
	//for (vector<int>::iterator iter = _birthdayMap[year].begin();
	//		iter != _birthdayMap[year].end(); ++iter) {
	//	tr << *iter << " ";
	//}
	//MCE_INFO(tr.str());
	//MCE_INFO("[BirthdayInfoData] getUserBirthyear _birthdayMap size = " << _birthdayMap.size());

	int birthyear = 0;
	for(map< int, vector<int> >::iterator iter=_birthdayMap.begin();
			iter!=_birthdayMap.end(); ++iter) {
		vector<int>::iterator fIt = lower_bound(iter->second.begin(),
												iter->second.end(),
												userId);
		//MCE_INFO("[BirthdayInfoData] iter->second size : " << iter->second.size());
		if(fIt != iter->second.end() &&
				*fIt == userId) {
			//MCE_INFO("[BirthdayInfoData] fIt==userId  second size : " << iter->second.size());
			birthyear = iter->first;
			break;
		}		
	}

    gettimeofday(&tvEnd, NULL);
    linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);  //unit S
    linEnd = ((double)tvEnd.tv_sec * 1000000 + (double)tvEnd.tv_usec);        //unit S
    lTime = linEnd - linStart;
    lTime = lTime/1000000;
	//MCE_INFO("[BirthdayInfoData] getUserBirthyear run time (gettimeofday) : " << lTime << " second!");

	return birthyear;
}

//********************************************************************************************************

void RebuildNetworkTask::handle() {
	MCE_INFO("[RebuildNetworkTask] id(" << _id << ")");
	Network result;
	vector<IceUtil::ThreadControl> threadsCtrl;
	
	IceUtil::Handle<GetUnivThread> GetUnivThreadPtr = new GetUnivThread(_id, result.univInfoSeq);
	threadsCtrl.push_back( GetUnivThreadPtr->start() );

	IceUtil::Handle<GetHighschoolThread> GetHighschoolThreadPtr = new GetHighschoolThread(_id, result.highschoolInfoSeq);
	threadsCtrl.push_back( GetHighschoolThreadPtr->start() );

	IceUtil::Handle<GetJuniorschoolThread> GetJuniorschoolThreadPtr = new GetJuniorschoolThread(_id, result.juniorschoolInfoSeq);
	threadsCtrl.push_back( GetJuniorschoolThreadPtr->start() );

	IceUtil::Handle<GetElementaryschoolThread> GetElementaryschoolThreadPtr = new GetElementaryschoolThread(_id, result.elementaryschoolInfoSeq);
	threadsCtrl.push_back( GetElementaryschoolThreadPtr->start() );

	for(vector<IceUtil::ThreadControl>::iterator i=threadsCtrl.begin(); i!=threadsCtrl.end(); ++i) {
		i->join();
	}
	
	NetworkDataPtr obj = new NetworkData(_id, result); 
	ServiceI::instance().addObject(obj, NETWORK_DATA, _id);
}
