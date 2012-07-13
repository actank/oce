#include "newcomercachedataI.h"
#include "newcomerutil.h"
#include "item.pb.h"
#include <algorithm>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace MyUtil;
using namespace xce::socialgraph;
using namespace xce::socialgraph::friendfindernewcomer;

//***************************************************************************

bool CompNewComerMcData::operator() (const NewComerMcData& comer1, const NewComerMcData& comer2) {
	return comer1.time > comer2.time;
}

bool CompNewComerDbData::operator() (const NewComerDbData& comer1, const NewComerDbData& comer2) {
	return comer1.time > comer2.time;
}

NewComerCacheDataI::NewComerCacheDataI(){
}

NewComerCacheDataI::NewComerCacheDataI( const NewComerCacheDataPtr& ncdp ){
	if( ncdp ){
		CompNewComerDbData comp;
		for( NewComerDbDataMap::iterator it = ncdp->newsDbMap.begin(); it != ncdp->newsDbMap.end(); ++it ){
			newsDbMap[it->first] = NewComerDbDataSeq( it->second );
			sort(newsDbMap[it->first].begin(), newsDbMap[it->first].end(), comp);
		}
	}
}

NewComerCacheDataI::NewComerCacheDataI(const NewComerDbDataMap& db_objs){
	CompNewComerDbData comp;
	for( NewComerDbDataMap::const_iterator it = db_objs.begin(); it != db_objs.end(); ++it ){
		newsDbMap[it->first] = NewComerDbDataSeq( it->second );
		sort(newsDbMap[it->first].begin(), newsDbMap[it->first].end(), comp);
	}
}

NewComerCacheDataI::NewComerCacheDataI(vector<unsigned char>& mc_objs){
	boost::shared_ptr<Items> items(new Items());
	items->ParseFromArray(mc_objs.data(), mc_objs.size());

	NewComerDbDataMap db_objs;
	for (int i = 0; i < items->items_size(); ++i) {
		const Item& item = items->items(i);
		if( item.fields_size()>=2 ){
			NewComerDbData data;
			data.userId = item.id();
			data.time = item.fields(0);
			db_objs[item.fields(1)].push_back( data );
		}
	}

	CompNewComerDbData comp;
	for( NewComerDbDataMap::iterator it = db_objs.begin(); it != db_objs.end(); ++it ){
		sort(it->second.begin(), it->second.end(), comp);
		NewComerDbDataSeq( it->second ).swap( newsDbMap[it->first] );
	}
}

vector<unsigned char> NewComerCacheDataI::getBinData(int userId,MyUtil::Int2IntSeqMap& to_remove ){
        struct tm *tm_local_now;
        time_t now = time(NULL);
        tm_local_now = localtime(&now);
        tm_local_now->tm_hour = 0;
        tm_local_now->tm_min = 0;
        tm_local_now->tm_sec = 0;
        time_t time_zero = timelocal(tm_local_now);
        int time_deadline = time_zero-DAYS_LIMIT*86400;

	NewComerMcDataSeq res;
	for( NewComerDbDataMap::iterator itMap = newsDbMap.begin(); itMap != newsDbMap.end(); ++itMap ){
		string schoolName = NewComerUtil::getSchoolName(userId,itMap->first);
        	for( NewComerDbDataSeq::iterator itSeq = itMap->second.begin(); itSeq != itMap->second.end(); ++itSeq ){
			NewComerMcData data;
			data.userId = itSeq->userId;
			data.time = itSeq->time;
			data.type = itMap->first;
			data.schoolName = schoolName;
			res.push_back( data );
		}
	}

	CompNewComerMcData comp;
	sort(res.begin(), res.end(), comp);

	int to_limit = (int)res.size()>=DATA_LIMIT ? DATA_LIMIT : (int)res.size();

        Items items;
        for( NewComerMcDataSeq::iterator it = res.begin(); it != res.end(); ++it ){
		if( to_limit-- > 0 && it->time > time_deadline ){
			Item* item_newcomer = items.add_items();
			item_newcomer->set_id(it->userId);
			item_newcomer->add_fields(it->time);
			item_newcomer->add_fields(it->type);
			item_newcomer->set_message(it->schoolName);
		} else{
			to_remove[ it->type ].push_back( it->userId );
		}
        }

        unsigned char* chs = new unsigned char[items.ByteSize()];
        items.SerializeToArray(chs,items.ByteSize());
        vector<unsigned char> binary_data;
        for (int i = 0; i < items.ByteSize(); ++i) {
                binary_data.push_back(chs[i]);
        }
        delete[] chs;
	return binary_data;
}

