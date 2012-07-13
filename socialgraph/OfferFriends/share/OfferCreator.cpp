#include "OfferCreator.h"

using namespace std;
using namespace MyUtil;
using namespace xce::offerfriends;

//***************************************************************************

OfferCacheDataNPtr OfferCreator::create(int userId,
																				int resultSize,
																				ostringstream& createRes)
{
	MCE_INFO("OfferCreator create userId:" << userId << " resultSize:" << resultSize);
	ostringstream createLog;
	ostringstream createDetail;

	if (resultSize <= 0) {
		createLog << "resultSize error";
		return new OfferCacheDataN;
	}

	//struct timeval tvStart, tvPref,
	//tvApply, tvFriend, tvBlock, tvCommBlock,
	//tvCreate, tvDedup,
	//tvPhotoFilter, tvStatusFilter, tvViewCountFilter, tvStarFilter,
	//tvEnd;
	//gettimeofday(&tvStart, NULL);
	//double linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);

	PreferenceDataPtr pref = PreferenceProducer::instance().create(userId, createLog);

	//gettimeofday(&tvPref, NULL);	
	//double linPrefEnd = ((double)tvPref.tv_sec * 1000000 + (double)tvPref.tv_usec);
	//double linPrefTime = linPrefEnd - linStart;
	//linPrefTime = linPrefTime/1000000;	
	//createLog	<< "(" << linPrefTime << "s)";

	std::ostringstream app_tmp;
	IntSeq applyList = OfferFriendsUtil::getApplySeq(userId);
	MCE_INFO("applyList size : " << applyList.size());

	//gettimeofday(&tvApply, NULL);	
	//double linApplyEnd = ((double)tvApply.tv_sec * 1000000 + (double)tvApply.tv_usec);
	//double linApplyTime = linApplyEnd - linPrefEnd;
	//linApplyTime = linApplyTime/1000000;	
	//createLog	<< " applyList(" << applyList.size() << ")(" << linApplyTime << "s)";

	IntSeq friendList = OfferFriendsUtil::getFriendSeq(userId);
	MCE_INFO("friendList size : " << friendList.size());

	//gettimeofday( &tvFriend, NULL );	
	//double linFriendEnd = ((double)tvFriend.tv_sec * 1000000 + (double)tvFriend.tv_usec);
	//double linFriendTime = linFriendEnd - linApplyEnd;
	//linFriendTime = linFriendTime/1000000;	
	//createLog	<< " friendList(" << friendList.size() << ")(" << linFriendTime << "s)";

  IntSeq blockList = OfferFriendsUtil::getBlockSeq(userId);
	MCE_INFO("blockList size : " << blockList.size());

	//gettimeofday(&tvBlock, NULL);	
	//double linBlockEnd = ((double)tvBlock.tv_sec * 1000000 + (double)tvBlock.tv_usec);
	//double linBlockTime = linBlockEnd - linFriendEnd;
	//linBlockTime = linBlockTime/1000000;	
	//createLog	<< " blockList(" << blockList.size() << ")(" << linBlockTime << "s)";

  IntSeq commBlockList = OfferFriendsUtil::getCommBlockSeq(userId);
	MCE_INFO("commBlockList size : " << commBlockList.size());

	//gettimeofday(&tvCommBlock, NULL);	
	//double linCommBlockEnd = ((double)tvCommBlock.tv_sec * 1000000 + (double)tvCommBlock.tv_usec);
	//double linCommBlockTime = linCommBlockEnd - linBlockEnd;
	//linCommBlockTime = linCommBlockTime/1000000;	
	//createLog	<< " commBlockList(" << commBlockList.size() << ")(" << linCommBlockTime << "s)";

	//createLog	<< " >> ";

	BackendDataPtr bak = BackendProducer::instance().create(userId, pref->getWeights(), applyList, friendList,
																												blockList, commBlockList, createLog, createDetail);

	//gettimeofday(&tvCreate, NULL);
	//double linCreateEnd = ((double)tvCreate.tv_sec * 1000000 + (double)tvCreate.tv_usec);
	//double linCreateTime = linCreateEnd - linCommBlockEnd;
	//linCreateTime = linCreateTime/1000000;	
	//createLog	<< " create(" << linCreateTime << "s)>> ";

	//make offerSource list from backendResult，calculate weight of the ids repeated in more than 1 type
	//按类型保存权重排序后的用户
	map<int, multimap<int, OfferDataN> > typedWeightMap;
	MyUtil::IntSeq ids = dedup(typedWeightMap, bak, createLog, createDetail);

	MCE_INFO("[OfferCreator] create ids size:" << ids.size());
	//gettimeofday(&tvDedup, NULL);
	//double linDedupEnd = ((double)tvDedup.tv_sec * 1000000 + (double)tvDedup.tv_usec);
	//double linDedupTime = linDedupEnd - linCreateEnd;
	//linDedupTime = linDedupTime/1000000;	
	//createLog	<< linDedupTime << "s)";

	map<int, bool> photo_map = OfferFriendsUtil::getPhotoStateBatch(ids);
	photo_filter(typedWeightMap, photo_map, createLog, createDetail);

	//gettimeofday(&tvPhotoFilter, NULL);
	//double linPhotoFilterEnd = ((double)tvPhotoFilter.tv_sec * 1000000 + (double)tvPhotoFilter.tv_usec);
	//double linPhotoFilterTime = linPhotoFilterEnd - linDedupEnd;
	//linPhotoFilterTime = linPhotoFilterTime / 1000000;
	//createLog	<< "(" << linPhotoFilterTime << "s)";

	map<int, bool> status_map = OfferFriendsUtil::getStatusBatch(ids);
	status_filter(typedWeightMap, status_map, createLog, createDetail);

	//gettimeofday( &tvStatusFilter, NULL );
	//double linStatusFilterEnd = ((double)tvStatusFilter.tv_sec*1000000 + (double)tvStatusFilter.tv_usec);
	//double linStatusFilterTime = linStatusFilterEnd - linPhotoFilterEnd;
	//linStatusFilterTime = linStatusFilterTime/1000000;
	//createLog	<< "(" << linStatusFilterTime << "s)";

	map<int, int> view_count_map = OfferFriendsUtil::getViewCountMap(ids);
	view_count_filter(typedWeightMap, view_count_map, createLog, createDetail);

	//gettimeofday( &tvViewCountFilter, NULL );
	//double linViewCountFilterEnd = ((double)tvViewCountFilter.tv_sec*1000000 + (double)tvViewCountFilter.tv_usec);
	//double linViewCountFilterTime = linViewCountFilterEnd - linStatusFilterEnd;
	//linViewCountFilterTime = linViewCountFilterTime/1000000;
	//createLog	<< "(" << linViewCountFilterTime << "s)";

	map<int,bool> star_map = OfferFriendsUtil::getStateStarBatch( ids );
	star_filter(typedWeightMap, star_map, createLog, createDetail);

	//gettimeofday( &tvStarFilter, NULL );
	//double linStarFilterEnd = ((double)tvStarFilter.tv_sec*1000000 + (double)tvStarFilter.tv_usec);
	//double linStarFilterTime = linStarFilterEnd - linViewCountFilterEnd;
	//linStarFilterTime = linStarFilterTime/1000000;
	//createLog	<< "(" << linStarFilterTime << "s)";

	OfferCacheDataNPtr offerRes = pick_out(resultSize, typedWeightMap, pref, createLog, createDetail);

	//gettimeofday( &tvEnd, NULL );
	//double linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
	//double linPickOutTime = linEnd - linStarFilterEnd;
	//linPickOutTime = linPickOutTime/1000000;
	//createLog	<< "(" << linPickOutTime << "s)";

	//double linTime = linEnd - linStart;
	//linTime = linTime/1000000;
	//createLog	<< " total(" << linTime << "s)";

	//MCE_INFO( "[OfferCreate::create] userId=" << userId << createLog.str() );
	//createRes << "userId=" << userId << createLog.str() << " details[ " << createDetail.str() << " ]";
	return offerRes;
}

