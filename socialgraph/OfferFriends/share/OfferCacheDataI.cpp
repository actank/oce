#include "OfferCacheDataI.h"

using namespace std;
using namespace MyUtil;
using namespace xce::offerfriends;

//***************************************************************************

OfferCacheDataI::OfferCacheDataI() :
	_timeStamp((int)time(NULL)) {
}

OfferCacheDataI::OfferCacheDataI( const OfferCacheDataNPtr& ocdp ) :
	_timeStamp((int)time(NULL)) {
	if( ocdp ){
		OfferDataNSeq(ocdp->bestOffers).swap( bestOffers );
		OfferDataNSeq(ocdp->offers).swap( offers );
	}
}

MyUtil::IntSeq OfferCacheDataI::getIds(){
	MyUtil::IntSeq res;
	IceUtil::Mutex::Lock lock(_mutex);
	for(OfferDataNSeq::iterator iter=bestOffers.begin(); iter!=bestOffers.end(); ++iter){
		res.push_back(iter->userId);
	}
	for(OfferDataNSeq::iterator iter=offers.begin(); iter!=offers.end(); ++iter){
		res.push_back(iter->userId);
	}
	return res;
}

OfferDataNSeq OfferCacheDataI::getFast(GetInfo& info,int limit) {
	info.hit = true;
	info.limit = limit;
	info.bestCount = bestOffers.size();
	info.offerCount = offers.size();
	OfferDataNSeq orderedOffers;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		if( limit < 0 || (int)(bestOffers.size()+offers.size()) <= limit ) {
			orderedOffers = bestOffers;
			orderedOffers.insert( orderedOffers.end(), offers.begin(), offers.end() );
			info.bestRes = bestOffers.size();
			info.onlineRes = 0;
			info.offerRes = offers.size();
		} else if( limit <= (int)bestOffers.size() ) {
			orderedOffers = OfferDataNSeq( bestOffers.begin(), bestOffers.begin()+limit );
			info.bestRes = limit;
			info.onlineRes = 0;
			info.offerRes = 0;
		} else{ // bestOffers.size() < limit < bestOffers.size()+offers.size()
			orderedOffers = bestOffers;
			orderedOffers.insert( orderedOffers.end(), offers.begin(), offers.begin()+(limit-(int)bestOffers.size()) );
			info.bestRes = bestOffers.size();
			info.onlineRes = 0;
			info.offerRes = (limit-(int)bestOffers.size());
		}
	}
	info.timeout = (long)((int)time(NULL)-_timeStamp);
	return orderedOffers;
}

OfferDataNSeq OfferCacheDataI::getBest(GetInfo& info, const std::map<int,bool>& onlinestates, int limit) {
	info.hit = true;
	info.limit = limit;
	info.bestCount = bestOffers.size();
	info.offerCount = offers.size();
	OfferDataNSeq orderedOffers;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		if( limit < 0 || (int)(bestOffers.size()+offers.size()) <= limit ) {
			orderedOffers = bestOffers;
			info.bestRes = bestOffers.size();
			OfferDataNSeq tmp = doOnlineFilter( info,offers,offers.size(),onlinestates );
			orderedOffers.insert( orderedOffers.end(), tmp.begin(), tmp.end() );
		} else if( limit <= (int)bestOffers.size() ) {
			orderedOffers = OfferDataNSeq( bestOffers.begin(), bestOffers.begin()+limit );
			info.bestRes = limit;
			info.onlineRes = 0;
			info.offerRes = 0;
		} else{ // bestOffers.size() < limit < bestOffers.size()+offers.size()
			orderedOffers = bestOffers;
			OfferDataNSeq tmp = doOnlineFilter( info,offers,limit-(int)bestOffers.size(),onlinestates );
			orderedOffers.insert( orderedOffers.end(), tmp.begin(), tmp.end() );
			info.bestRes = bestOffers.size();
		}
	}
	info.timeout = (long)((int)time(NULL)-_timeStamp);
	return orderedOffers;
}

