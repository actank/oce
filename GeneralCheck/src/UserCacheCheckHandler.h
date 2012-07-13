#ifndef _USERCACHE_CHECK_HANDLER_H__
#define _USERCACHE_CHECK_HANDLER_H__

#include "CheckHandler.h"

#include "UserCacheAdapter.h"
#include "UserBaseAdapter.h"
#include "DistUserCacheReloaderAdapter.h"

namespace xce 
{
namespace generalcheck
{
	class UserCacheCheckHandler : public CheckHandler
	{
		public:
		
			UserCacheCheckHandler( const int sourceCluster, const int desCluster ) : CheckHandler( sourceCluster, desCluster ) 
			{
			}
			
		public:
			
			virtual MyUtil::ObjectMap getSource( const MyUtil::LongSeq& ids, const int mod )
			{
				MyUtil::ObjectMap res;
				for( MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it )
				{
					xce::adapter::userbase::UserBaseCommonViewPtr data;

					try
					{
						data = xce::adapter::userbase::UserBaseAdapter::instance().getUserBaseCommonView( *it );
					}catch( ... )
					{
						std::cout<<"debug UserCacheCheckHandler::getSource Exception!"<<std::endl;
						continue;
					}
					if( data )
					{
						res.insert( make_pair( *it, data ) );
					}
				}

				return res;
			}
		
			virtual MyUtil::ObjectMap getDes( const MyUtil::LongSeq& ids, const int mod )
			{
				std::map<int, xce::usercache::UserCachePtr> data = xce::usercache::UserCacheAdapter::instance().GetUserCache( MyUtil::IntSeq( ids.begin(), ids.end() ) );
				return MyUtil::ObjectMap( data.begin(), data.end() );
			}
		
			virtual MyUtil::LongSeq compare( const MyUtil::ObjectMap& sourceData, const MyUtil::ObjectMap& desData )
			{
				MyUtil::LongSeq wrongIds;

				for( MyUtil::ObjectMap::const_iterator it = sourceData.begin(); it != sourceData.end(); ++it )
				{
					MyUtil::ObjectMap::const_iterator findIt = desData.find( it->first );
					if( findIt == desData.end() )
					{
						wrongIds.push_back( it->first );
						continue;
					}

					if( isEqual( it->second, findIt->second ) == false )
					{
						wrongIds.push_back( it->first );
					}
				}
				
				return wrongIds;
			}
		
			virtual void repair( const MyUtil::LongSeq& ids, const int mod )
			{
				xce::distcache::usercache::DistUserCacheReloaderAdapter::instance().reload( MyUtil::IntSeq( ids.begin(), ids.end() ) );
			}

		private:

			bool isEqual( const Ice::ObjectPtr& sourceData, const Ice::ObjectPtr& desData )
			{
				xce::adapter::userbase::UserBaseCommonViewPtr sData = xce::adapter::userbase::UserBaseCommonViewPtr::dynamicCast( sourceData );
				xce::usercache::UserCachePtr dData = xce::usercache::UserCachePtr::dynamicCast( desData );

				if( !sData )
				{
					return false;
				}
				if( !dData )
				{
					return false;
				}
				
				if( dData->id() != sData->id() )
				{
					std::cout << "debug " << dData->id() << " wrong field id " << std::endl;
					return false;
				}
				if( dData->status() != sData->status() )
				{
					std::cout << "debug " << dData->id() << " wrong field status " << std::endl;
					return false;
				}
				if( dData->name() != sData->name() )
				{
					std::cout << "debug " << dData->id() << " wrong field namespace " << std::endl;
					return false;
				}
				if( dData->univ() != sData->univ() )
				{
					std::cout << "debug " << dData->id() << " wrong field univ " << std::endl;
					return false;
				}
				if( dData->tinyurl() != sData->tinyurl() )
				{
					std::cout << "debug " << dData->id() << " wrong field tinyurl " << std::endl;
					return false;
				}
				if( sData->gender() != "男生" && sData->gender() != "女生" )
				{
					if( dData->gender() != "" )
					{
						std::cout << "debug " << dData->id() << " wrong field gender " << std::endl;
						return false;
					}
				}else
				{
					if( dData->gender() != sData->gender() )
					{
						std::cout << "debug " << dData->id() << " wrong field gender " << std::endl;
						return false;
					}
				}
				if( dData->state() != sData->state() )
				{
					std::cout << "debug " << dData->id() << " wrong field state " << std::endl;
					return false;
				}
				if( dData->level() != sData->level() )
				{
					std::cout << "debug " << dData->id() << " wrong field level " << std::endl;
					return false;
				}

				return true;
			}

	};

}
}

#endif
