#include "BackendUtil.h"
#include "OfferCreator.h"

using namespace std;
using namespace MyUtil;
using namespace xce::friendfinder;
using namespace xce::offerfriends;

//***************************************************************************

OfferDataNSeq BackendData::get(int type) {
	map<int, OfferDataNSeq>::iterator it = _typedOfferDataNSeq.find( type );
	if( it != _typedOfferDataNSeq.end() ) {
		return it->second;
	} else {
		return OfferDataNSeq();
	}
}

void BackendData::add(int type, const OfferDataNSeq& offers) {
	MCE_INFO("[BackendData] ada type(" << type << ") offers size(" << offers.size() << ")");
	IceUtil::Mutex::Lock lock(*this);
	map<int, OfferDataNSeq>::iterator it = _typedOfferDataNSeq.find( type );
	if(it == _typedOfferDataNSeq.end()){
		pair<map<int, OfferDataNSeq>::iterator, bool> insRes = _typedOfferDataNSeq.insert(make_pair(type,offers));
		//it = insRes.first;
	}
}

void BackendData::remove(int type, int userId) {
	IceUtil::Mutex::Lock lock(*this);
	map<int, OfferDataNSeq>::iterator it = _typedOfferDataNSeq.find( type );
	if(it == _typedOfferDataNSeq.end()){ 
		return;
	}
	for(OfferDataNSeq::iterator iter=it->second.begin(); iter!=it->second.end(); ++iter){
		if(userId == iter->userId){
			it->second.erase(iter);
			break;
		}
	}
}

//**************************************************************************************************************

BackendDataPtr BackendProducer::create(int userId, 
																			 const map<int,int>& weights,
																			 const MyUtil::IntSeq& applyList,
																			 const MyUtil::IntSeq& friendList,
																			 const MyUtil::IntSeq& blockList,
																			 const MyUtil::IntSeq& commBlockList,
              												 std::ostringstream& createLog,
																			 std::ostringstream& createDetail)
{
	OfferDataNSeq offerCommonFriend = createFromCommonFriend(userId, 300, weights,
																				applyList, friendList, blockList, commBlockList,
																				createLog,createDetail);
	//MCE_INFO("[BackendProducer] create userId:" << userId << " weights size:" << weights.size() 
	//		<< " offerCommonFriend size:" << offerCommonFriend.size());

	map<int, OfferDataNSeq> offerSameInfoFriends = createFromSameInfoFriend(userId, 300, weights,
																								 applyList, friendList, blockList, commBlockList,
																								 createLog,createDetail);

	MCE_INFO("[BackendProducer] create userId:" << userId << " weights size:" << weights.size() 
			<< " offerCommonFriend size:" << offerCommonFriend.size()
			<< " offerSameInfoFriends size : " << offerSameInfoFriends.size());

	OfferDataNSeq offerIPFriend = createFromIPFriend( userId, 300, weights,
		applyList, friendList, blockList, commBlockList,
		createLog,createDetail );

	BackendDataPtr res = new BackendData();
	res->add(BaseTypeCommonFriend, offerCommonFriend);
	map<int,OfferDataNSeq>::iterator it = offerSameInfoFriends.find(BaseTypeSameInfoWorkPlace);
	if( it!=offerSameInfoFriends.end() ){
		res->add(BaseTypeSameInfoWorkPlace,it->second);
	}
	it = offerSameInfoFriends.find(BaseTypeSameInfoUniv);
	if( it!=offerSameInfoFriends.end() ){
		res->add(BaseTypeSameInfoUniv,it->second);
	}
	it = offerSameInfoFriends.find(BaseTypeSameInfoHighSchool);
	if( it!=offerSameInfoFriends.end() ){
		res->add(BaseTypeSameInfoHighSchool,it->second);
	}
	it = offerSameInfoFriends.find(BaseTypeSameInfoJuniorSchool);
	if( it!=offerSameInfoFriends.end() ){
		res->add(BaseTypeSameInfoJuniorSchool,it->second);
	}
	it = offerSameInfoFriends.find(BaseTypeSameInfoElementarySchool);
	if( it!=offerSameInfoFriends.end() ){
		res->add(BaseTypeSameInfoElementarySchool,it->second);
	}
	res->add(BaseTypeIPSame,offerIPFriend);

	return res;
}

