#ifndef _USERAPICACHE_CHECK_HANDLER_H__
#define _USERAPICACHE_CHECK_HANDLER_H__

#include "CheckHandler.h"

#include "user/UserApiCache/dist/reloader/DistUserApiCacheAdapter.h"
#include "DistUserApiCacheReloaderAdapter.h"
#include "UserBaseAdapter.h"
#include "UserConfigAdapter.h"
#include <sstream>

namespace xce 
{
namespace generalcheck
{
	using xce::userapicache::UserApiCacheData1Ptr;

	class UserApiCacheCheckHandler : public CheckHandler
	{
		public:
		
			UserApiCacheCheckHandler( const int sourceCluster, const int desCluster ) : CheckHandler( sourceCluster, desCluster, 6, 1000 ) 
			{
				xce::distcache::userapicache::DistUserApiCacheAdapter::instance().initialize();
			}
			
		public:
			
			virtual MyUtil::ObjectMap getSource( const MyUtil::LongSeq& ids, const int mod )
			{
				MyUtil::ObjectMap res;
				for( MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it )
				{
					xce::adapter::userbase::UserBaseFullViewPtr fullViewPtr;
					xce::adapter::UserConfigInfoPtr configInfoPtr;

					try
					{
						fullViewPtr = xce::adapter::userbase::UserBaseAdapter::instance().getUserBaseFullView( *it );
						configInfoPtr = xce::adapter::UserConfigAdapter::instance().getUserConfig(*it);		
					} catch( ... )
					{
						std::cout<<"debug UserApiCacheHandler::getSource Exception!"<<std::endl;
						continue;
					}
					if( fullViewPtr != 0 && configInfoPtr != 0) {
						xce::userapicache::UserApiCacheData1Ptr apiCacheData1Ptr = new xce::userapicache::UserApiCacheData1();
						apiCacheData1Ptr->id = fullViewPtr->id();
						apiCacheData1Ptr->status = fullViewPtr->status();
						apiCacheData1Ptr->name = fullViewPtr->name();
						apiCacheData1Ptr->state = fullViewPtr->state();
						apiCacheData1Ptr->level = fullViewPtr->level();
						apiCacheData1Ptr->auth = fullViewPtr->auth();
						apiCacheData1Ptr->stage = fullViewPtr->stage();
						apiCacheData1Ptr->gender = fullViewPtr->gender();
						apiCacheData1Ptr->univ = fullViewPtr->univ();
						apiCacheData1Ptr->birthyear = fullViewPtr->birthyear();
						apiCacheData1Ptr->birthday = fullViewPtr->birthday();
						apiCacheData1Ptr->birthmonth = fullViewPtr->birthmonth();
						apiCacheData1Ptr->homeprovince = fullViewPtr->homeprovince();
						apiCacheData1Ptr->homecity = fullViewPtr->homecity();
						apiCacheData1Ptr->mainurl = fullViewPtr->mainurl();
						apiCacheData1Ptr->tinyurl = fullViewPtr->tinyurl();
						apiCacheData1Ptr->headurl = fullViewPtr->headurl();
						
						apiCacheData1Ptr->statusconfig = configInfoPtr->statusConfig();
						apiCacheData1Ptr->basicconfig = configInfoPtr->basicConfig();
						apiCacheData1Ptr->profileprivacy = configInfoPtr->profilePrivacy();
						res.insert( make_pair( *it, apiCacheData1Ptr ) );
					}
				}

				return res;
			}
		
			virtual MyUtil::ObjectMap getDes( const MyUtil::LongSeq& ids, const int mod )
			{
				std::vector<long> missIds;
				std::map<long, xce::distcache::userapicache::UserApiCacheMemDataPtr> data = xce::distcache::userapicache::DistUserApiCacheAdapter::instance().get(ids, missIds);
				return MyUtil::ObjectMap(data.begin(),data.end());
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
				xce::distcache::userapicache::DistUserApiCacheReloaderAdapter::instance().reload( MyUtil::IntSeq( ids.begin(), ids.end() ) );
			}

		private:

			bool isEqual( const Ice::ObjectPtr& sourceData, const Ice::ObjectPtr& desData )
			{
				xce::userapicache::UserApiCacheData1Ptr sData = xce::userapicache::UserApiCacheData1Ptr::dynamicCast( sourceData );
				xce::distcache::userapicache::UserApiCacheMemDataPtr dData = xce::distcache::userapicache::UserApiCacheMemDataPtr::dynamicCast( desData );
				string s_str = toString(sData);
				string d_str = toString(dData);
				if(s_str.compare("NULL") != 0 && s_str.compare(d_str) == 0) {
					return true;
				} else {
					string dif1 = "", dif2 = "";
					findDiff(s_str, d_str, dif1, dif2);
					std::cout <<"debug id = " << sData->id << "\t" << dif1 << endl;
					std::cout <<"debug id = " << dData->id() << "\t" << dif2 << endl;
					return false;
				}
			}
			void findDiff(const string& str1, const string& str2, string& dif1, string& dif2) {
				int spos = 0;
				int size1 = str1.length();
				int size2 = str2.length();
				while(spos < size1 && spos < size2) {
					size_t fpos1 = str1.find("\t", spos);
					size_t fpos2 = str2.find("\t", spos);
					if(fpos1 != string::npos) {
						dif1 = str1.substr(spos, fpos1 - spos);
					} else {
						dif1 = str1.substr(spos);
					}
					if(fpos2 != string::npos) {
						dif2 = str2.substr(spos, fpos2 - spos);
					} else {
						dif2 = str2.substr(spos);
					}
					if(dif1.compare(dif2) != 0) {
						return;
					}
					spos = fpos1 + 1;
				}
			}
			string toString(const xce::userapicache::UserApiCacheData1Ptr ptr) {
				ostringstream str;
				if(ptr == 0) {
					str << "NULL";
				} else {
					str << "UserApiCacheData : id = " << ptr->id << "\tstatus = " << ptr->status << "\tname = " << ptr->name 
						<< "\tstate = " << ptr->state << "\tlevel = " << ptr->level << "\tauth = " << ptr->auth << "\tstatusconfig = " 
						<< ptr->statusconfig << "\tbasicconfig = " << ptr->basicconfig << "\tprofileprivacy = "
						<< ptr->profileprivacy << "\tstage = " << ptr->stage << "\tgender = " << ptr->gender << "\tuniv = " 
						<< ptr->univ << "\tbirthyear = " << ptr->birthyear << "\tbirthmonth = " << ptr->birthmonth << "\tbirthday = "
						<< ptr->birthday << "\thomeprovince = " << ptr->homeprovince << "\thomecity = " << ptr->homecity
						<< "\tmainurl = " << ptr->mainurl << "\theadurl = " << ptr->headurl << "\ttinyurl = " << ptr->tinyurl;
				}
				return str.str();
			}
			
			string toString(const xce::distcache::userapicache::UserApiCacheMemDataPtr ptr) {
				ostringstream str;
				if(ptr == 0) {
					str << "NULL";
				} else {
					str << "UserApiCacheData : id = " << ptr->id() << "\tstatus = " << ptr->status() << "\tname = " << ptr->name() 
						<< "\tstate = " << ptr->state() << "\tlevel = " << ptr->level() << "\tauth = " << ptr->auth() << "\tstatusconfig = " 
						<< ptr->statusconfig() << "\tbasicconfig = " << ptr->basicconfig() << "\tprofileprivacy = "
						<< ptr->profileprivacy() << "\tstage = " << ptr->stage() << "\tgender = " << ptr->gender() << "\tuniv = " 
						<< ptr->univ() << "\tbirthyear = " << ptr->birthyear() << "\tbirthmonth = " << ptr->birthmonth() << "\tbirthday = "
						<< ptr->birthday() << "\thomeprovince = " << ptr->homeprovince() << "\thomecity = " << ptr->homecity()
						<< "\tmainurl = " << ptr->mainurl() << "\theadurl = " << ptr->headurl() << "\ttinyurl = " << ptr->tinyurl();
				}
				return str.str();
			}
	};

}
}

#endif