OfferCacheDataNPtr OfferCreator::createHighSchoolData(int userId,
																											int resultSize,
																											ostringstream& createRes) 
{
	MCE_INFO("OfferCreator createHighSchoolData userId:" << userId << " resultSize:" << resultSize);
	ostringstream createLog;
	ostringstream createDetail;

	if (resultSize <= 0) {
		createLog << "resultSize error";
		return new OfferCacheDataN;
	}

	struct timeval tvStart, tvPref,
	tvApply, tvFriend, tvBlock, tvCommBlock,
	tvCreate, tvDedup,
	tvPhotoFilter, tvStatusFilter, tvViewCountFilter, tvStarFilter,
	tvEnd;
	gettimeofday(&tvStart, NULL);
	double linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);

	PreferenceDataPtr pref = PreferenceProducer::instance().create(userId, createLog);

	gettimeofday(&tvPref, NULL);	
	double linPrefEnd = ((double)tvPref.tv_sec * 1000000 + (double)tvPref.tv_usec);
	double linPrefTime = linPrefEnd - linStart;
	linPrefTime = linPrefTime/1000000;	
	createLog	<< "(" << linPrefTime << "s)";

	std::ostringstream app_tmp;
	IntSeq applyList = OfferFriendsUtil::getApplySeq(userId);
	MCE_INFO("applyList size : " << applyList.size());

	gettimeofday(&tvApply, NULL);	
	double linApplyEnd = ((double)tvApply.tv_sec * 1000000 + (double)tvApply.tv_usec);
	double linApplyTime = linApplyEnd - linPrefEnd;
	linApplyTime = linApplyTime/1000000;	
	createLog	<< " applyList(" << applyList.size() << ")(" << linApplyTime << "s)";

	IntSeq friendList = OfferFriendsUtil::getFriendSeq(userId);
	MCE_INFO("friendList size : " << friendList.size());

	gettimeofday( &tvFriend, NULL );	
	double linFriendEnd = ((double)tvFriend.tv_sec * 1000000 + (double)tvFriend.tv_usec);
	double linFriendTime = linFriendEnd - linApplyEnd;
	linFriendTime = linFriendTime/1000000;	
	createLog	<< " friendList(" << friendList.size() << ")(" << linFriendTime << "s)";

  IntSeq blockList = OfferFriendsUtil::getBlockSeq(userId);
	MCE_INFO("blockList size : " << blockList.size());

	gettimeofday(&tvBlock, NULL);	
	double linBlockEnd = ((double)tvBlock.tv_sec * 1000000 + (double)tvBlock.tv_usec);
	double linBlockTime = linBlockEnd - linFriendEnd;
	linBlockTime = linBlockTime/1000000;	
	createLog	<< " blockList(" << blockList.size() << ")(" << linBlockTime << "s)";

  IntSeq commBlockList = OfferFriendsUtil::getCommBlockSeq(userId);
	MCE_INFO("commBlockList size : " << commBlockList.size());

	gettimeofday(&tvCommBlock, NULL);	
	double linCommBlockEnd = ((double)tvCommBlock.tv_sec * 1000000 + (double)tvCommBlock.tv_usec);
	double linCommBlockTime = linCommBlockEnd - linBlockEnd;
	linCommBlockTime = linCommBlockTime/1000000;	
	createLog	<< " commBlockList(" << commBlockList.size() << ")(" << linCommBlockTime << "s)";

	createLog	<< " >> ";

	BackendDataPtr bak = BackendProducer::instance().createHighSchoolData(userId, pref->getWeights(), applyList, friendList,
																																				blockList, commBlockList, createLog, createDetail);

	gettimeofday(&tvCreate, NULL);
	double linCreateEnd = ((double)tvCreate.tv_sec * 1000000 + (double)tvCreate.tv_usec);
	double linCreateTime = linCreateEnd - linCommBlockEnd;
	linCreateTime = linCreateTime/1000000;	
	createLog	<< " create(" << linCreateTime << "s)>> ";

	//make offerSource list from backendResult，calculate weight of the ids repeated in more than 1 type
	//按类型保存权重排序后的用户
	map<int, multimap<int, OfferDataN> > typedWeightMap;
	MyUtil::IntSeq ids = dedup(typedWeightMap, bak, createLog, createDetail);

	MCE_INFO("[OfferCreator] createHighSchoolData ids size:" << ids.size() << "typedWeightMap size:" << typedWeightMap.size());

	gettimeofday(&tvDedup, NULL);
	double linDedupEnd = ((double)tvDedup.tv_sec * 1000000 + (double)tvDedup.tv_usec);
	double linDedupTime = linDedupEnd - linCreateEnd;
	linDedupTime = linDedupTime/1000000;	
	createLog	<< linDedupTime << "s)";

	map<int, bool> photo_map = OfferFriendsUtil::getPhotoStateBatch(ids);
	photo_filter(typedWeightMap, photo_map, createLog, createDetail);

	gettimeofday(&tvPhotoFilter, NULL);
	double linPhotoFilterEnd = ((double)tvPhotoFilter.tv_sec * 1000000 + (double)tvPhotoFilter.tv_usec);
	double linPhotoFilterTime = linPhotoFilterEnd - linDedupEnd;
	linPhotoFilterTime = linPhotoFilterTime / 1000000;
	createLog	<< "(" << linPhotoFilterTime << "s)";

	map<int, bool> status_map = OfferFriendsUtil::getStatusBatch(ids);
	status_filter(typedWeightMap, status_map, createLog, createDetail);

	gettimeofday( &tvStatusFilter, NULL );
	double linStatusFilterEnd = ((double)tvStatusFilter.tv_sec*1000000 + (double)tvStatusFilter.tv_usec);
	double linStatusFilterTime = linStatusFilterEnd - linPhotoFilterEnd;
	linStatusFilterTime = linStatusFilterTime/1000000;
	createLog	<< "(" << linStatusFilterTime << "s)";

	map<int, int> view_count_map = OfferFriendsUtil::getViewCountMap(ids);
	view_count_filter(typedWeightMap, view_count_map, createLog, createDetail);

	gettimeofday( &tvViewCountFilter, NULL );
	double linViewCountFilterEnd = ((double)tvViewCountFilter.tv_sec*1000000 + (double)tvViewCountFilter.tv_usec);
	double linViewCountFilterTime = linViewCountFilterEnd - linStatusFilterEnd;
	linViewCountFilterTime = linViewCountFilterTime/1000000;
	createLog	<< "(" << linViewCountFilterTime << "s)";

	map<int,bool> star_map = OfferFriendsUtil::getStateStarBatch( ids );
	star_filter(typedWeightMap, star_map, createLog, createDetail);

	gettimeofday( &tvStarFilter, NULL );
	double linStarFilterEnd = ((double)tvStarFilter.tv_sec*1000000 + (double)tvStarFilter.tv_usec);
	double linStarFilterTime = linStarFilterEnd - linViewCountFilterEnd;
	linStarFilterTime = linStarFilterTime/1000000;
	createLog	<< "(" << linStarFilterTime << "s)";

	OfferCacheDataNPtr offerRes = pick_out(resultSize, typedWeightMap, pref, createLog, createDetail);

	gettimeofday( &tvEnd, NULL );
	double linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
	double linPickOutTime = linEnd - linStarFilterEnd;
	linPickOutTime = linPickOutTime/1000000;
	createLog	<< "(" << linPickOutTime << "s)";

	double linTime = linEnd - linStart;
	linTime = linTime/1000000;
	createLog	<< " total(" << linTime << "s)";

	MCE_INFO( "[OfferCreate::create] userId=" << userId << createLog.str() );
	createRes << "userId=" << userId << createLog.str() << " details[ " << createDetail.str() << " ]";
	return offerRes;
}