BackendDataPtr BackendProducer::createHighSchoolData(int userId, 
																										 const map<int,int>& weights,
																										 const MyUtil::IntSeq& applyList,
																										 const MyUtil::IntSeq& friendList,
																										 const MyUtil::IntSeq& blockList,
																										 const MyUtil::IntSeq& commBlockList,
                																		 std::ostringstream& createLog,
																										 std::ostringstream& createDetail)
{
	OfferDataNSeq offerCommonFriend = createFromCommonFriend(userId, 300, weights,
																				applyList, friendList, blockList, commBlockList,
																				createLog,createDetail);
	MCE_INFO("[BackendProducer] createHighSchoolData userId:" << userId << " weights size:" << weights.size() 
			<< " offerCommonFriend size:" << offerCommonFriend.size());

	map<int, OfferDataNSeq> offerSameInfoFriends = createFromSameInfoFriendForHighSchool(userId, 300, weights,
																								 						applyList, friendList, blockList, commBlockList,
																								 						createLog, createDetail);

	MCE_INFO("[BackendProducer] create userId:" << userId << " weights size:" << weights.size() 
			<< " offerCommonFriend size:" << offerCommonFriend.size()
			<< " offerSameInfoFriends size : " << offerSameInfoFriends.size());

	OfferDataNSeq offerIPFriend = createFromIPFriend( userId, 300, weights,
		applyList, friendList, blockList, commBlockList,
		createLog,createDetail );

	BackendDataPtr res = new BackendData();
	res->add(BaseTypeCommonFriend, offerCommonFriend);

	map<int, OfferDataNSeq>::iterator it = offerSameInfoFriends.find(BaseTypeSameInfoWorkPlace);
	if( it != offerSameInfoFriends.end() ){
		res->add(BaseTypeSameInfoWorkPlace,it->second);
	}

	it = offerSameInfoFriends.find(BaseTypeSameInfoUniv);
	if( it != offerSameInfoFriends.end() ){
		res->add(BaseTypeSameInfoUniv,it->second);
	}

	it = offerSameInfoFriends.find(BaseTypeSameInfoHighSchool);
	if( it != offerSameInfoFriends.end() ){
		res->add(BaseTypeSameInfoHighSchool,it->second);
	}

	it = offerSameInfoFriends.find(BaseTypeSameInfoJuniorSchool);
	if( it != offerSameInfoFriends.end() ){
		res->add(BaseTypeSameInfoJuniorSchool,it->second);
	}

	it = offerSameInfoFriends.find(BaseTypeSameInfoElementarySchool);
	if( it != offerSameInfoFriends.end() ){
		res->add(BaseTypeSameInfoElementarySchool,it->second);
	}

/*中学推人*/
	it = offerSameInfoFriends.find(BaseTypeAreaEnrollSame);
	if( it != offerSameInfoFriends.end() ){
		res->add(BaseTypeAreaEnrollSame, it->second);
	}

	res->add(BaseTypeIPSame,offerIPFriend);

	return res;
}

