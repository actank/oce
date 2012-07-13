#ifndef _USER_DESC_CHECK_HANDLER_H__
#define _USER_DESC_CHECK_HANDLER_H__

#include "CheckHandler.h"

#include "UserContactAdapter.h"
#include "UserTimeAdapter.h"
#include "UserDescReaderAdapter.h"
#include "user/UserDesc/hotback/share/UserDescDataI.cpp"
#include <sstream>
#include <ext/hash_set>

namespace xce 
{
namespace generalcheck
{
	using xce::userdesc::UserDescDataPtr;
	using xce::userdesc::UserTimeFactoryI;
	using xce::userdesc::UserContactFactoryI;
	using xce::adapter::userdesc::UserDescReaderAdapter;
	using xce::adapter::usercontact::UserContactAdapter;
	using xce::adapter::usertime::UserTimeAdapter;

	class UserDescCheckHandler : public CheckHandler
	{
		public:
		
			UserDescCheckHandler( const int sourceCluster, const int desCluster ) : CheckHandler( sourceCluster, desCluster, 10, 1000 ) 
			{
				_userTimeFactory = new UserTimeFactoryI();
				_userContactFactory = new UserContactFactoryI();
			}
			
		public:
			
			virtual MyUtil::ObjectMap getSource( const MyUtil::LongSeq& ids, const int mod )
			{
				MyUtil::ObjectMap res;
				for( MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it )
				{
					UserDescDataPtr data;

					try
					{
						data = getUserDescFromDB(*it);
					} catch( ... )
					{
						std::ostringstream os;
						os<<"debug UserDescHandler::getSource Exception id = "<<*it<<" !";
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
					UserDescDataPtr data;

					try
					{
						data = UserDescReaderAdapter::instance().getUserDesc(*it);
					} catch( ... )
					{
						std::ostringstream os;
						os<<"debug UserDescHandler::getDes Exception id = "<<*it<<" !";
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
					UserDescDataPtr sourceData;

					try
					{
						sourceData = getUserDescFromDB(*it);
					} catch( ... )
					{
						wrongIds.push_back(*it);
						std::ostringstream os;
						os<<"debug UserDescHandler::compare getSource Exception id = "<<*it<<" !";
						std::cout<<os.str()<<std::endl;
						continue;
					}
					
					UserDescDataPtr desData;

					try
					{
						desData = UserDescReaderAdapter::instance().getUserDesc(*it);
					} catch( ... )
					{
						wrongIds.push_back(*it);
						std::ostringstream os;
						os<<"debug UserDescHandler::compare getDes Exception id = "<<*it<<" !";
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
						UserContactAdapter::instance().reload(*it);
						UserTimeAdapter::instance().reload(*it);
					}catch(...)
					{
						std::cout<<"UserDescCheckHander repair failed id = "<<*it;
						try
						{
							UserContactAdapter::instance().reload(*it);
							UserTimeAdapter::instance().reload(*it);
						}catch(...)
						{
							std::cout<<"UserDescCheckHander repair failed again id = "<<*it;
						}
					}
				}
			}

		private:
			ServantFactoryPtr _userTimeFactory;
			ServantFactoryPtr _userContactFactory;

			UserDescDataPtr getUserDescFromDB(int userId) {
				UserDescDataPtr result = new UserDescData;
				UserTimeDataIPtr rawTimeData = UserTimeDataIPtr::dynamicCast(_userTimeFactory->create(userId));
				if(rawTimeData) {
					result->userTime = UserTimeDataPtr::dynamicCast(rawTimeData->getData());
				}
				UserContactDataIPtr rawContactData = UserContactDataIPtr::dynamicCast(_userContactFactory->create(userId));
				if(rawContactData) {
					result->userContact = UserContactDataPtr::dynamicCast(rawContactData->getData());
				}
				result->userFav = new UserFavData;
				result->userFav->id = result->userTime->id;
				return result;
			}

			bool isEqual( const Ice::ObjectPtr& sourceData, const Ice::ObjectPtr& desData )
			{
				UserDescDataPtr sData = UserDescDataPtr::dynamicCast( sourceData );
				UserDescDataPtr dData = UserDescDataPtr::dynamicCast( desData );
				bool flag = true;
				if(sData->userTime->id != dData->userTime->id)
				{
					std::cout<<"debug wrong userTime id; source " << sData->userTime->id << " des " << dData->userTime->id << std::endl;
					flag = false;
				}
				if(sData->userTime->registerTime != dData->userTime->registerTime)
				{
					std::cout<<"debug wrong userTime id " << sData->userTime->id << " registerTime; source " << sData->userTime->registerTime << " des " << dData->userTime->registerTime << std::endl;
					flag = false;
				}
				if(sData->userTime->activateTime != dData->userTime->activateTime)
				{
					std::cout<<"debug wrong userTime id " << sData->userTime->id << " activateTime; source " << sData->userTime->activateTime << " des " << dData->userTime->activateTime << std::endl;
					flag = false;
				}
				if(sData->userTime->lastLoginTime != dData->userTime->lastLoginTime)
				{
					std::cout<<"debug wrong userTime id " << sData->userTime->id << " lastLoginTime; source " << sData->userTime->lastLoginTime << " des " << dData->userTime->lastLoginTime << std::endl;
					flag = false;
				}
				if(sData->userTime->lastLogoutTime != dData->userTime->lastLogoutTime)
				{
					std::cout<<"debug wrong userTime id " << sData->userTime->id << " lastLogoutTime; source " << sData->userTime->lastLogoutTime << " des " << dData->userTime->lastLogoutTime << std::endl;
					flag = false;
				}
				if(sData->userContact->id != dData->userContact->id)
				{
					std::cout<<"debug wrong userContact id; source " << sData->userContact->id << " des " << dData->userContact->id << std::endl;
					flag = false;
				}
				if(sData->userContact->msn != dData->userContact->msn)
				{
					std::cout<<"debug wrong userContact id " << sData->userContact->id << " msn; source " << sData->userContact->msn << " des " << dData->userContact->msn << std::endl;
					flag = false;
				}
				if(sData->userContact->mobilePhone != dData->userContact->mobilePhone)
				{
					std::cout<<"debug wrong userContact id " << sData->userContact->id << " mobilePhone; source " << sData->userContact->mobilePhone << " des " << dData->userContact->mobilePhone << std::endl;
					flag = false;
				}
				if(sData->userContact->fixPhone != dData->userContact->fixPhone)
				{
					std::cout<<"debug wrong userContact id " << sData->userContact->id << " fixPhone; source " << sData->userContact->fixPhone << " des " << dData->userContact->fixPhone << std::endl;
					flag = false;
				}
				if(sData->userContact->qq != dData->userContact->qq)
				{
					std::cout<<"debug wrong userContact id " << sData->userContact->id << " qq; source " << sData->userContact->qq << " des " << dData->userContact->qq << std::endl;
					flag = false;
				}
				if(sData->userContact->homepage != dData->userContact->homepage)
				{
					std::cout<<"debug wrong userContact id " << sData->userContact->id << " homepage; source " << sData->userContact->id << " des " << dData->userContact->id << std::endl;
					flag = false;
				}
				return flag;
			}
	};

}
}

#endif
