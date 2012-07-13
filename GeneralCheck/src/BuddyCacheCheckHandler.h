#ifndef _BUDDYCACHE_CHECK_HANDLER_H__
#define _BUDDYCACHE_CHECK_HANDLER_H__

#include "CheckHandler.h"
#include "BuddyByIdReplicaAdapter.h"
#include "BuddyByNameReplicaAdapter.h"
#include "BuddyByAddTimeReplicaAdapter.h"
#include "BuddyByOnlineTimeReplicaAdapter.h"
#include "common.h"

namespace xce 
{
namespace generalcheck
{
	using xce::buddy::adapter::BuddyByNameCacheAdapter;
	using xce::buddy::adapter::BuddyByAddTimeCacheAdapter;
	using xce::buddy::adapter::BuddyByIdCacheAdapter;
	using xce::buddy::adapter::BuddyByOnlineTimeCacheAdapter;

	
	template<class T>
	MyUtil::Int2IntMap getCount(const MyUtil::IntSeq& ids, const string& name, const string& func, MyUtil::IntSeq (T::*fp)(const MyUtil::IntSeq& ids), 
								MyUtil::Int2IntMap (T::*fp1)(const MyUtil::IntSeq& ids)) {
		MyUtil::IntSeq errorSeq = (T::instance().*fp)(ids);
		for(MyUtil::IntSeq::iterator it = errorSeq.begin(), end = errorSeq.end(); it != end; ++it) {
			MCE_INFO(name << " REPLICAERROR " << *it);
		}

		MyUtil::Int2IntMap buddycountmap = (T::instance().*fp1)(ids);
		if(buddycountmap.size()!=ids.size()){
			std::cout<<"debug " << name << "CacheCheckHandler::" << func << " Exception: getFriendCounts failed"
					<<" ids.size=" <<ids.size() <<" buddycountmap.size="<< buddycountmap.size() 
					<<" ids.front=" << ids.front() << " ids.back=" << ids.back() <<std::endl;
		}
		return buddycountmap; 
	}	

	class BuddyCacheCheckHandler : public CheckHandler
	{
		public:
			BuddyCacheCheckHandler( const int sourceCluster, const int desCluster ,const int threadSize, const int getBatchSize) : 
				CheckHandler( sourceCluster, desCluster, threadSize, getBatchSize ) {
			}
			
			virtual MyUtil::ObjectMap getSource( const MyUtil::LongSeq& ids, const int mod )
			{
				std::cout << "debug BuddyCacheCheckHandler::BuddyCacheCheckHandler " << ids.size() << " ids will be processed" << endl; 
				MyUtil::ObjectMap res;
				MyUtil::IntSeq idsint(ids.begin(), ids.end());
				MyUtil::Int2IntMap buddycountmap = getCount(idsint, "BuddyById", "getSource", &BuddyByIdCacheAdapter::checkFriendCounts, 
															&BuddyByIdCacheAdapter::getFriendCounts);
				for( MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it ) {
					MyUtil::Int2IntMap::const_iterator itf = buddycountmap.find(*it);
					if(itf == buddycountmap.end()){
						std::cout << "getSource redo id: " << *it << endl;
						continue;
					}
					CheckDataPtr data = new CheckData(itf->second);
					res.insert(make_pair( *it, data ) );
				}
				return res;
			}
		
