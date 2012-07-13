#include "UserCacheAdapter.h"
#include "StatusCacheAdapter.h"
#include "user/UserCache/dist/reloader/DistUserCacheAdapter.h"
#include "UserCache.h"
#include "util/cpp/TimeCost.h"

#include "UserBaseAdapter.h"
#include "PresenceAdapter.h"
#include "WapOnlineAdapter.h"
#include "DistUserCacheReloaderAdapter.h"
using namespace xce::usercache;
using namespace xce::statuscache;
using namespace xce::distcache::usercacheadapter;
using namespace std;

using MyUtil::TimeCost;
using MyUtil::Str2StrMap;
using xce::usercache::UserCacheData3Ptr;
using xce::adapter::userbase::UserBaseFullViewPtr;
using xce::adapter::userbase::UserBaseAdapter;

#include "OnlineEventMediator.h"
using xce::mediator::onlineevent::OFFLINE;
using xce::mediator::onlineevent::ONLINEWEBPAGGER;
using xce::mediator::onlineevent::ONLINEXNT;
using xce::mediator::onlineevent::ONLINEWAP;
using xce::mediator::onlineevent::OFFLINEWAP;

using namespace com::xiaonei::wap::adapter;
using mop::hi::svc::adapter::PresenceAdapter;
using mop::hi::svc::adapter::WEBPAGER_ONLINE;
using mop::hi::svc::adapter::TALK_ONLINE;
UserCacheData3Ptr Base2Cache(const int id)
{
	UserBaseFullViewPtr data;
	try
	{
		data = UserBaseAdapter::instance().getUserBaseFullView(id);
	}catch(...)
	{
		return NULL;
	}
	
	UserCacheData3Ptr res = new UserCacheData3;
	res->id = data->id();
	res->status = data->status();
	res->univ = data->univ();
	res->state = data->state();
	res->level = data->level();
	res->name = data->name();
	res->tinyUrl = data->tinyurl();
	string gg = data->gender();
	if (gg=="男生") {
		res->gender = 1;
	} else if (gg=="女生") {
		res->gender = 2;
	} else {
		res->gender = 0;
	}
	
	std::vector<int> ids;
	ids.push_back( id );
	try {
		std::map<int, int> webRes = mop::hi::svc::adapter::PresenceAdapter::instance().getOnlineStat(ids);
		std::map<int, int>::const_iterator findIt = webRes.find( data->id() );
		if( findIt == webRes.end() )
		{
			res->online = 0;
		}else
		{
			int online_cache = 0;					
			int online = 0;
			online = (findIt->second & WEBPAGER_ONLINE) ? ONLINEWEBPAGGER : OFFLINE;
			if (online > 0) 
			{
				online_cache |= (1 << (online - 1));
			}
			online = (findIt->second & TALK_ONLINE) ? ONLINEXNT: OFFLINE;
			if (online > 0) 
			{
				online_cache |= (1 << (online - 1));
			}

			res->online = online_cache;
		}
		
		map<int, int> wap_res = WapOnlineAdapter::instance().getOnlineStatus(ids);
		findIt = wap_res.find( data->id() );
		if( findIt == wap_res.end() )
		{
			MCE_WARN( "InsertCallGuarantee::Base2Cache no Wap online state id = "<<data->id() );
		}else
		{
			int online_cache = res->online;					
			int online = 0;
			online = (findIt->second & WAP_ONLINE) ? ONLINEWAP	: OFFLINEWAP;
			if (online > 0) 
			{
				online_cache |= (1 << (online - 1));
			}

			res->online = online_cache;
		}

	} catch(const Ice::Exception& e) {
		std::cout<<"Base2Cache Exception: "<<e.what()<<std::endl;
	} catch(const std::exception& e) {
		std::cout<<"Base2Cache Exception: "<<e.what()<<std::endl;
	} catch(...) {
		std::cout<<"Base2Cache Unknow Exception" << std::endl;
	}
	return res;
}