OfferDataNSeq BackendProducer::createFromCommonFriend(int userId,
																		 									int commFriendLimit,
																		 									const std::map<int, int>& weights,
																		 									const MyUtil::IntSeq& applyList,
																		 									const MyUtil::IntSeq& friendList,
																		 									const MyUtil::IntSeq& blockList,
																		 									const MyUtil::IntSeq& commBlockList,
																		 									std::ostringstream& createLog,
																		 									std::ostringstream& createDetail)
{
	struct timeval tvStart, tvComm;
	gettimeofday( &tvStart, NULL );

	OfferDataNSeq res;
	com::xiaonei::service::CommonFriendSeq commonFriends;
	commonFriends = OfferFriendsUtil::calCommonFriend(userId, applyList, friendList, blockList, commBlockList, commFriendLimit);

	vector<int> commonIdSeq;
	for (vector<com::xiaonei::service::CommonFriend>::const_iterator iter = commonFriends.begin(); 
			iter != commonFriends.end(); ++iter) {
		commonIdSeq.push_back(iter->userId);
	}

	Int2IntMap commonIdMap = OfferFriendsUtil::getFriendCountBatch(commonIdSeq);  //何必呢？？
	int base_weight = 0;
	std::map<int, int>::const_iterator weiIt = weights.find(BaseTypeCommonFriend);
	if (weiIt != weights.end()) {
		base_weight = weiIt->second;
	}

	createDetail << " comms_";
	for (vector<com::xiaonei::service::CommonFriend>::const_iterator commonIt = commonFriends.begin(); 
			commonIt != commonFriends.end(); ++commonIt) {
		Int2IntMap::const_iterator fIt = commonIdMap.find(commonIt->userId);
		short ffSize;   //这个干嘛了？？
		if (fIt == commonIdMap.end()) {
			ffSize = 1;
			continue;
		}
		ffSize = (short)fIt->second;
		OfferDataN commFriend;
		commFriend.userId = commonIt->userId;
		short sameElementSize = (short)commonIt->shares.size();
		commFriend.weight = base_weight;

		commFriend.sign = ((int)0) | (((int)1) << BaseTraitCommonFriend);
		res.push_back(commFriend);
		createDetail << commFriend.userId << "_";
	}

	gettimeofday( &tvComm, NULL );
	double linCommStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);
	double linCommEnd = ((double)tvComm.tv_sec*1000000 + (double)tvComm.tv_usec);
	double linCommTime = linCommEnd - linCommStart;
	linCommTime = linCommTime/1000000;
	createLog	<< " commonFriends(" << commonFriends.size() << ")(" << linCommTime << "s)";
	return res;
}