			virtual MyUtil::ObjectMap getDes( const MyUtil::LongSeq& ids, const int mod ) {
				MyUtil::ObjectMap res;
				MyUtil::IntSeq idsint(ids.begin(), ids.end());
				MyUtil::Int2IntMap nameMap = getCount(idsint, "BuddyByName", "getDes", &BuddyByNameCacheAdapter::checkFriendCounts, 
															&BuddyByNameCacheAdapter::getFriendCounts);
				MyUtil::Int2IntMap addTimeMap = getCount(idsint, "BuddyByAddTime", "getDes", &BuddyByAddTimeCacheAdapter::checkFriendCounts, 
															&BuddyByAddTimeCacheAdapter::getFriendCounts);
				MyUtil::Int2IntMap onlineTimeMap = getCount(idsint, "BuddyByOnlineTime", "getDes", &BuddyByOnlineTimeCacheAdapter::checkFriendCounts, 
															&BuddyByOnlineTimeCacheAdapter::getFriendCounts);
				MyUtil::Int2IntMap::const_iterator nameEnd = nameMap.end(), addTimeEnd = addTimeMap.end(), onlineTimeEnd = onlineTimeMap.end();
				for(MyUtil::IntSeq::const_iterator it = idsint.begin(), end = idsint.end(); it != end; ++it) {
					MyUtil::Int2IntMap::const_iterator nameIt = nameMap.find(*it);
					MyUtil::Int2IntMap::const_iterator addTimeIt = addTimeMap.find(*it);
					MyUtil::Int2IntMap::const_iterator onlineTimeIt = onlineTimeMap.find(*it);
					if(nameIt != nameEnd && addTimeIt != addTimeEnd && onlineTimeIt != onlineTimeEnd && nameIt->second == addTimeIt->second 
						&& addTimeIt->second == onlineTimeIt->second) {
						CheckDataPtr data = new CheckData(nameIt->second);
						res.insert(make_pair(*it, data));
					} else {
						map<int, int> countMap;	
						printErrMsg(nameIt, nameEnd, "NameCache", countMap);
						printErrMsg(addTimeIt, addTimeEnd, "AddTimeCache", countMap);
						printErrMsg(onlineTimeIt, onlineTimeEnd, "OnlineTimeCache", countMap);
						for(map<int, int>::const_iterator mapIt = countMap.begin(); mapIt != countMap.end(); ++mapIt) {
							if(mapIt->second == 2) {		 
								CheckDataPtr data = new CheckData(mapIt->first);
								res.insert(make_pair(*it, data));
								break;
							}
						}
					}
				}
				return res;
			}
		
			virtual MyUtil::LongSeq compare( const MyUtil::ObjectMap& sourceData, const MyUtil::ObjectMap& desData )
			{
				MyUtil::LongSeq wrongIds;

				for( MyUtil::ObjectMap::const_iterator it = sourceData.begin(); it != sourceData.end(); ++it )
				{
					MyUtil::ObjectMap::const_iterator findIt = desData.find( it->first );
					if( findIt == desData.end() )
					{
						//std::cout << "GETERROR " << it->first << " not found in desData" << std::endl;
						MCE_INFO("GETERROR " << it->first << " not found in desData");
						wrongIds.push_back( it->first );
						continue;
					}

					if( isEqual( it->second, findIt->second ) == false )
					{
						//std::cout << "CHECKERROR " << it->first << " source.size:" << (CheckDataPtr::dynamicCast(it->second))->count
						//	<< " des.size:" << (CheckDataPtr::dynamicCast(findIt->second))->count << endl; 
						MCE_INFO("CHECKERROR " << it->first << " source.size:" << (CheckDataPtr::dynamicCast(it->second))->count
                                                        << " des.size:" << (CheckDataPtr::dynamicCast(findIt->second))->count);
						wrongIds.push_back( it->first );
					}
				}
				
				for( MyUtil::ObjectMap::const_iterator it = desData.begin(); it != desData.end(); ++it )
				{
					MyUtil::ObjectMap::const_iterator findIt = sourceData.find( it->first );
					if( findIt == sourceData.end() )
					{
						std::cout << "UNEXPECTED " << it->first << "not found in sourceData" << std::endl;
						wrongIds.push_back( it->first );
					}
				}

				return wrongIds;
			}
		
			virtual void repair( const MyUtil::LongSeq& ids, const int mod )
			{
				//do nothing
				for(MyUtil::LongSeq::const_iterator it = ids.begin(); it!= ids.end(); ++it){
					std::cout<< "repair " << *it << std::endl;
				}
			}

		private:

			bool isEqual( const Ice::ObjectPtr& sourceData, const Ice::ObjectPtr& desData )
			{
				return (CheckDataPtr::dynamicCast(sourceData))->count == (CheckDataPtr::dynamicCast(desData))->count ? true : false;
			}

			void printErrMsg(MyUtil::Int2IntMap::const_iterator it, MyUtil::Int2IntMap::const_iterator nIt, const string& type, map<int, int>& countMap) {
				if(it == nIt) {
					MCE_INFO(type << " " << it->first << " not in it");
				} else {
					MCE_INFO(type << " id = " << it->first << " count = " << it->second);
					countMap[it->second]++;
				}
			}

	};

}
}

#endif