vector<unsigned char> NewComerCacheDataI::add(int host,int type,NewComerDbData& guest,MyUtil::Int2IntSeqMap& to_remove ){
        struct tm *tm_local_now;
        time_t now = time(NULL);
        tm_local_now = localtime(&now);
        tm_local_now->tm_hour = 0;
        tm_local_now->tm_min = 0;
        tm_local_now->tm_sec = 0;
        time_t time_zero = timelocal(tm_local_now);
        int time_deadline = time_zero-DAYS_LIMIT*86400;

	if( time_deadline > guest.time ){
		MCE_WARN("[NewComerCacheDataI::add] add " << guest.userId << "(" << guest.time << ") to (type:" << type << ")" << host << " failed: over time" );
		return vector<unsigned char>();
	}

	ostringstream tr;
	NewComerMcDataSeq res;
	bool hit = false;
	for( NewComerDbDataMap::iterator itMap = newsDbMap.begin(); itMap != newsDbMap.end(); ++itMap ){
		string schoolName = NewComerUtil::getSchoolName(host,itMap->first);
		tr << "|" << schoolName;
		if( itMap->first == type ){
			tr << "|hittype";
			hit = true;
        		for( NewComerDbDataSeq::iterator itSeq = itMap->second.begin(); itSeq != itMap->second.end(); ++itSeq ){
				if( itSeq->userId == guest.userId ){
					tr << "|dup";
					return vector<unsigned char>();
				}
			}
			itMap->second.push_back(guest);
			CompNewComerDbData comp;
			sort(itMap->second.begin(), itMap->second.end(), comp);
			NewComerDbDataSeq( itMap->second ).swap( itMap->second );
			tr << "|" << itMap->second.size()-1 << "+1";
		} else{
			tr << "|nottype";
		}
        	for( NewComerDbDataSeq::iterator itSeq = itMap->second.begin(); itSeq != itMap->second.end(); ++itSeq ){
			NewComerMcData data;
			data.userId = itSeq->userId;
			data.time = itSeq->time;
			data.type = itMap->first;
			data.schoolName = schoolName;
			res.push_back( data );
		}
	}
	if( !hit ){
		newsDbMap[type].push_back(guest);
		string schoolName = NewComerUtil::getSchoolName(host,type);
		tr << "|" << schoolName << "|addtype";
		NewComerMcData data;
		data.userId = guest.userId;
		data.time = guest.time;
		data.type = type;
		data.schoolName = schoolName;
		res.push_back( data );
	}

	CompNewComerMcData comp;
	sort(res.begin(), res.end(), comp);
	tr << "|total_" << res.size();

	int to_limit = (int)res.size()>=DATA_LIMIT ? DATA_LIMIT : (int)res.size();

        Items items;
        for( NewComerMcDataSeq::iterator it = res.begin(); it != res.end(); ++it ){
		if( to_limit-- > 0 && it->time > time_deadline ){
			Item* item_newcomer = items.add_items();
			item_newcomer->set_id(it->userId);
			item_newcomer->add_fields(it->time);
			item_newcomer->add_fields(it->type);
			item_newcomer->set_message(it->schoolName);
		} else{
			to_remove[ it->type ].push_back( it->userId );
		}
        }

        unsigned char* chs = new unsigned char[items.ByteSize()];
        items.SerializeToArray(chs,items.ByteSize());
        vector<unsigned char> binary_data;
        for (int i = 0; i < items.ByteSize(); ++i) {
                binary_data.push_back(chs[i]);
        }
        delete[] chs;
	tr << "|remove_" << to_remove.size() << "|newmc_" << binary_data.size();
	MCE_INFO( tr.str() );
	return binary_data;
}

