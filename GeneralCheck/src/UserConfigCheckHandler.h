#ifndef _USER_CONFIG_CHECK_HANDLER_H__
#define _USER_CONFIG_CHECK_HANDLER_H__

#include "CheckHandler.h"

#include "UserConfig.h"
#include "UserConfigAdapter.h"
#include "UserConfigReaderAdapter.h"
#include <sstream>
#include <ext/hash_set>
#include <string.h>

namespace xce 
{
namespace generalcheck
{
	using xce::adapter::UserConfigAdapter;
	using xce::readeradapter::UserConfigReaderAdapter;


	class UserConfigCheckHandler : public CheckHandler
	{
		public:
		
			UserConfigCheckHandler( const int sourceCluster, const int desCluster ) : CheckHandler( sourceCluster, desCluster, 5, 1000 )
			{
			}
			
		public:
			
			virtual MyUtil::ObjectMap getSource( const MyUtil::LongSeq& ids, const int mod )
			{
				MyUtil::ObjectMap res;
				for( MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it )
				{
					xce::adapter::UserConfigInfoPtr data;

					try
					{
						data = UserConfigAdapter::instance().getUserConfig(*it);
					} catch( ... )
					{
						std::ostringstream os;
						os<<"debug UserConfigHandler::getSource Exception id = "<<*it<<" !";
						std::cout<<os.str()<<std::endl;
						continue;
					}
					if( data ) {
						res.insert( make_pair( *it, data ) );
					}
				}

				return res;
			}
		
			virtual MyUtil::ObjectMap getDes( const MyUtil::LongSeq& ids, const int mod )
			{
				MyUtil::ObjectMap res;
				for( MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it )
				{
					xce::readeradapter::UserConfigInfoPtr data;

					try
					{
						data = UserConfigReaderAdapter::instance().getUserConfig(*it);
					} catch( ... )
					{
						std::ostringstream os;
						os<<"debug UserConfigHandler::getDes Exception id = "<<*it<<" !";
						std::cout<<os.str()<<std::endl;
						continue;
					}
					if( data ) {
						res.insert( make_pair( *it, data ) );
					}
				}

				return res;
			}
		
			virtual MyUtil::LongSeq compare( const MyUtil::ObjectMap& sourceData, const MyUtil::ObjectMap& desData )
			{
				__gnu_cxx::hash_set<long> wrongIdsSet;
				MyUtil::LongSeq wrongIds;

				for( MyUtil::ObjectMap::const_iterator it = sourceData.begin(); it != sourceData.end(); ++it )
				{
					MyUtil::ObjectMap::const_iterator findIt = desData.find( it->first );
					if( findIt == desData.end() )
					{
						wrongIdsSet.insert(it->first);
						continue;
					}

					if( isEqual( it->second, findIt->second ) == false )
					{
						wrongIdsSet.insert(it->first);
					}
				}
				
				for( MyUtil::ObjectMap::const_iterator it = desData.begin(); it != desData.end(); ++it )
				{
					MyUtil::ObjectMap::const_iterator findIt = sourceData.find( it->first );
					if( findIt == sourceData.end() )
					{
						wrongIdsSet.insert(it->first);
						continue;
					}
				}

				for(__gnu_cxx::hash_set<long>::const_iterator it = wrongIdsSet.begin(); it != wrongIdsSet.end(); ++it)
				{
					xce::adapter::UserConfigInfoPtr sourceData;

					try
					{
						sourceData = UserConfigAdapter::instance().getUserConfig(*it);
					} catch( ... )
					{
						wrongIds.push_back(*it);
						std::ostringstream os;
						os<<"debug UserConfigHandler::compare getSource Exception id = "<<*it<<" !";
						std::cout<<os.str()<<std::endl;
						continue;
					}
					
					xce::readeradapter::UserConfigInfoPtr desData;

					try
					{
						desData = UserConfigReaderAdapter::instance().getUserConfig(*it);
					} catch( ... )
					{
						wrongIds.push_back(*it);
						std::ostringstream os;
						os<<"debug UserConfigHandler::compare getDes Exception id = "<<*it<<" !";
						std::cout<<os.str()<<std::endl;
						continue;
					}
					
					if( isEqual( sourceData, desData ) == false )
					{
						wrongIds.push_back(*it);
					}

				}

				return wrongIds;
			}
		
			virtual void repair( const MyUtil::LongSeq& ids, const int mod )
			{
				for(MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it)
				{
					try
					{
						UserConfigAdapter::instance().reload(*it);
					}catch(...)
					{
						std::cout<<"UserConfigCheckHander repair failed id = "<<*it;
						try
						{
							UserConfigAdapter::instance().reload(*it);
						}catch(...)
						{
							std::cout<<"UserConfigCheckHander repair failed again id = "<<*it;
						}
					}
				}
			}

		private:

			template <class T>
			bool checkItem(int userId, string itemName, T sItem, T dItem){
				bool result = (sItem == dItem);
				if(!result){
					std::cout << "Inconsistent UserConfigInfo[" << itemName << "] : userId=" << userId
							<< ", source=" << sItem << ", des=" << dItem << std::endl;
				}
				return result;
			}

			bool isEqual( const Ice::ObjectPtr& sourceData, const Ice::ObjectPtr& desData )
			{
				xce::adapter::UserConfigInfoPtr sData = xce::adapter::UserConfigInfoPtr::dynamicCast( sourceData );
				xce::readeradapter::UserConfigInfoPtr dData = xce::readeradapter::UserConfigInfoPtr::dynamicCast( desData );

				bool result = true;
				int userId = sData->userId();

				result &= checkItem<int>(userId, "basicConfig", sData->basicConfig(), dData->basicConfig());
				result &= checkItem<int>(userId, "pokeConfig", sData->pokeConfig(), dData->pokeConfig());
				result &= checkItem<int>(userId, "requestFriendConfig", sData->requestFriendConfig(), dData->requestFriendConfig());
				result &= checkItem<int>(userId, "photoConfig", sData->photoConfig(), dData->photoConfig());
				result &= checkItem<int>(userId, "messageConfig", sData->messageConfig(), dData->messageConfig());
				result &= checkItem<long>(userId, "emailConfig", sData->emailConfig(), dData->emailConfig());
				result &= checkItem<int>(userId, "browseConfig", sData->browseConfig(), dData->browseConfig());
				result &= checkItem<string>(userId, "displayMenuList", sData->displayMenuList(), dData->displayMenuList());
				result &= checkItem<string>(userId, "moreMenuList", sData->moreMenuList(), dData->moreMenuList());
				result &= checkItem<int>(userId, "wantSeeCss", sData->wantSeeCss(), dData->wantSeeCss());
				result &= checkItem<string>(userId, "sendFeedConfig", sData->sendFeedConfig(), dData->sendFeedConfig());
				result &= checkItem<string>(userId, "recvFeedConfig", sData->recvFeedConfig(), dData->recvFeedConfig());
				result &= checkItem<string>(userId, "profilePrivacy", sData->profilePrivacy(), dData->profilePrivacy());
				result &= checkItem<int>(userId, "statusConfig", sData->statusConfig(), dData->statusConfig());

				return result;
			}
	};

}
}

#endif
