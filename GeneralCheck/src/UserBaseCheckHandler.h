#ifndef _USER_BASE_CHECK_HANDLER_H__
#define _USER_BASE_CHECK_HANDLER_H__

#include "CheckHandler.h"

#include "UserBaseAdapter.h"
#include "UserBornAdapter.h"
#include "UserDoingAdapter.h"
#include "UserLoginCountAdapter.h"
#include "UserNameAdapter.h"
#include "UserPassportAdapter.h"
#include "UserRightAdapter.h"
#include "UserStageAdapter.h"
#include "UserStateAdapter.h"
#include "UserUrlAdapter.h"
#include "UserBaseReaderAdapter.h"
#include <sstream>
#include <ext/hash_set>

namespace xce 
{
namespace generalcheck
{
	using xce::adapter::userbase::UserBaseAdapter;
	using xce::adapter::userbasereader::UserBaseReaderAdapter;
	
	using xce::adapter::userpassport::UserPassportAdapter;
	using xce::adapter::username::UserNameAdapter;
	using xce::adapter::userstate::UserStateAdapter;
	using xce::adapter::userright::UserRightAdapter;
	using xce::adapter::userlogincount::UserLoginCountAdapter;
	using xce::adapter::userurl::UserUrlAdapter;
	using xce::adapter::userborn::UserBornAdapter;
	using xce::adapter::userstage::UserStageAdapter;
	using xce::adapter::userdoing::UserDoingAdapter;

	class UserBaseCheckHandler : public CheckHandler
	{
		public:
		
			UserBaseCheckHandler( const int sourceCluster, const int desCluster ) : CheckHandler( sourceCluster, desCluster, 6, 1000 ) 
			{
			}
			
		public:
			
