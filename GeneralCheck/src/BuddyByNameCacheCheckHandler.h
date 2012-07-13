#ifndef _BUDDYBYNAMECACHE_CHECK_HANDLER_H__
#define _BUDDYBYNAMECACHE_CHECK_HANDLER_H__

#include "CheckHandler.h"

#include "BuddyByIdReplicaAdapter.h"
#include "BuddyByNameReplicaAdapter.h"
#include "BuddyCacheCheckHandler.h"
#include "common.h"

namespace xce 
{
namespace generalcheck
{
	class BuddyByNameCacheCheckHandler : public CheckHandler
	{
		public:
			BuddyByNameCacheCheckHandler( const int sourceCluster, const int desCluster ,const int threadSize, const int getBatchSize) : 
				CheckHandler( sourceCluster, desCluster, threadSize, getBatchSize ) 
			{
			}
			
		public:
			
			virtual MyUtil::ObjectMap getSource( const MyUtil::LongSeq& ids, const int mod )
			{
				MyUtil::ObjectMap res;
				MyUtil::IntSeq idsint;
				for(MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it){
					idsint.push_back(*it);
				}
				MyUtil::Int2IntMap buddycountmap = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendCounts(idsint);
				if(buddycountmap.size()!=ids.size()){
					std::cout<<"debug BuddyByNameCacheCheckHandler::getSource Exception: getFriendCounts failed"
						<<" ids.size=" <<ids.size() <<" buddycountmap.size="<< buddycountmap.size() 
						<<" ids.front=" << ids.front() << " ids.back=" << ids.back() <<std::endl;
				} 
				for( MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it )
				{
					MyUtil::Int2IntMap::const_iterator itf = buddycountmap.find(*it);
					if(itf == buddycountmap.end()){
						std::cout << "getSource redo id: " << *it << endl;
						continue;
					}
					CheckDataPtr data = new CheckData(itf->second);
					res.insert( make_pair( *it, data ) );
				}

				return res;
			}
		
			virtual MyUtil::ObjectMap getDes( const MyUtil::LongSeq& ids, const int mod )
			{
				MyUtil::ObjectMap res;
				MyUtil::IntSeq idsint;
                                for(MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it){
                                        idsint.push_back(*it);
                                }
                                MyUtil::IntSeq errorSeq = xce::buddy::adapter::BuddyByNameCacheAdapter::instance().checkFriendCounts(idsint);
				for(MyUtil::IntSeq::iterator it = errorSeq.begin(); it != errorSeq.end(); ++it){
					//std::cout << "REPLICAERROR " << *it << std::endl;
					MCE_INFO( "REPLICAERROR " << *it );
				}
				MyUtil::Int2IntMap buddycountmap = xce::buddy::adapter::BuddyByNameCacheAdapter::instance().getFriendCounts(idsint);
                                if(buddycountmap.size()!=ids.size()){
                                        std::cout<<"debug BuddyByNameCacheCheckHandler::getDes Exception: getFriendCounts failed"
						<<" ids.size=" <<ids.size() <<" buddycountmap.size="<< buddycountmap.size() 
						<<" ids.front=" << ids.front() << " ids.back=" << ids.back() <<std::endl;
                                }
                                for( MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it )
                                {
                                        MyUtil::Int2IntMap::const_iterator itf = buddycountmap.find(*it);
                                        if(itf == buddycountmap.end()){
                                                std::cout << "getDes redo id: " << *it << endl;
                                                continue;
                                        }
                                        CheckDataPtr data = new CheckData(itf->second);
                                        res.insert( make_pair( *it, data ) );
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

	};

}
}

#endif
