#ifndef __DISTCACHESWITCHER_I_H__
#define __DISTCACHESWITCHER_I_H__

#include <zookeeper/zookeeper.h>
#include <IceUtil/Mutex.h>
#include <math.h>
#include <boost/lexical_cast.hpp>

#include <ServiceI.h>

#include <DistCacheMonitor.h>

#include "distcache/ClusterManager.h"
#include "distcache/PreferManager.h"
#include "distcache/Distribution.h"

namespace xce{
namespace distcache{
namespace monitor{ 

	const string DISTCACHE_SWITCHER = "M";
	class DistCacheSwitcherI : public DistCacheSwitcher, virtual public MyUtil::Singleton<DistCacheSwitcherI> 
	{
		public:
			DistCacheSwitcherI()
			{
				try
				{
					m_zk = zookeeper_init("u24:2181,u25:2181,u25:2181,u26:2181,u27:2181,u28:2181,u29:2181/DistCache", NULL, 10000, 0, 0, 0);
				}catch(...)
				{
					MCE_WARN("DistCacheSwitcherI : Can't init zk!");
				}
				
			}

		public:

			bool changeNodeStatus( const std::string& serviceName, const std::string& nodeName, int block, const NodeStatus status, bool rwFlag, const Ice::Current& cur )
			{
				if( rwFlag )
				{
					if( !checkFilter(serviceName, block) )
					{
						return true;
					}
				}
				
				xce::distcache::ClusterManager cm; 
				cm.initialize(serviceName,"");
				std::ostringstream os;
				switch( status )
				{
					case ONLINE:
						if( cm.setStatus(nodeName, xce::distcache::ONLINE) )
						{
							os<<"Service : "<<serviceName<<" Node : "<<nodeName<<" set status "<<status<<" success!";
							MCE_WARN(os.str());
							return true;
						}else
						{
							// To do alarm
							os<<"Service : "<<serviceName<<" Node : "<<nodeName<<" set status "<<status<<" failed!";
							MCE_WARN(os.str());
							return false;
						}
						break;
					case OFFLINE:
						if( cm.setStatus(nodeName, xce::distcache::OFFLINE) )
						{
							os<<"Service : "<<serviceName<<" Node : "<<nodeName<<" set status "<<status<<" success!";
							MCE_WARN(os.str());
							return true;
						}else
						{
							// To do alarm
							os<<"Service : "<<serviceName<<" Node : "<<nodeName<<" set status "<<status<<" failed!";
							MCE_WARN(os.str());
							return false;
						}
						break;
					case UPGRADING:
						if( cm.setStatus(nodeName, xce::distcache::UPGRADING) )
						{
							os<<"Service : "<<serviceName<<" Node : "<<nodeName<<" set status "<<status<<" success!";
							MCE_WARN(os.str());
							return true;
						}else
						{
							// To do alarm
							os<<"Service : "<<serviceName<<" Node : "<<nodeName<<" set status "<<status<<" failed!";
							MCE_WARN(os.str());
							return false;
						}
						break;
				}
				return false;

			}

			bool delFilter( const std::string& serviceName, const Ice::Current& cur )
			{
				IceUtil::Mutex::Lock lock( m_mutex );

				std::map<std::string, std::map<int,bool> >::iterator findIt = m_filter.find( serviceName );

				if( findIt != m_filter.end() )
				{
					m_filter.erase( findIt );
					return true;
				}else
				{
					return false;
				}

				return false;
			}
		
		private:

			bool checkFilter( const std::string& serviceName , const int block )
			{
				IceUtil::Mutex::Lock lock( m_mutex );

				if( m_filter.find( serviceName ) == m_filter.end() )
				{
					std::map<int,bool> blockMap;
					blockMap.insert( make_pair( block, true) );
					m_filter.insert( make_pair( serviceName, blockMap ) );
					return true;
				}else
				{
					std::pair< std::map<int,bool>::iterator, bool > res = m_filter.at(serviceName).insert( make_pair( block, true) );
					return res.second;
				}

				return false;
			}
		
		private: 

			zhandle_t* m_zk;
			
			IceUtil::Mutex m_mutex;

			std::map<std::string, std::map<int,bool> > m_filter;

	};

}
}
}

#endif