OfferDataNSeq OfferCacheDataI::getRandom(GetInfo& info, int limit) {
	info.hit = true;
	info.limit = limit;
	info.bestCount = bestOffers.size();
	info.offerCount = offers.size();
	OfferDataNSeq orderedOffers;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		if( limit < 0 || (int)(bestOffers.size()+offers.size()) <= limit ) {
			orderedOffers = doRandom( bestOffers, bestOffers.size() );
			OfferDataNSeq tmp = doRandom( offers, offers.size() );
			orderedOffers.insert( orderedOffers.end(), tmp.begin(), tmp.end() );
			info.bestRes = bestOffers.size();
			info.onlineRes = 0;
			info.offerRes = tmp.size();
		} else if( limit <= (int)bestOffers.size() ) {
			orderedOffers = doRandom( bestOffers, limit );
			info.bestRes = limit;
			info.onlineRes = 0;
			info.offerRes = 0;
		} else{ // bestOffers.size() < limit < bestOffers.size()+offers.size()
			orderedOffers = doRandom( bestOffers, bestOffers.size() );
			OfferDataNSeq tmp = doRandom( offers, limit-(int)bestOffers.size() );
			orderedOffers.insert( orderedOffers.end(), tmp.begin(), tmp.end() );
			info.bestRes = bestOffers.size();
			info.onlineRes = 0;
			info.offerRes = tmp.size();
		}
	}
	info.timeout = (long)((int)time(NULL)-_timeStamp);
	return orderedOffers;
}

OfferDataNSeq OfferCacheDataI::doRandom( const OfferDataNSeq& sourceOffers, int limit ){
	OfferDataNSeq ret;
	int value_sum = 0;                                //随机取好友
	vector<int> posSequence;
	vector<bool> hitSequence;
	for(OfferDataNSeq::const_iterator iter=sourceOffers.begin(); iter!=sourceOffers.end(); ++iter){
		value_sum += iter->weight;
		posSequence.push_back(iter->weight);
		hitSequence.push_back(false);
	}

	if( sourceOffers.empty() || value_sum <= 0 || limit <=0 ){
		return ret;
	}

	int loop_count = (int)sourceOffers.size() <= limit ? (int)sourceOffers.size() : limit;
	while( --loop_count >= 0 ){
		int randValue = rand() % value_sum;
		int value_plus = 0;
		for(size_t pos=0; pos<posSequence.size(); ++pos){
			if( hitSequence[pos] ){
				continue;
			}
			value_plus += posSequence[pos];
			if( value_plus - randValue >= 0 ) {
				hitSequence[pos] = true;
				value_sum -= posSequence[pos];
				ret.push_back( sourceOffers[pos] );
				break;
			}
		}
	}
	return ret;
}

OfferDataNSeq OfferCacheDataI::doOnlineFilter( GetInfo& info,const OfferDataNSeq& sourceOffers,int limit,const std::map<int,bool>& onlinestates ){
	if( sourceOffers.empty() || limit <= 0 ){
		return sourceOffers;
	}
	OfferDataNSeq ret_online;
	OfferDataNSeq ret_offline;
	for(OfferDataNSeq::const_iterator iter=sourceOffers.begin(); iter!=sourceOffers.end(); ++iter){
		map<int,bool>::const_iterator it = onlinestates.find( iter->userId );
		if( it != onlinestates.end() && it->second ){
			ret_online.push_back( *iter );
		} else{
			ret_offline.push_back( *iter );
		}
	}
	OfferDataNSeq ret;
	for(OfferDataNSeq::const_iterator iter=ret_online.begin(); iter!=ret_online.end(); ++iter){
		ret.push_back(*iter);
		if( --limit <= 0 ) break;
	}
	info.onlineRes= ret.size();
	if( limit <= 0 ){
		info.offerRes = 0;
		return ret;
	} 
	ret_offline = doRandom( ret_offline,ret_offline.size() );
	int count = 0;
	for( OfferDataNSeq::const_iterator iter=ret_offline.begin(); iter!=ret_offline.end(); ++iter ){
		ret.push_back(*iter);
		if( ++count == limit ) break;
	}
	info.offerRes = count;
	return ret;
}

void OfferCacheDataI::remove( RmInfo& info,int userId ){
	info.hit = true;
	int timenow = (int)time(NULL);
	IceUtil::Mutex::Lock lock(_mutex);
	OfferDataNSeq::iterator it1=bestOffers.begin();
	for(; it1!=bestOffers.end(); ++it1){
		if(it1->userId == userId){
			info.found = true;
			info.data = *it1;
			bestOffers.erase(it1);
			break;
		}
	}
	if( it1 != bestOffers.end() ){
		info.bestCount = bestOffers.size();
		info.offerCount = offers.size();
		info.timeout = (long)(timenow-_timeStamp);
		_timeStamp = timenow;
		return;
	}
	OfferDataNSeq::iterator it2=offers.begin();
	for(; it2!=offers.end(); ++it2){
		if(it2->userId == userId){
			info.found = true;
			info.data = *it2;
			offers.erase(it2);
			break;
		}
	}
	info.bestCount = bestOffers.size();
	info.offerCount = offers.size();
	info.timeout = (long)(timenow-_timeStamp);
	if( it2 != offers.end() ){
		_timeStamp = timenow;
	}
}