vector<unsigned char> NewComerCacheDataI::remove( RmInfo& info,int userId ){

	info.hit = true;

	NewComerMcDataSeq res;
	for( NewComerDbDataMap::iterator itMap = newsDbMap.begin(); itMap != newsDbMap.end(); ++itMap ){
		string schoolName = NewComerUtil::getSchoolName(userId,itMap->first);
		NewComerDbDataSeq::iterator iterTobeRemove = itMap->second.end();
		for(NewComerDbDataSeq::iterator itSeq=itMap->second.begin(); itSeq!=itMap->second.end(); ++itSeq){
			if( itSeq->userId == userId ){
				info.found = true;
				iterTobeRemove = itSeq;
				info.types.push_back( itMap->first );
			} else{
				NewComerMcData data;
				data.userId = itSeq->userId;
				data.time = itSeq->time;
				data.type = itMap->first;
				data.schoolName = schoolName;
				res.push_back( data );
			}
		}
		if( iterTobeRemove != itMap->second.end() ){
			itMap->second.erase(iterTobeRemove);
		}
	}

        Items items;
        for( NewComerMcDataSeq::iterator it = res.begin(); it != res.end(); ++it ){
		Item* item_newcomer = items.add_items();
		item_newcomer->set_id(it->userId);
		item_newcomer->add_fields(it->time);
		item_newcomer->add_fields(it->type);
		item_newcomer->set_message(it->schoolName);
        }

        unsigned char* chs = new unsigned char[items.ByteSize()];
        items.SerializeToArray(chs,items.ByteSize());
        vector<unsigned char> binary_data;
        for (int i = 0; i < items.ByteSize(); ++i) {
                binary_data.push_back(chs[i]);
        }
        delete[] chs;
	return binary_data;
}

MyUtil::IntSeq NewComerCacheDataI::getIds(){
	MyUtil::IntSeq res;
	IceUtil::Mutex::Lock lock(_mutex);
	for( NewComerDbDataMap::iterator itMap = newsDbMap.begin(); itMap != newsDbMap.end(); ++itMap ){
		for(NewComerDbDataSeq::iterator itSeq=itMap->second.begin(); itSeq!=itMap->second.end(); ++itSeq){
			res.push_back(itSeq->userId);
		}
	}
	return res;
}

NewComerDbDataMap NewComerCacheDataI::getAll() {
	IceUtil::Mutex::Lock lock(_mutex);
	return newsDbMap;
}

NewComerDbDataMap NewComerCacheDataI::getBest(const std::map<int,bool>& onlinestates, int inDays, int limit) {
        struct tm *tm_local_now;
        time_t now = time(NULL);
        tm_local_now = localtime(&now);
        tm_local_now->tm_hour = 0;
        tm_local_now->tm_min = 0;
        tm_local_now->tm_sec = 0;
        time_t time_zero = timelocal(tm_local_now);
        int time_limit = time_zero-inDays*86400;
	/*
        int time_deadline = time_zero-DAYS_LIMIT*86400;
	*/

	NewComerDbDataMap res;
	{
		IceUtil::Mutex::Lock lock(_mutex);

		int type_tmp;
		int online_count_tmp;
		NewComerDbDataSeq data_tmp;

		int type_res = 0;
		int online_count_res = 0;
		NewComerDbDataSeq data_res;

		for( NewComerDbDataMap::iterator itMap = newsDbMap.begin(); itMap != newsDbMap.end(); ++itMap ){
        		NewComerDbDataSeq timedSeq;
			for( NewComerDbDataSeq::iterator itSeq=itMap->second.begin(); itSeq!=itMap->second.end(); ++itSeq ){
				if( time_limit < itSeq->time ){
					timedSeq = NewComerDbDataSeq( itSeq, itMap->second.end() );
					break;
				}
			}

			type_tmp = itMap->first;
			data_tmp = doOnlineFilter( online_count_tmp, timedSeq, limit, onlinestates );
			if( online_count_tmp>online_count_res ){
				type_res = type_tmp;
				online_count_res = online_count_tmp;
				data_res.swap( data_tmp );
			}
		}

		if( !type_res ){
			return res;
		}

		if( limit<0 || (int)data_res.size()<=limit ) {
			res[type_res] = data_res;
		} else{
			res[type_res] = NewComerDbDataSeq( data_res.begin(), data_res.begin()+limit );
		}
	}

	return res;
}