MyUtil::IntSeq OfferCreator::dedup(std::map<int, std::multimap<int, OfferDataN> >& typedWeightMap, 
										 							 const BackendDataPtr& bak, 
										 							 ostringstream& createLog, 
										 							 ostringstream& createDetail)
{
	struct timeval tvStart, tvStage1, tvEnd;
	gettimeofday( &tvStart, NULL );
	double linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);

	map<int, map<int, OfferDataN> > typedIdMap;				//map<type,map<id,offer>>;
	typedIdMap.insert(make_pair<int, map<int, OfferDataN> >(BaseTypeMix, map<int, OfferDataN>()));		//type:BaseTypeMix
	map<int, OfferDataN>& mixIdMap = typedIdMap[BaseTypeMix];		//&

	for (std::map<int, OfferDataNSeq>::iterator it = bak->_typedOfferDataNSeq.begin(); 
			it != bak->_typedOfferDataNSeq.end(); ++it) {				//it: <type,OfferDataNSeq> in bak
		//new <it->first,map<id,OfferDataN> > in typedIdMap
		typedIdMap.insert(make_pair<int, map<int, OfferDataN> >(it->first, map<int, OfferDataN>()));
		for (OfferDataNSeq::iterator it2 = it->second.begin(); 
				it2 != it->second.end(); ++it2) {				//it2: OfferDataN in bak
			map<int, OfferDataN>::iterator it3 = mixIdMap.find(it2->userId);			//it3: <id,OfferDataN> in mixIdMap
			if( it3 != mixIdMap.end() ){
				it3->second.weight += it2->weight + 10;
				it3->second.sign |= it2->sign;
				continue;
			}
			bool found = false;
			for (map<int, map<int, OfferDataN> >::iterator it4 = typedIdMap.begin(); 
					it4 != typedIdMap.end(); ++it4) {
				if (it4->first == BaseTypeMix) {
					continue;			//it4: <type,map<id,OfferDataN> > in typedIdMap without BaseTypeMix
				}
				map<int, OfferDataN>::iterator it5 = it4->second.find( it2->userId );//it5: <int,OfferDataN> in it4->second
				if (it5 != it4->second.end()) {
					it5->second.weight += it2->weight + 10;
					//it5->second.infos.insert( it2->infos.begin(), it2->infos.end() );
					it5->second.sign |= it2->sign;
					mixIdMap[it2->userId] = it5->second;
					it4->second.erase(it5);
					found = true;
					break;
				}
			}
			if (!found) {
				typedIdMap[it->first].insert(make_pair<int, OfferDataN>(it2->userId, *it2));
			}
		}
	}
	gettimeofday(&tvStage1, NULL);
	double linStage1 = ((double)tvStage1.tv_sec * 1000000 + (double)tvStage1.tv_usec);
	double linStage1Time = linStage1 - linStart;
	linStage1Time = linStage1Time / 1000000;
	createLog	<< " dedup(" << linStage1Time << "s/";

	MyUtil::IntSeq ids;
	for (map<int, map<int, OfferDataN> >::iterator it4 = typedIdMap.begin(); 
			it4 != typedIdMap.end(); ++it4 ) {
		typedWeightMap.insert(make_pair<int, multimap<int, OfferDataN> >(it4->first, multimap<int, OfferDataN>()));
		for (map<int, OfferDataN>::iterator it5 = it4->second.begin(); 
				it5 != it4->second.end(); ++it5) {
			typedWeightMap[it4->first].insert(multimap<int, OfferDataN>::value_type(it5->second.weight, it5->second));
			ids.push_back(it5->second.userId);
		}
	}

	gettimeofday(&tvEnd, NULL);
	double linEnd = ((double)tvEnd.tv_sec * 1000000 + (double)tvEnd.tv_usec);
	double linEndTime = linEnd - linStage1;
	linEndTime = linEndTime / 1000000;
	createLog	<< linEndTime << "s=";

	return ids;
}

