#ifndef __BUDDY_SIXDEGREES_STARTSTOPFLAG_I_H__
#define __BUDDY_SIXDEGREES_STARTSTOPFLAG_I_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <string>
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ObjectCacheI.h"
#include <tcrdb.h>
#include <iostream>
#include <set>

namespace xce {
namespace buddy {

/*****************************************************************************/

/**
 *对host和guest生成一个统一ID
 */
class CreateGlodId
{
public:
	static long getGlodId(int host, int guest) {
		return ((long)host << 32 ) + guest ;
	}
};

/**
 *控制各种计算开始和结束的类
 */
class StartStopFlag : virtual public IceUtil::RWRecMutex , virtual public MyUtil::Singleton<StartStopFlag> 
{
public : 
	bool start(int host , int guest)
	{
		IceUtil::RWRecMutex::WLock lock(*this);
		//----------------insert global id-------------------  
		long id = CreateGlodId::getGlodId(host, guest);
		{	
			std::map<long, std::set<long> >::iterator it1= hostAndGuest_.find(id);
			if(it1 != hostAndGuest_.end())
			{
				MCE_INFO("StartStopFlag::start the host: " << host << "guest: " << guest << "already runing" );
				return false; 

			}
			std::set<long> idSet;
			idSet.insert(host);
			idSet.insert(guest);

			hostAndGuest_.insert(make_pair(id,idSet));
		}

       	//----------------insert host id-------------------  
		{	
			std::map<long, std::set<long> >::iterator it1= hostAndGuest_.find(host);
			if(it1 != hostAndGuest_.end())
			{
				MCE_INFO("StartStopFlag::start the host: " << host<< "already runing" );
				it1->second.insert(id);

			}else{
				std::set<long> idSet;
				idSet.insert(id);
				hostAndGuest_.insert(make_pair(host,idSet));
			}
		}

		//----------------insert guest id-------------------  
		{	
			std::map<long, std::set<long> >::iterator it1= hostAndGuest_.find(guest);
			if(it1 != hostAndGuest_.end())
			{
				MCE_INFO("StartStopFlag::start the  guest: " << guest << "already runing" );
				it1->second.insert(id);

			}else{
				std::set<long> idSet;
				idSet.insert(id);
				hostAndGuest_.insert(make_pair(guest,idSet));
			}
		}
		return true;

	}



	bool stop(int host ,int guest)
        {
		IceUtil::RWRecMutex::WLock lock(*this);
		long id = CreateGlodId::getGlodId(host, guest);
		//----------------delete host<<32 + guest id-------------------  
		{	
			std::map<long, std::set<long> >::iterator it1= hostAndGuest_.find(id);
			if(it1 != hostAndGuest_.end())
			{
				hostAndGuest_.erase(it1);
				MCE_INFO("StartStopFlag::start the host: " << host << "guest: " << guest <<  "stop ok!" );

			}
		}

		//----------------delete host id-------------------  
		{	
			std::map<long, std::set<long> >::iterator it1= hostAndGuest_.find(host);
			if(it1 != hostAndGuest_.end())
			{

				std::set<long>::iterator it2 = it1->second.find(id);
				if(it2 != it1->second.end())
				{
					it1->second.erase(it2);
					MCE_INFO("StartStopFlag::start the host: " << host  <<  "build stop ok!" );
					if(it1->second.size()==0)
					{
						hostAndGuest_.erase(it1);
					}
				}

			}
			MCE_INFO("StartStopFlag::start the host: " << host  <<  "build stop ok!" );
		}

		//----------------delete guest id-------------------  
		{	
			std::map<long, std::set<long> >::iterator it1= hostAndGuest_.find(guest);
			if(it1 != hostAndGuest_.end())
			{

				std::set<long>::iterator it2 = it1->second.find(id);
				if(it2 != it1->second.end())
				{
					it1->second.erase(it2);
					MCE_INFO("StartStopFlag::start the guest: " << guest  <<  "build stop ok!" );
					if(it1->second.size()==0)
					{
						hostAndGuest_.erase(it1);
					}
				}

			}
			MCE_INFO("StartStopFlag::start the guest: " << guest  <<  "build stop ok!" );
		}

		return true;
	}

	bool isHostAndGuestStop(int host , int guest)
	{
		IceUtil::RWRecMutex::RLock lock(*this);
		long id = CreateGlodId::getGlodId(host, guest);
		//----------------insert host<<32 + guest id-------------------  
		std::map<long, std::set<long> >::iterator it1= hostAndGuest_.find(id);
		if(it1 != hostAndGuest_.end())
		{
			return false; 
		}
		return true;

	}

	bool isBuildIndexStop(int id)
	{
		IceUtil::RWRecMutex::RLock lock(*this);
		//----------------insert host<<32 + guest id-------------------  
		std::map<long, std::set<long> >::iterator it1= hostAndGuest_.find(id);
		if(it1 != hostAndGuest_.end())
		{
			return false; 
		}
		return true;

	} 


private :
	std::map<long, std::set<long> > hostAndGuest_;

};

typedef IceUtil::Handle<StartStopFlag> StartStopFlagPtr;


}
}

#endif