NewComerDbDataMap NewComerCacheDataI::getRandom(int inDays, int limit) {
        struct tm *tm_local_now;
        time_t now = time(NULL);
        tm_local_now = localtime(&now);
        tm_local_now->tm_hour = 0;
        tm_local_now->tm_min = 0;
        tm_local_now->tm_sec = 0;
        time_t time_zero = timelocal(tm_local_now);
        int time_limit = time_zero-inDays*86400;
	/*
        int time_deadline = time_zero-DAYS_LIMIT*86400;
	*/

	NewComerDbDataMap res;
	{
		IceUtil::Mutex::Lock lock(_mutex);

		int randValue = rand() % newsDbMap.size();
		NewComerDbDataMap::iterator itMap = newsDbMap.begin();
		for( int i=0; i<randValue; ++i ){
			++itMap;
		}

        	NewComerDbDataSeq timedSeq;
		for( NewComerDbDataSeq::iterator itSeq=itMap->second.begin(); itSeq!=itMap->second.end(); ++itSeq ){
			if( time_limit < itSeq->time ){
				timedSeq = NewComerDbDataSeq( itSeq, itMap->second.end() );
				break;
			}
		}
        	NewComerDbDataSeq ret = doRandom( timedSeq, limit );
		res[ itMap->first ] = ret;
		return res;
	}
}

NewComerDbDataSeq NewComerCacheDataI::getByType(int type,int inDays,int limit) {
        struct tm *tm_local_now;
        time_t now = time(NULL);
        tm_local_now = localtime(&now);
        tm_local_now->tm_hour = 0;
        tm_local_now->tm_min = 0;
        tm_local_now->tm_sec = 0;
        time_t time_zero = timelocal(tm_local_now);
        int time_limit = time_zero-inDays*86400;
	/*
        int time_deadline = time_zero-DAYS_LIMIT*86400;
	*/

	NewComerDbDataSeq res;
	{
		IceUtil::Mutex::Lock lock(_mutex);

		NewComerDbDataSeq timedSeq;
		NewComerDbDataMap ::iterator itMap = newsDbMap.find( type );
		if( itMap != newsDbMap.end() ){
			for( NewComerDbDataSeq::iterator itSeq=itMap->second.begin(); itSeq!=itMap->second.end(); ++itSeq ){
				if( time_limit < itSeq->time ){
					timedSeq = NewComerDbDataSeq( itSeq, itMap->second.end() );
					break;
				}
			}
			res = doRandom( timedSeq, limit );
			//MCE_WARN("[NewComerCacheDataI::getByType] " << timedSeq.size() << " " << res.size() );
		}
		return res;
	}
}

NewComerDbDataSeq NewComerCacheDataI::doRandom( const NewComerDbDataSeq& src, int limit ){
	NewComerDbDataSeq ret;
	int value_sum = 0;                                //随机取好友
	vector<int> posSequence;
	vector<bool> hitSequence;
	for(NewComerDbDataSeq::const_iterator iter=src.begin(); iter!=src.end(); ++iter){
		//TODO:
		int weight = 50;
		//
		value_sum += weight;
		posSequence.push_back(weight);
		hitSequence.push_back(false);
	}

	if( src.empty() || value_sum <= 0 || limit ==0 ){
		return ret;
	}

	int loop_count = ( (int)src.size()<=limit || limit<0 ) ? (int)src.size() : limit;
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
				ret.push_back( src[pos] );
				break;
			}
		}
	}
	return ret;
}

NewComerDbDataSeq NewComerCacheDataI::doOnlineFilter( int& online_count,const NewComerDbDataSeq& src,int limit,const std::map<int,bool>& onlinestates ){
	if( src.empty() || limit == 0 ){
		return NewComerDbDataSeq();
	}
	NewComerDbDataSeq ret_online;
	NewComerDbDataSeq ret_offline;
	for(NewComerDbDataSeq::const_iterator iter=src.begin(); iter!=src.end(); ++iter){
		map<int,bool>::const_iterator it = onlinestates.find( iter->userId );
		if( it != onlinestates.end() && it->second ){
			ret_online.push_back( *iter );
		} else{
			ret_offline.push_back( *iter );
		}
	}
	NewComerDbDataSeq ret;
	for(NewComerDbDataSeq::const_iterator iter=ret_online.begin(); iter!=ret_online.end(); ++iter){
		ret.push_back(*iter);
		if( --limit == 0 ) break;
	}
	online_count = ret.size();
	if( limit == 0 ){
		return ret;
	} 
	ret_offline = doRandom( ret_offline,ret_offline.size() );
	int count = 0;
	for( NewComerDbDataSeq::const_iterator iter=ret_offline.begin(); iter!=ret_offline.end(); ++iter ){
		ret.push_back(*iter);
		if( ++count == limit ) break;
	}
	return ret;
}