void OfferCreator::photo_filter(map<int, multimap<int, OfferDataN> >& typedWeightMap, 
																const map<int, bool>& photo_map, 
																ostringstream& createLog, 
																ostringstream& createDetail)
{
	createLog	<< " photofilter(";
	createDetail << " photofilt_";
	for (map<int, multimap<int, OfferDataN> >::iterator it = typedWeightMap.begin(); 
			it != typedWeightMap.end(); ++it) {
		int filter_count = 0;
		multimap<int, OfferDataN> tmp;
		for (multimap<int, OfferDataN>::iterator iter = it->second.begin(); 
				iter != it->second.end(); ++iter) {
			map<int, bool>::const_iterator itMap = photo_map.find(iter->second.userId);
			if (itMap==photo_map.end() || itMap->second) {
				tmp.insert(multimap<int, OfferDataN>::value_type(iter->first, iter->second));
			} else {
				++filter_count;
				createDetail << iter->second.userId << "_";
			}
		}
		createLog << "t" << it->first << "-" << filter_count << "=" << tmp.size() << "/";
		tmp.swap(it->second);
	}
	createLog	<< ")";
}

void OfferCreator::status_filter(map<int, multimap<int, OfferDataN> >& typedWeightMap, 
									 							 const map<int, bool>& status_map, 
									 							 ostringstream& createLog, 
									 							 ostringstream& createDetail)
{
	createLog	<< " statusfilter(";
	createDetail << " statusfilt_";
	for(map<int,multimap<int,OfferDataN> >::iterator it = typedWeightMap.begin(); it != typedWeightMap.end(); ++it ){
		int filter_count = 0;
		multimap<int,OfferDataN> tmp;
		for(multimap<int,OfferDataN>::iterator iter=it->second.begin(); iter!=it->second.end(); ++iter){
			map<int,bool>::const_iterator itMap = status_map.find( iter->second.userId );
			if(itMap==status_map.end() || !itMap->second){
				tmp.insert( multimap<int,OfferDataN>::value_type( iter->first, iter->second ) );
			} else{
				++filter_count;
				createDetail << iter->second.userId << "_";
			}
		}
		createLog << "t" << it->first << "-" << filter_count << "=" << tmp.size() << "/";
		tmp.swap( it->second );
	}
	createLog	<< ")";
}