			virtual MyUtil::ObjectMap getSource( const MyUtil::LongSeq& ids, const int mod )
			{
				MyUtil::ObjectMap res;
				for( MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it )
				{
					xce::adapter::userbase::UserBaseFullViewPtr data;

					try
					{
						data = UserBaseAdapter::instance().getUserBaseFullView(*it);
					} catch( ... )
					{
						std::ostringstream os;
						os<<"debug UserBaseHandler::getSource Exception id = "<<*it<<" !";
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
					xce::adapter::userbasereader::UserBaseFullViewPtr data;

					try
					{
						data = UserBaseReaderAdapter::instance().getUserBaseFullView(*it);
					} catch( ... )
					{
						std::ostringstream os;
						os<<"debug UserBaseHandler::getDes Exception id = "<<*it<<" !";
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
					xce::adapter::userbase::UserBaseFullViewPtr sourceData;

					try
					{
						sourceData = UserBaseAdapter::instance().getUserBaseFullView(*it);
					} catch( ... )
					{
						wrongIds.push_back(*it);
						std::ostringstream os;
						os<<"debug UserBaseHandler::compare getSource Exception id = "<<*it<<" !";
						std::cout<<os.str()<<std::endl;
						continue;
					}
					
					xce::adapter::userbasereader::UserBaseFullViewPtr desData;

					try
					{
						desData = UserBaseReaderAdapter::instance().getUserBaseFullView(*it);
					} catch( ... )
					{
						wrongIds.push_back(*it);
						std::ostringstream os;
						os<<"debug UserBaseHandler::compare getDes Exception id = "<<*it<<" !";
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
						UserPassportAdapter::instance().reload(*it);
						UserNameAdapter::instance().reload(*it);
						UserStateAdapter::instance().reload(*it);
						UserRightAdapter::instance().reload(*it);
						UserLoginCountAdapter::instance().reload(*it);
						UserUrlAdapter::instance().reload(*it);
						UserBornAdapter::instance().reload(*it);
						UserStageAdapter::instance().reload(*it);
						UserDoingAdapter::instance().reload(*it);
					}catch(...)
					{
						std::cout<<"UserBaseCheckHander repair failed id = "<<*it;
						try
						{
							UserPassportAdapter::instance().reload(*it);
							UserNameAdapter::instance().reload(*it);
							UserStateAdapter::instance().reload(*it);
							UserRightAdapter::instance().reload(*it);
							UserLoginCountAdapter::instance().reload(*it);
							UserUrlAdapter::instance().reload(*it);
							UserBornAdapter::instance().reload(*it);
							UserStageAdapter::instance().reload(*it);
							UserDoingAdapter::instance().reload(*it);
						}catch(...)
						{
							std::cout<<"UserBaseCheckHander repair failed again id = "<<*it;
						}
					}
				}
			}

		private:

			bool isEqual( const Ice::ObjectPtr& sourceData, const Ice::ObjectPtr& desData )
			{
				xce::adapter::userbase::UserBaseFullViewPtr sData = xce::adapter::userbase::UserBaseFullViewPtr::dynamicCast( sourceData );
				xce::adapter::userbasereader::UserBaseFullViewPtr dData = xce::adapter::userbasereader::UserBaseFullViewPtr::dynamicCast( desData );
				bool flag = true;
				if(sData->id() != dData->id())
				{
					std::cout<<"debug wrong id; source " << sData->id() << " des " << dData->id() << std::endl;
					flag = false;
				}
				if(sData->status() != dData->status())
				{
					std::cout<<"debug wrong id " << sData->id() << " status; source " << sData->status()<< " des " << dData->status()<< std::endl;
					flag = false;
				}
				if(sData->safestatus() != dData->safestatus())
				{
					std::cout<<"debug wrong id " << sData->id() << " safestatus; source " << sData->safestatus() << " des " << dData->safestatus() << std::endl;
					flag = false;
				}
				if(sData->account() != dData->account())
				{
					std::cout<<"debug wrong id " << sData->id()<< " account; source " << sData->account() << " des " << dData->account() << std::endl;
					flag = false;
				}
				if(sData->passwordmd5() != dData->passwordmd5())
				{
					std::cout<<"debug wrong id " << sData->id() << " passwordmd5; source " << sData->passwordmd5() << " des " << dData->passwordmd5() << std::endl;
					flag = false;
				}
				if(sData->domain() != dData->domain())
				{
					std::cout<<"debug wrong id " << sData->id() << " domain; source" << sData->domain() << " des " << dData->domain() << std::endl;
					flag = false;
				}
				if(sData->name() != dData->name())
				{
					std::cout<<"debug wrong id " << sData->id() << " name; source " << sData->name() << " des " << dData->name() << std::endl;
					flag = false;
				}
				if(sData->nickname() != dData->nickname())
				{
					std::cout<<"debug wrong id " << sData->id() << " nickname; source " << sData->nickname() << " des " << dData->nickname() << std::endl;
					flag = false;
				}
				if(sData->auth() != dData->auth())
				{
					std::cout<<"debug wrong id " << sData->id() << " auth; source " << sData->auth() << " des " << dData->auth() << std::endl;
					flag = false;
				}
				if(sData->rights() != dData->rights())
				{
					std::cout<<"debug wrong id " << sData->id() << " rights; source " << sData->rights() << " des " << dData->rights() << std::endl;
					flag = false;
				}
				if(sData->state() != dData->state())
				{
					std::cout<<"debug wrong id " << sData->id() << " state; source " << sData->state() << " des " << dData->state() << std::endl;
					flag = false;
				}
				if(sData->star() != dData->star())
				{
					std::cout<<"debug wrong id " << sData->id() << " star; source " << sData->star() << " des " << dData->star() << std::endl;
					flag = false;
				}
				if(sData->level() != dData->level())
				{
					std::cout<<"debug wrong id " << sData->id() << " level; source " << sData->level() << " des " << dData->level() << std::endl;
					flag = false;
				}
				if(sData->logincount() != dData->logincount())
				{
					std::cout<<"debug wrong id " << sData->id() << " logincount; source " << sData->logincount() << " des " << dData->logincount() << std::endl;
					flag = false;
				}
				if(sData->gender() != dData->gender())
				{
					std::cout<<"debug wrong id " << sData->id() << " gender; source " << sData->gender() << " des " << dData->gender() << std::endl;
					flag = false;
				}
				if(sData->birthday() != dData->birthday())
				{
					std::cout<<"debug wrong id " << sData->id() << " birthday; source " << sData->birthday() << " des " << dData->birthday() << std::endl;
					flag = false;
				}
				if(sData->birthmonth() != dData->birthmonth())
				{
					std::cout<<"debug wrong id " << sData->id() << " birthmonth; source " << sData->birthmonth() << " des " << dData->birthmonth() << std::endl;
					flag = false;
				}
				if(sData->birthyear() != dData->birthyear())
				{
					std::cout<<"debug wrong id " << sData->id() << " birthyear; source " << sData->birthyear() << " des " << dData->birthyear() << std::endl;
					flag = false;
				}
				/*if(sData->islunarbirth() != dData->islunarbirth())
				{
					std::cout<<"debug wrong id " << sData->id() << " islunarbirth; source " << sData->islunarbirth() << " des " << dData->islunarbirth() << std::endl;
					flag = false;
				}*/
				if(sData->homeprovince() != dData->homeprovince())
				{
					std::cout<<"debug wrong id " << sData->id() << " homeprovince; source " << sData->homeprovince() << " des " << dData->homeprovince() << std::endl;
					flag = false;
				}
				if(sData->homecity() != dData->homecity())
				{
					std::cout<<"debug wrong id " << sData->id() << " homecity; source " << sData->homecity() << " des " << dData->homecity() << std::endl;
					flag = false;
				}
				if(sData->citycode() != dData->citycode())
				{
					std::cout<<"debug wrong id " << sData->id() << " citycode; source " << sData->citycode() << " des " << dData->citycode() << std::endl;
					flag = false;
				}
				if(sData->astrology() != dData->astrology())
				{
					std::cout<<"debug wrong id " << sData->id() << " astrology; source " << sData->astrology() << " des " << dData->astrology() << std::endl;
					flag = false;
				}
				if(sData->univ() != dData->univ())
				{
					std::cout<<"debug wrong id " << sData->id() << " univ; source " << sData->univ() << " des " << dData->univ() << std::endl;
					flag = false;
				}
				if(sData->stage() != dData->stage())
				{
					std::cout<<"debug wrong id " << sData->id() << " stage; source " << sData->stage() << " des " << dData->stage() << std::endl;
					flag = false;
				}
				if(sData->doing() != dData->doing())
				{
					std::cout<<"debug wrong id " << sData->id() << " doing; source " << sData->doing() << " des " << dData->doing() << std::endl;
					flag = false;
				}
				if(sData->doingtime() != dData->doingtime())
				{
					std::cout<<"debug wrong id " << sData->id() << " doingtime; source " << sData->doingtime() << " des " << dData->doingtime() << std::endl;
					flag = false;
				}
				if(sData->largeurl() != dData->largeurl())
				{
					std::cout<<"debug wrong id " << sData->id() << " largeurl; source " << sData->largeurl() << " des " << dData->largeurl() << std::endl;
					flag = false;
				}
				if(sData->tinyurl() != dData->tinyurl())
				{
					std::cout<<"debug wrong id " << sData->id() << " tinyurl; source " << sData->tinyurl() << " des " << dData->tinyurl() << std::endl;
					flag = false;
				}
				if(sData->headurl() != dData->headurl())
				{
					std::cout<<"debug wrong id " << sData->id() << " headurl; source " << sData->headurl() << " des " << dData->headurl() << std::endl;
					flag = false;
				}
				if(sData->mainurl() != dData->mainurl())
				{
					std::cout<<"debug wrong id " << sData->id() << " mainurl; source " << sData->mainurl() << " des " << dData->mainurl() << std::endl;
					flag = false;
				}
				return flag;
			}
	};

}
}

#endif