std::map<int, OfferDataNSeq> BackendProducer::createFromSameInfoFriend(int userId,
																																			 int infoLimit,
																																			 const std::map<int,int>& weights,
																																			 const MyUtil::IntSeq& applyList,
																																			 const MyUtil::IntSeq& friendList,
																																			 const MyUtil::IntSeq& blockList,
																																			 const MyUtil::IntSeq& commBlockList,
																																			 std::ostringstream& createLog,
																																			 std::ostringstream& createDetail) 
{
	struct timeval tvStart, tvInfo;
	gettimeofday( &tvStart, NULL );

	map<int, OfferDataNSeq> res;
	IntSeq filterList = friendList;
	filterList.push_back(userId);
	//MCE_INFO("[BackendProducer] createFromSameInfoFriend filterList size:" << filterList.size());

  NetworkInfo sameInfoFriends = OfferFriendsUtil::getNetworkInfoWithHostageFromDB(userId, infoLimit, filterList);

	int base_weight = 0;
	map<int, int>::const_iterator weiIt = weights.find(BaseTypeSameInfoUniv);
	if (weiIt != weights.end()) {
		base_weight = weiIt->second;
	}

	int univCount = 0;
	createDetail << " univ_";
	pair<map<int, OfferDataNSeq>::iterator, bool> ins_univ_iter = res.insert(make_pair(BaseTypeSameInfoUniv, OfferDataNSeq()));

	for (ItemSeq::iterator iter = sameInfoFriends.univInfo.begin(); 
			iter != sameInfoFriends.univInfo.end(); ++iter) {   //添加大学资料匹配好友
		for(MyUtil::IntSeq::iterator it = iter->idSeq.begin(); 
				it != iter->idSeq.end(); ++it) {
			OfferDataN infoFriend;
			infoFriend.weight = base_weight;
			infoFriend.userId = *it;
			infoFriend.sign = ((int)0) | (((int)1)<<BaseTraitSameInfoUniv);
			ins_univ_iter.first->second.push_back(infoFriend);
			createDetail << infoFriend.userId << "_";
			++univCount;
		}
	}

	base_weight = 0;
	weiIt = weights.find(BaseTypeSameInfoHighSchool);
	if (weiIt != weights.end()) {
		base_weight = weiIt->second;
	}

	int highCount = 0;
	createDetail << " high_";
	pair<map<int, OfferDataNSeq>::iterator, bool> ins_high_iter = res.insert(make_pair(BaseTypeSameInfoHighSchool, OfferDataNSeq()));

	for (ItemSeq::iterator iter = sameInfoFriends.highschoolInfo.begin(); 
			iter != sameInfoFriends.highschoolInfo.end(); ++iter) {   //添加高中资料匹配好友
		for(MyUtil::IntSeq::iterator it=iter->idSeq.begin(); it!=iter->idSeq.end(); ++it) {
			OfferDataN infoFriend;
			infoFriend.weight = base_weight;
			infoFriend.userId = *it;
			infoFriend.sign = ((int)0) | (((int)1)<<BaseTraitSameInfoHighSchool);
			ins_high_iter.first->second.push_back(infoFriend);
			createDetail << infoFriend.userId << "_";
			++highCount;
		}
	}

	base_weight = 0;
	weiIt = weights.find(BaseTypeSameInfoJuniorSchool);
	if (weiIt!=weights.end()) {
		base_weight = weiIt->second;
	}
	int juCount = 0;
	createDetail << " juni_";
	pair<map<int, OfferDataNSeq>::iterator, bool> ins_junior_iter = res.insert(make_pair(BaseTypeSameInfoJuniorSchool, OfferDataNSeq()));

	for (ItemSeq::iterator iter = sameInfoFriends.juniorschoolInfo.begin(); 
			iter != sameInfoFriends.juniorschoolInfo.end(); ++iter) {  //添加初中资料匹配好友
		for(MyUtil::IntSeq::iterator it = iter->idSeq.begin(); 
				it != iter->idSeq.end(); ++it) {
			OfferDataN infoFriend;
			infoFriend.weight = base_weight;
			infoFriend.userId = *it;
			infoFriend.sign = ((int)0) | (((int)1)<<BaseTraitSameInfoJuniorSchool);
			ins_junior_iter.first->second.push_back(infoFriend);
			createDetail << infoFriend.userId << "_";
			++juCount;
		}
	}

	base_weight = 0;
	weiIt = weights.find(BaseTypeSameInfoElementarySchool);
	if (weiIt != weights.end()) {
		base_weight = weiIt->second;
	}

	int eleCount = 0;
	createDetail << " elem_";
	pair<map<int, OfferDataNSeq>::iterator, bool> ins_ele_iter = res.insert(make_pair(BaseTypeSameInfoElementarySchool, OfferDataNSeq()));
	for (ItemSeq::iterator iter = sameInfoFriends.elementaryschoolInfo.begin(); 
			iter != sameInfoFriends.elementaryschoolInfo.end(); ++iter) {  //添加小学资料匹配好友
		for (MyUtil::IntSeq::iterator it = iter->idSeq.begin(); 
				it!=iter->idSeq.end(); ++it) {
			OfferDataN infoFriend;
			infoFriend.weight = base_weight;
			infoFriend.userId = *it;

			infoFriend.sign = ((int)0) | (((int)1)<<BaseTraitSameInfoElementarySchool);
			ins_ele_iter.first->second.push_back(infoFriend);
			createDetail << infoFriend.userId << "_";
			++eleCount;
		}
	}

	gettimeofday(&tvInfo, NULL);
	double linInfoStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);	
	double linInfoEnd = ((double)tvInfo.tv_sec*1000000 + (double)tvInfo.tv_usec);
	double linInfoTime = linInfoEnd - linInfoStart;
	linInfoTime = linInfoTime/1000000;
	createLog	<< " sameInfoFriends("
			<< "(u" << sameInfoFriends.univInfo.size() << ":" << univCount << ")"
			<< "(h" << sameInfoFriends.highschoolInfo.size() << ":" << highCount << ")"
			<< "(j" << sameInfoFriends.juniorschoolInfo.size() << ":" << juCount << ")"
			<< "(e" << sameInfoFriends.elementaryschoolInfo.size() << ":" << eleCount << ")"
			<< "(" << linInfoTime << "s)";
	return res;
}

