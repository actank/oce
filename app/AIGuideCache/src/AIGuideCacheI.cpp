#include "AIGuideCacheI.h"

using namespace std;
using namespace Ice;
using namespace xce::aiguidecache;

void MyUtil::initialize()  {

	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&AIGuideCacheManagerI::instance(), service.createIdentity("M", ""));
}

int Bits32::get(int pos) {

	if ( (pos < 0) || (pos >= 32) ) {
		return 0;
	}

	return _data[pos];
}

void Bits32::set(int pos, bool value) {

	if ( (pos < 0) || (pos >= 32) ) {
		return;
	}

	// IceUtil::Mutex::Lock lock(*this);	
	if ( value ) {
		_data.set(pos);
	}
	else {
		_data.reset(pos);
	}
}

int BitsArray::get(int pos) {
	
	if ( (pos < 0) || (pos >= _length) ) {
		MCE_WARN("BitsArray : out of bound - " << pos);
		return 0;
	}

	int pos1 = pos / 32;
	int pos2 = pos % 32;

	return _data.at(pos1).get(pos2);
}

void BitsArray::set(int pos, bool value) {

	if ( (pos < 0) || (pos >= _length) ) {
		MCE_WARN("BitsArray : out of bound - " << pos);
		return ;
	}

	int pos1 = pos / 32;
	int pos2 = pos % 32;

	_data.at(pos1).set(pos2, value);

	MCE_DEBUG("[BitsArray::set] " << pos << "," << pos1 << "," << pos2 << " " << value);
}

void AIGuideCacheManagerI::set(::Ice::Int userId,
                                             ::Ice::Int appId,
                                             bool status,
                                             const Ice::Current& current)
{
	if ( userId <= 0 ) {
		MCE_WARN("Invalid User Id : " << userId);
		return;
	}

	switch (appId)  {
	case FARMGAME :
		_arrayFarm.set(userId, status);
		MCE_DEBUG("[AIGuideCache] set " << userId << ", FARMGAME," << status);
		break;
	case DOG :
		_arrayDog.set(userId, status);
		MCE_DEBUG("[AIGuideCache] set " << userId << ", DOG," << status);
		break;
	case CARPARK :
		_arrayCarpark.set(userId, status);		
		MCE_DEBUG("[AIGuideCache] set " << userId << ", CARPARK," << status);
		break;
	case GODFATHER :
		_arrayGod.set(userId, status);
		MCE_DEBUG("[AIGuideCache] set " << userId << ", GODFATHER," << status);
		break;
	case TYCOON :
		_arrayTycoon.set(userId, status);
		MCE_DEBUG("[AIGuideCache] set " << userId << ", TYCOON," << status);
		break;
	default:
		MCE_WARN("[AIGuideCache::set] " << userId << ", Invalid App " << appId);
	}
	// 如果是置为true, 则清除其他位
	if ( status ) {
		for ( int i = 0; i < APPMAX; i++ ) {
			if ( i != appId ) {
				set(userId, i, false);
				// _arrays[i].set(userId, false);				
			}
		}			
	}

	MCE_DEBUG("[AIGuideCache] set " << userId << " : " << appId << " : "  << status);
}

bool AIGuideCacheManagerI::get(::Ice::Int userId,
                                             ::Ice::Int appId,
                                             const Ice::Current& current)
{
	if ( userId <= 0 ) {
		MCE_WARN("Invalid User Id : " << userId);
		return false;
	}

	int thebit = 0;

	switch (appId)  {
	case FARMGAME :
		thebit = _arrayFarm.get(userId);
		break;
	case DOG :
		thebit = _arrayDog.get(userId);
		break;
	case CARPARK :
		thebit = _arrayCarpark.get(userId);
		break;
	case GODFATHER :
		thebit = _arrayGod.get(userId);
		break;
	case TYCOON :
		thebit = _arrayTycoon.get(userId);
		break;
	default:
		MCE_WARN("[AIGuideCache::get] " << userId << ", Invalid App " << appId);
		return false;
	}

	if ( thebit > 0 ) {
		return true; 
	} else {
		return false;
	}
}