void OfferCreator::view_count_filter( map<int,multimap<int,OfferDataN> >& typedWeightMap,
		const map<int,int>& view_count_map, ostringstream& createLog, ostringstream& createDetail ) {
	createLog	<< " viewcountfilter(";
	if( view_count_map.size() < 50 ){
		createLog << "total=" << view_count_map.size() << "<50)";
		return;
	}
	createDetail << " viewfilt_";
	for(map<int,multimap<int,OfferDataN> >::iterator it = typedWeightMap.begin(); it != typedWeightMap.end(); ++it ){
		int filter_count = 0;
		multimap<int,OfferDataN> tmp;
		for(multimap<int,OfferDataN>::iterator iter=it->second.begin(); iter!=it->second.end(); ++iter){
			map<int,int>::const_iterator itMap = view_count_map.find( iter->second.userId );
			if( itMap==view_count_map.end() ){
				tmp.insert( multimap<int,OfferDataN>::value_type( iter->second.weight, iter->second ) );
			} else if( (itMap->second>=50) && (itMap->second<500) ){
				iter->second.weight += 5;
				tmp.insert( multimap<int,OfferDataN>::value_type( iter->second.weight, iter->second ) );
			} else if( (itMap->second>=500) && (itMap->second<2000) ){
				iter->second.weight += 10;
				tmp.insert( multimap<int,OfferDataN>::value_type( iter->second.weight, iter->second ) );
			} else if( itMap->second>=2000 ){
				iter->second.weight += 20;
				tmp.insert( multimap<int,OfferDataN>::value_type( iter->second.weight, iter->second ) );
			} else{
				++filter_count;
				createDetail << iter->second.userId << "_";
			}
		}
		createLog << "t" << it->first << "-" << filter_count << "=" << tmp.size() << "/";
		tmp.swap( it->second );
	}
	createLog	<< ")";
}