std::map<int, OfferDataNSeq> BackendProducer::createFromSameInfoFriendForHighSchool(int userId,
																																		 								int infoLimit,
																																		 								const std::map<int,int>& weights,
																																		 								const MyUtil::IntSeq& applyList,
																																		 								const MyUtil::IntSeq& friendList,
																																		 								const MyUtil::IntSeq& blockList,
																																		 								const MyUtil::IntSeq& commBlockList,
																																		 								std::ostringstream& createLog,
																																		 								std::ostringstream& createDetail)
{
	MCE_INFO("[BackendProducer] createFromSameInfoFriendForHighSchool userId:" << userId << " infoLimit:" << infoLimit);
	struct timeval tvStart, tvInfo;
	gettimeofday( &tvStart, NULL );

	map<int, OfferDataNSeq> res;
	IntSeq filterList = friendList;
	filterList.push_back(userId);
	MCE_INFO("[BackendProducer] createFromSameInfoFriendForHighSchool filterList size:" << filterList.size());

  NetworkInfo sameInfoFriends = OfferFriendsUtil::getNetworkInfoWithHostageFromDB(userId, infoLimit, filterList);

	int base_weight = 0;
	map<int, int>::const_iterator weiIt = weights.find(BaseTypeSameInfoUniv);
	if (weiIt != weights.end()) {
		base_weight = weiIt->second;
		MCE_INFO("BaseTypeSameInfoUniv: " << base_weight);
	}

	int univCount = 0;
	createDetail << " univ_";
	pair<map<int, OfferDataNSeq>::iterator, bool> ins_univ_iter = res.insert(make_pair(BaseTypeSameInfoUniv, OfferDataNSeq()));

	for (ItemSeq::iterator iter = sameInfoFriends.univInfo.begin(); 
			iter != sameInfoFriends.univInfo.end(); ++iter) {   //添加大学资料匹配好友
		for(MyUtil::IntSeq::iterator it = iter->idSeq.begin(); 
				it != iter->idSeq.end(); ++it) {
			OfferDataN infoFriend;
			infoFriend.weight = base_weight;
			infoFriend.userId = *it;
			infoFriend.sign = ((int)0) | (((int)1)<<BaseTraitSameInfoUniv);
			ins_univ_iter.first->second.push_back(infoFriend);
			createDetail << infoFriend.userId << "_";
			++univCount;
		}
	}

	base_weight = 0;
	weiIt = weights.find(BaseTypeSameInfoHighSchool);
	if (weiIt != weights.end()) {
		base_weight = weiIt->second;
	}

	int highCount = 0;
	createDetail << " high_";
	pair<map<int, OfferDataNSeq>::iterator, bool> ins_high_iter = res.insert(make_pair(BaseTypeSameInfoHighSchool, OfferDataNSeq()));

	for (ItemSeq::iterator iter = sameInfoFriends.highschoolInfo.begin(); 
			iter != sameInfoFriends.highschoolInfo.end(); ++iter) {   //添加高中资料匹配好友
		for(MyUtil::IntSeq::iterator it=iter->idSeq.begin(); it!=iter->idSeq.end(); ++it) {
			OfferDataN infoFriend;
			infoFriend.weight = base_weight;
			infoFriend.userId = *it;
			infoFriend.sign = ((int)0) | (((int)1)<<BaseTraitSameInfoHighSchool);
			ins_high_iter.first->second.push_back(infoFriend);
			createDetail << infoFriend.userId << "_";
			++highCount;
		}
	}

	base_weight = 0;
	weiIt = weights.find(BaseTypeSameInfoJuniorSchool);
	if (weiIt!=weights.end()) {
		base_weight = weiIt->second;
	}
	int juCount = 0;
	createDetail << " juni_";
	pair<map<int, OfferDataNSeq>::iterator, bool> ins_junior_iter = res.insert(make_pair(BaseTypeSameInfoJuniorSchool, OfferDataNSeq()));

	for (ItemSeq::iterator iter = sameInfoFriends.juniorschoolInfo.begin(); 
			iter != sameInfoFriends.juniorschoolInfo.end(); ++iter) {  //添加初中资料匹配好友
		for(MyUtil::IntSeq::iterator it = iter->idSeq.begin(); 
				it != iter->idSeq.end(); ++it) {
			OfferDataN infoFriend;
			infoFriend.weight = base_weight;
			infoFriend.userId = *it;
			infoFriend.sign = ((int)0) | (((int)1)<<BaseTraitSameInfoJuniorSchool);
			ins_junior_iter.first->second.push_back(infoFriend);
			createDetail << infoFriend.userId << "_";
			++juCount;
		}
	}

	base_weight = 0;
	weiIt = weights.find(BaseTypeSameInfoElementarySchool);
	if (weiIt != weights.end()) {
		base_weight = weiIt->second;
	}

	int eleCount = 0;
	createDetail << " elem_";
	pair<map<int, OfferDataNSeq>::iterator, bool> ins_ele_iter = res.insert(make_pair(BaseTypeSameInfoElementarySchool, OfferDataNSeq()));
	for (ItemSeq::iterator iter = sameInfoFriends.elementaryschoolInfo.begin(); 
			iter != sameInfoFriends.elementaryschoolInfo.end(); ++iter) {  //添加小学资料匹配好友
		for (MyUtil::IntSeq::iterator it = iter->idSeq.begin(); 
				it!=iter->idSeq.end(); ++it) {
			OfferDataN infoFriend;
			infoFriend.weight = base_weight;
			infoFriend.userId = *it;

			infoFriend.sign = ((int)0) | (((int)1)<<BaseTraitSameInfoElementarySchool);
			ins_ele_iter.first->second.push_back(infoFriend);
			createDetail << infoFriend.userId << "_";
			++eleCount;
		}
	}

	/*中学推荐*/
  HighSchoolInfo highSchoolFriends = OfferFriendsUtil::getHighSchoolInfo(userId, infoLimit, filterList);
	base_weight = 0;
	weiIt = weights.find(BaseTypeAreaEnrollSame);
	if (weiIt != weights.end()) {
		base_weight = weiIt->second;
		MCE_INFO("BaseTypeAreaEnrollSame : " << base_weight);
	}

	int areaCount = 0;
	createDetail << " area_";
	pair<map<int, OfferDataNSeq>::iterator, bool> ins_area_iter = res.insert(make_pair(BaseTypeAreaEnrollSame, OfferDataNSeq()));
	for (RecommendItemSeq::iterator iter = highSchoolFriends.highschoolseq.begin(); 
			iter != highSchoolFriends.highschoolseq.end(); ++iter) {
		OfferDataN infoFriend;
		infoFriend.weight = base_weight;
		infoFriend.userId = iter->uid;

		infoFriend.sign = ((int)0) | (((int)1)<<BaseTraitAreaEnrollSame);
		ins_area_iter.first->second.push_back(infoFriend);
		createDetail << infoFriend.userId << "_";
		++areaCount;
	}
	for (RecommendItemSeq::iterator iter = highSchoolFriends.juniorschoolseq.begin(); 
			iter != highSchoolFriends.juniorschoolseq.end(); ++iter) {
		OfferDataN infoFriend;
		infoFriend.weight = base_weight;
		infoFriend.userId = iter->uid;

		infoFriend.sign = ((int)0) | (((int)1)<<BaseTraitAreaEnrollSame);
		ins_area_iter.first->second.push_back(infoFriend);
		createDetail << infoFriend.userId << "_";
		++areaCount;
	}
	for (RecommendItemSeq::iterator iter = highSchoolFriends.collegeseq.begin(); 
			iter != highSchoolFriends.collegeseq.end(); ++iter) {
		OfferDataN infoFriend;
		infoFriend.weight = base_weight;
		infoFriend.userId = iter->uid;

		infoFriend.sign = ((int)0) | (((int)1)<<BaseTraitAreaEnrollSame);
		ins_area_iter.first->second.push_back(infoFriend);
		createDetail << infoFriend.userId << "_";
		++areaCount;
	}

	gettimeofday(&tvInfo, NULL);
	double linInfoStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);	
	double linInfoEnd = ((double)tvInfo.tv_sec*1000000 + (double)tvInfo.tv_usec);
	double linInfoTime = linInfoEnd - linInfoStart;
	linInfoTime = linInfoTime/1000000;
	createLog	<< " sameInfoFriends("
			<< "(u" << sameInfoFriends.univInfo.size() << ":" << univCount << ")"
			<< "(h" << sameInfoFriends.highschoolInfo.size() << ":" << highCount << ")"
			<< "(j" << sameInfoFriends.juniorschoolInfo.size() << ":" << juCount << ")"
			<< "(e" << sameInfoFriends.elementaryschoolInfo.size() << ":" << eleCount << ")"
			<< "(a" << res[BaseTypeAreaEnrollSame].size() << ":" << areaCount << ")"
			<< "(" << linInfoTime << "s)";
	MCE_INFO("BackendUtil createlog << " << createLog.str());

	return res;
}