bool check ( int id, xce::distcache::usercacheadapter::UserCacheMemDataPtr dData, UserCacheData3Ptr uData , const string nameStr = "usercache")
{
	if( dData == NULL | uData == NULL )
	{
		return false;
	}
	bool isPrint = false;
	ostringstream os;

	os<<"check exception id "<<id;
	if( dData->id() != uData->id)
	{
		isPrint = true;
		os<<" distcache id = "<<dData->id()<<" but "<<nameStr<<" id = "<<uData->id;
	}
	if( dData->status() != uData->status)
	{
		isPrint = true;
		os<<" distcache status = "<<dData->status()<<" but "<<nameStr<<" status = "<<uData->status;
	}
	if( dData->univ() != uData->univ)
	{
		isPrint = true;
		os<<" distcache univ = "<<dData->univ()<<" but "<<nameStr<<" univ = "<<uData->univ;
	}
	if( dData->state() != uData->state)
	{
		isPrint = true;
		os<<" distcache state = "<<dData->state()<<" but "<<nameStr<<" state = "<<uData->state;
	}
	if( dData->level() != uData->level)
	{
		isPrint = true;
		os<<" distcache level = "<<dData->level()<<" but "<<nameStr<<" level = "<<uData->level;
	}
	if( dData->gender() != (int)uData->gender)
	{
		isPrint = true;
		os<<" distcache gender = "<<dData->gender()<<" but "<<nameStr<<" gender = "<<(int)uData->gender;
	}
	if( dData->online() != (int)uData->online)
	{
		isPrint = true;
		os<<" distcache online = "<<dData->online()<<" but "<<nameStr<<" online = "<<(int)uData->online;
	}
	if( dData->name() != uData->name)
	{
		isPrint = true;
		os<<" distcache name = "<<dData->name()<<" but "<<nameStr<<" name = "<<uData->name;
	}
	if( dData->tinyurl() != uData->tinyUrl)
	{ 
		isPrint = true;
		os<<" distcache tinyUrl = "<<dData->tinyurl()<<" but "<<nameStr<<" tinyUrl = "<<uData->tinyUrl;
	}

	if( isPrint )
	{
		cout<<os.str()<<endl;
	}

	return isPrint;
}

bitset<STATUS_CACHE_SIZE> idsStatus;
int main()
{
	DistUserCacheAdapter::instance().initialize();

	cout<<"get statuscache start"<<endl;
	while(true)
	{
		try
		{
			StatusCacheAdapter::instance().getAllStatus(idsStatus);
			break;
		}catch(Ice::Exception& e)
		{
			cout<< e.what()<<endl;
			getchar();
			continue;
		}catch(...)
		{
			cout<<"check exception statuscache"<<endl;
			getchar();
			continue;
		}
		getchar();
	}
	cout<<"get statuscache end"<<endl;

	//while(true)
	{
		cout<<"check start"<<endl;
		MyUtil::IntSeq ids;
		MyUtil::LongSeq idsL;
	
		int count = 0;

		for( int i = 0; i < STATUS_CACHE_SIZE; ++i )
		{
			if( idsStatus[i] == 0 )
			{
				ids.push_back( i );
				idsL.push_back( i );
				++count;
			}
	
			if( count >= 3000 )
			{	
				std::map<long,xce::distcache::usercacheadapter::UserCacheMemDataPtr> dMap;
				std::map<int,xce::usercache::UserCacheData3Ptr> uMap;
	
				try
				{
					MyUtil::TimeCost tc = MyUtil::TimeCost::create("new");
					dMap = DistUserCacheAdapter::instance().getDataWithCAS( idsL );
				}catch(Ice::Exception& e)
				{
					cout<< e.what()<<endl;
				}catch(...)
				{
					cout<<"get exception distcache"<<endl;
				}
	
				try
				{
					MyUtil::TimeCost tc = MyUtil::TimeCost::create("old");
					uMap = xce::usercache::UserCacheAdapter::instance().getProxy( i )->getUserCacheMap3( ids );
				}catch(Ice::Exception& e)
				{
					cout<< e.what()<<endl;
				}catch(...)
				{
					cout<<"get exception usercache"<<endl;
				}

				vector<int> loadIds;
				for( vector<int>::const_iterator it = ids.begin(); it != ids.end(); ++it )
				{
					map<long,xce::distcache::usercacheadapter::UserCacheMemDataPtr>::iterator findDIt = dMap.find( *it );
					map<int,xce::usercache::UserCacheData3Ptr>::iterator findUIt = uMap.find( *it );
					
					if( ( findDIt != dMap.end() ) && ( findUIt != uMap.end() ) )
					{
						if( check( *it, findDIt->second, findUIt->second ) )
						{
							if( check( *it, findDIt->second, Base2Cache( *it ), "userbase" ) )
							{
								cout<<" distcache wrong id "<<*it<<endl;
								loadIds.push_back(*it);
							}else
							{
								cout<<" usercache wrong id "<<*it<<endl;
							}
						}
						continue;
					}
					
					if( ( findUIt == uMap.end() ) && ( findDIt != dMap.end() ) )
					{
						cout<<"check exception id "<< *it << " usercache does not have but distcache has " <<endl;
					
					}else if( ( findUIt != uMap.end() ) && ( findDIt == dMap.end() ) )
					{
						cout<<"check exception id "<< *it << " distcache does not have but usercache has " <<endl;
					}else
					{
						cout<<"check exception id "<< *it << " distcache and usercache both do not have "<< endl;
					}
				}
					if(loadIds.size()>0)
					{
						try
						{
							xce::distcache::usercache::DistUserCacheReloaderAdapter::instance().getProxy(0)->reload( loadIds );
						}catch(...)
						{
						}
					}
				vector<int>().swap(ids);
				vector<long>().swap(idsL);
				cout<<"check max id "<<i<<endl;
				count = 0;
			}
		}
		cout<<"check end"<<endl;
	}
	
	return 1;
}