void OfferCreator::star_filter(map<int, multimap<int, OfferDataN> >& typedWeightMap, 
								 							 const map<int,bool>& star_map, 
								 							 ostringstream& createLog, 
								 							 ostringstream& createDetail)
{
	createLog	<< " starfilter(";
	createDetail << " starfilt_";
	for(map<int,multimap<int,OfferDataN> >::iterator it = typedWeightMap.begin(); it != typedWeightMap.end(); ++it ){
		int star_count = 0;
		multimap<int,OfferDataN> tmp;
		for(multimap<int,OfferDataN>::iterator iter=it->second.begin(); iter!=it->second.end(); ++iter){
			map<int,bool>::const_iterator itMap = star_map.find( iter->second.userId );
			if(itMap!=star_map.end() && itMap->second){
				iter->second.weight += 20;
				tmp.insert( multimap<int,OfferDataN>::value_type( iter->second.weight, iter->second ) );
				++star_count;
				createDetail << iter->second.userId << "_";
			} else{
				tmp.insert( multimap<int,OfferDataN>::value_type( iter->second.weight, iter->second ) );
			}
		}
		createLog << "t" << it->first << "+" << star_count << "/";
		tmp.swap( it->second );
	}
	createLog	<< ")";
}

//**************************************************************************************************************