OfferDataNSeq BackendProducer::createFromIPFriend( int userId, int ipFriendLimit, const map<int,int>& weights,
		const IntSeq& applyList, const IntSeq& friendList, const IntSeq& blockList, const IntSeq& commBlockList,
		ostringstream& createLog, ostringstream& createDetail ){
	struct timeval tvStart, tvIP;
	gettimeofday( &tvStart, NULL );

	OfferDataNSeq res;

	IntSeq filterList( friendList );	
	filterList.push_back(userId);
	filterList.insert( filterList.end(), applyList.begin(), applyList.end() );
	sort( filterList.begin(), filterList.end() );

	int ipLimit = 1;
	xce::friendfinder::IPNoteSeq ipSeq = OfferFriendsUtil::getFreqIP(userId, ipLimit);

	int base_weight = 0;
	map<int,int>::const_iterator weiIt = weights.find(BaseTypeIPSame);
	if( weiIt!=weights.end() ) base_weight = weiIt->second;
	vector<int> ipFriends;
	createDetail << " ip_";
	if( !ipSeq.empty() ) {
		ipFriends = OfferFriendsUtil::getUsersWithMaskByIP( ipSeq.at(0).ip, ipFriendLimit, filterList );
		/*
		char chrLongValue[30];
		sprintf(chrLongValue, "%lu", ipSeq.at(0).ip);
		*/
		for(vector<int>::iterator iter=ipFriends.begin(); iter!=ipFriends.end(); ++iter){
			OfferDataN ipFriend;
			ipFriend.userId = *iter;
			ipFriend.weight = base_weight;
			//ipFriend.infos.insert(pair<int, string>(BaseTraitIPSame, string(chrLongValue)));
			ipFriend.sign = ipFriend.sign | (((int)1)<<BaseTraitIPSame);
			res.push_back(ipFriend);
			createDetail << ipFriend.userId << "_";
		}
		createLog << " ipFriends(" << ipSeq.at(0).ip << ")(" << ipFriends.size() << ")";
	} else {
		createLog << " ipFriends(0)(" << ipFriends.size() << ")";
	}

	gettimeofday( &tvIP, NULL );
	double linIPStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);
	double linIPEnd = ((double)tvIP.tv_sec*1000000 + (double)tvIP.tv_usec);
	double linIPTime = linIPEnd - linIPStart;
	linIPTime = linIPTime/1000000;
	createLog << "(" << linIPTime << "s)";
	return res;

}