OfferCacheDataNPtr OfferCreator::pick_out(int resultSize, map<int, multimap<int, OfferDataN> >& typedWeightMap,
															PreferenceDataPtr& pref, ostringstream& createLog, ostringstream& createDetail) 
{
	createLog	<< " pickout(";
	createDetail << " pickout_";

	OfferCacheDataNPtr offerRes = new OfferCacheDataN();
	////pop and order offers for each type, meanwhile check if they exist in _ownerId's friendlist
	multiset<OfferDataN, less_offer> bestOffers;
	multiset<OfferDataN, less_offer> offers;

	multimap<int, OfferDataN>& mixWeightMap = typedWeightMap[BaseTypeMix];
	for (map<int, OfferDataN>::reverse_iterator bestIt = mixWeightMap.rbegin(); 
			bestIt != mixWeightMap.rend(); ++bestIt) {
		bestOffers.insert(bestIt->second);
		createDetail << bestIt->second.userId << "_";
		if( (--resultSize)==0 ) break;
	}
	MCE_INFO("OfferCreator bestOffers size : " << bestOffers.size());

	createLog	<< "best:" << bestOffers.size();
	map<int, int> quotas = pref->getQuotasFrom(resultSize, typedWeightMap);
	for (map<int, int>::iterator iter = quotas.begin();
			iter != quotas.end(); ++iter) {
		//MCE_INFO("[OfferCreator] " << iter->first << " : " << iter->second);
	}
	for( map<int,int>::iterator typeIt = quotas.begin(); typeIt != quotas.end(); ++typeIt ) {
		multimap<int, OfferDataN>& typedWeightItem = typedWeightMap[ typeIt->first ];
		multimap<int, OfferDataN>::reverse_iterator offerIt = typedWeightItem.rbegin();
		createLog	<< "/" << "t" << typeIt->first << "-" << typedWeightItem.size() << "-" << typeIt->second;
		while( typeIt->second > 0 && offerIt != typedWeightItem.rend() ) {
			offers.insert(offerIt->second);
			++offerIt;
			--(typeIt->second);
		}
	}
	MCE_INFO("OfferCreator offers size : " << offers.size());

	for (multiset<OfferDataN, less_offer>::const_iterator it = bestOffers.begin(); it
			!= bestOffers.end(); ++it) {
		offerRes->bestOffers.push_back(*it);
	}

	for (multiset<OfferDataN, less_offer>::const_iterator it = offers.begin(); it
			!= offers.end(); ++it) {
		offerRes->offers.push_back(*it);
	}

	createLog	<< ")";
	return offerRes;
}

