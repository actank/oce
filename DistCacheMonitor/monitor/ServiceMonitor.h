#ifndef __SERVICE_MONITOR_H__
#define __SERVICE_MONITOR_H__

#include <zookeeper/zookeeper.h>
#include <IceUtil/Mutex.h>
#include <math.h>
#include <boost/lexical_cast.hpp>

#include <ServiceI.h>

#include <DistCacheMonitor.h>

#include "distcache/ClusterManager.h"
#include "distcache/PreferManager.h"
#include "distcache/Distribution.h"

#include "DistCacheSwitcherAdapter.h"

#include "AlertManager.h"

namespace xce{
namespace distcache{
namespace monitor{ 

	class MemcachedStatsInfo : public Ice::Object
	{
		public:
			std::map<std::string, std::string> stats;
			int checkAlarmTime;

	};
	typedef IceUtil::Handle<MemcachedStatsInfo> MemcachedStatsInfoPtr;
	typedef std::map<std::string, MemcachedStatsInfoPtr> MemcachedStatsInfoMap;
	typedef std::map<int, MemcachedStatsInfoMap> MemcachedStatsInfoBlockMap;
	typedef std::map<std::string, xce::distcache::node::ICEWriteNodePtr> WriteNodesMap;

	class ServiceAgent : public Ice::Object
	{
		public:
			
			ServiceAgent(const std::string& name)
			{
				m_blockSize = m_clusterManager.initialize(name,"");
				m_name = name;
			}

		public:

			void setFlag( bool flag )
			{
				DistCacheSwitcherAdapter::instance().delFilter( m_name );
			}
			
			void check()
			{
				m_lastMemcachedStatsMap.swap(m_memcachedStatsMap);
				m_memcachedStatsMap.clear();
				m_lastWriteServicesMap.swap(m_writeServicesMap);
				m_writeServicesMap.clear();
				renewMemcachedStats();
				renewWriteServices();
				alarmIfNeedMemcached();
				alarmIfNeedWriteServices();
			}

			void renewWriteServices()
			{
				std::vector<xce::distcache::node::WriteNodePtr> writeServices = m_clusterManager.getWriteNodes();

				WriteNodesMap tempAlarmMap;

				for(std::vector<xce::distcache::node::WriteNodePtr>::const_iterator it = writeServices.begin(); it != writeServices.end(); ++it )
				{
					m_writeServicesMap.insert(make_pair( (*it)->getName(), boost::dynamic_pointer_cast<xce::distcache::node::ICEWriteNode>(*it)));

					WriteNodesMap::iterator findIt = m_writeServicesAlarmFilter.find( (*it)->getName() );

					if( findIt != m_writeServicesAlarmFilter.end() )
					{
						tempAlarmMap.insert( make_pair( findIt->first, findIt->second ) );
					}
				}

				m_writeServicesAlarmFilter.swap( tempAlarmMap );
			}
			
			void renewMemcachedStats()
			{
				std::map<int, MemcachedStatsInfoMap> tempAlarmMap;

				for( int i = 0; i< m_blockSize; ++i )
				{
					m_memcachedStatsMap.insert( make_pair( i, MemcachedStatsInfoMap() ) );
					
					std::vector<xce::distcache::node::ReadNodePtr> nodes = m_clusterManager.getReadNodes( i );

					for( std::vector<xce::distcache::node::ReadNodePtr>::const_iterator it = nodes.begin(); it != nodes.end(); ++it )
					{
						std::string name = (*it)->getName();
					
						MemcachedStatsInfoBlockMap::iterator findMIt = m_memcachedAlarmFilter.find( i );  
						if( findMIt != m_memcachedAlarmFilter.end() )
						{
							if( tempAlarmMap.find( i ) == tempAlarmMap.end() )
							{
								tempAlarmMap.insert( make_pair( i, MemcachedStatsInfoMap() ) );
							}
							
							if( findMIt->second.find( name ) != findMIt->second.end() )
							{
								tempAlarmMap.at( i ).insert( *( findMIt->second.find( name ) ) );
							}
						}
					
						MemcachedStatsInfoBlockMap::iterator findLIt = m_lastMemcachedStatsMap.find( i );
						if( findLIt != m_lastMemcachedStatsMap.end() )
						{
							if( findLIt->second.find( name ) == findLIt->second.end() )
							{
								std::ostringstream os;
								os<<"Service Name : " <<m_name<<" Block : "<<i<<" ReadNode : "<< name <<" : "<< " new added!";;
								MCE_WARN(os.str());
							}
						}else
						{
							std::ostringstream os;
							os<<"Service Name : "<<m_name<<" Block : "<<i<<" new added!";;
							MCE_WARN(os.str());
						}
						
						MemcachedStatsInfoPtr stats = new MemcachedStatsInfo;
						stats->checkAlarmTime = 0;
						try
						{
							if((*it)->getStats( stats->stats ) == false)
							{
								//to do alarm
								//m_memcachedAlarmFilter = tempAlarmMap;
								std::ostringstream os;
								os<<"Service Name : " <<m_name<<" ReadNode : "<< name <<" can not getStats!";
								MCE_WARN( os.str() );
								//xce::distcache::monitor::AlertManager::instance().alert( os.str() );
								continue;
								//confirmChangeReadNode( i, name, os );
								//tempAlarmMap = m_memcachedStatsMap;
							}
						}catch(...)
						{
							std::ostringstream os;
							os<<"Service Name : " <<m_name<<" ReadNode : "<< name <<" can not getStats!";
							MCE_WARN( os.str() );
							//xce::distcache::monitor::AlertManager::instance().alert( os.str() );
							continue;
						}
						m_memcachedStatsMap.at( i ).insert( make_pair(name,stats) );
					}
				}

				m_memcachedAlarmFilter.swap( tempAlarmMap );
			}
			
		private:
		
			bool checkStatus( MyUtil::Str2StrMap& newStats, MyUtil::Str2StrMap& oldStats , std::ostringstream& os )
			{
				bool flag = false;

				float newL = 0;
				float oldL = 0;
				for(MyUtil::Str2StrMap::const_iterator nIt = newStats.begin(); nIt != newStats.end(); ++nIt)
				{
					if(nIt->second.find("get_misses") != std::string::npos)
					{
						MyUtil::Str2StrMap::const_iterator oIt = oldStats.find( nIt->first );
						if(oIt == oldStats.end())
						{
							flag = true;
							break;
						}
						newL = boost::lexical_cast<float>( nIt->second );
						oldL = boost::lexical_cast<float>( oIt->second );
					
						if( ( (float) abs( newL - oldL )/ oldL ) > 0.2 )
						{
							os<<" get_misses new "<<newL<<" old "<<oldL;
							flag = true;
						}
						continue;
					}
					
					if(nIt->second.find("pid") != std::string::npos)
					{
						MyUtil::Str2StrMap::const_iterator oIt = oldStats.find( nIt->first );
						if(oIt == oldStats.end())
						{
							flag = true;
							break;
						}
						newL = boost::lexical_cast<float>( nIt->second );
						oldL = boost::lexical_cast<float>( oIt->second );
					
						if(  newL != oldL )
						{
							os<<" pid new "<<newL<<" old "<<oldL;
							flag = true;
						}
						continue;
					}
					
					if(nIt->second.find("evictions") != std::string::npos)
					{
						newL = boost::lexical_cast<int>( nIt->second );
					
						if(  newL != 0 )
						{
							os<<" evictions new "<<newL;
							flag = true;
						}
						continue;
					}
				
					if(nIt->second.find("curr_connections") != std::string::npos)
					{
						MyUtil::Str2StrMap::const_iterator oIt = oldStats.find( nIt->first );
						if(oIt == oldStats.end())
						{
							flag = true;
							break;
						}
						newL = boost::lexical_cast<float>( nIt->second );
						oldL = boost::lexical_cast<float>( oIt->second );
					
						if( ( (float) abs( newL - oldL )/ oldL ) > 0.2 )
						{
							os<<" curr_connections new "<<newL<<" old "<<oldL;
							flag = true;
						}
						continue;
					}
					
					if(nIt->second.find("threads") != std::string::npos)
					{
						MyUtil::Str2StrMap::const_iterator oIt = oldStats.find( nIt->first );
						if(oIt == oldStats.end())
						{
							flag = true;
							break;
						}
						newL = boost::lexical_cast<float>( nIt->second );
						oldL = boost::lexical_cast<float>( oIt->second );
					
						if( ( (float) abs( newL - oldL )/ oldL ) > 0.2 )
						{
							os<<" threads new "<<newL<<" old "<<oldL;
							flag = true;
						}
						continue;
					}
				}
				
				if( flag )
				{
					return true;
				}
				
				return false;
			}
			
			void alarmIfNeedMemcached()
			{
				for( int i = 0; i< m_blockSize; ++i )
				{
					MemcachedStatsInfoBlockMap::iterator findLIt = m_lastMemcachedStatsMap.find( i );
			
					if(m_memcachedStatsMap.find(i) == m_memcachedStatsMap.end())
					{
						continue;
					}
					
					for( MemcachedStatsInfoMap::const_iterator it = m_memcachedStatsMap.at(i).begin(); it != m_memcachedStatsMap.at(i).end(); ++it )
					{
						std::ostringstream os;
						os<<"Service Name : " <<m_name<<" Block : "<<i<<" ReadNode : "<<it->first<<" : ";
						if( findLIt != m_lastMemcachedStatsMap.end() )
						{
							MemcachedStatsInfoMap::iterator findIt = m_lastMemcachedStatsMap.at(i).find( it->first );
							if( findIt != m_lastMemcachedStatsMap.at(i).end() )
							{
								if( checkStatus( it->second->stats, findIt->second->stats, os) )
								{
									// To do
									MCE_WARN( os.str() );
									xce::distcache::monitor::AlertManager::instance().alert( os.str() );
									//confirmChangeReadNode( it->first, nIt->first ,os );
								}
		
								{
		
									for( MyUtil::Str2StrMap::const_iterator sIt = it->second->stats.begin(); sIt != it->second->stats.end(); ++sIt )
									{
										if( sIt->first == "cmd_get" )
										{
											MCE_WARN(boost::lexical_cast<long>(sIt->second)-boost::lexical_cast<long>(findIt->second->stats.find( sIt->first )->second ) );
										}
										os<<sIt->first<<" new "<<sIt->second<<" old "<<findIt->second->stats.find( sIt->first )->second<<"\n";
									}
									MCE_INFO( os.str() );
								}
							}else
							{
								os<<" new added in new config!";
								MCE_INFO(os.str());
								{
									for( MyUtil::Str2StrMap::const_iterator sIt = it->second->stats.begin(); sIt != it->second->stats.end(); ++sIt )
									{
										os<<" "<<sIt->first<<" new "<<sIt->second<<" old no "<<"\n";
									}
									MCE_INFO( os.str() );
								}
							}
						}else
						{
							os<<" new add block in new config!";
							MCE_INFO( os.str() );
						}
					}
				
					if( findLIt != m_lastMemcachedStatsMap.end() )
					{
						for( MemcachedStatsInfoMap::const_iterator it = m_lastMemcachedStatsMap.at(i).begin(); it != m_lastMemcachedStatsMap.at(i).end(); ++it )
						{
							std::ostringstream os;
							os<<"Service Name : " <<m_name<<" Block : "<<i<<" ReadNode : "<<it->first<<" : ";
							MemcachedStatsInfoMap::iterator findIt = m_memcachedStatsMap.at(i).find( it->first );
							if( findIt == m_memcachedStatsMap.at(i).end() )
							{
								os<<" not in new config!";
								MCE_WARN(os.str());
								//xce::distcache::monitor::AlertManager::instance().alert( os.str() );
								{
									for( MyUtil::Str2StrMap::const_iterator sIt = it->second->stats.begin(); sIt != it->second->stats.end(); ++sIt )
									{
										os<<" "<<sIt->first<<" new no "<<" old "<<sIt->second<<"\n";
									}
									MCE_INFO( os.str() );
								}
							}
						}
					}
				}

				for( MemcachedStatsInfoBlockMap::const_iterator it = m_lastMemcachedStatsMap.begin(); it != m_lastMemcachedStatsMap.end(); ++it )
				{
					if( m_memcachedStatsMap.find( it->first ) == m_memcachedStatsMap.end() )
					{
						std::ostringstream os;
						os<<"Service Name : " <<m_name<<" Block : "<<it->first<<" old hava but new no!";
						MCE_WARN( os.str() );
					}
				}
			}
			
			void alarmIfNeedWriteServices()
			{
				for( WriteNodesMap::const_iterator it = m_writeServicesMap.begin(); it != m_writeServicesMap.end(); ++it )
				{
					std::ostringstream os;
					os<<"Service Name : " <<m_name<<" WriteNode : "<<it->first<<" : ";
					WriteNodesMap::iterator findIt = m_lastWriteServicesMap.find( it->first );

					if( findIt == m_lastWriteServicesMap.end() )
					{
						os<<"new added! ";
						MCE_WARN(os.str());
					}
					
					try
					{
						it->second->getPrx()->ice_ping();
					}catch(Ice::Exception& e)
					{
						os<<" Ice Exception : "<<e.what() << " ! ";

						MCE_WARN(os.str());
						xce::distcache::monitor::AlertManager::instance().alert( os.str() );
						//confirmChangeWriteNode( it->first, os );
					}catch(...)
					{
						os<<" UnKnown Exception ! ";
					
						MCE_WARN(os.str());
						xce::distcache::monitor::AlertManager::instance().alert( os.str() );
						//confirmChangeWriteNode( it->first, os );
					}

				}
				
				for( WriteNodesMap::const_iterator it = m_lastWriteServicesMap.begin(); it != m_lastWriteServicesMap.end(); ++it )
				{
					std::ostringstream os;
					os<<"Service Name : " <<m_name<<" WriteNode : "<<it->first<<" : ";
					WriteNodesMap::iterator findIt = m_writeServicesMap.find( it->first );
					if( findIt == m_writeServicesMap.end() )
					{
						//alarm service died
						os<<"No prx in new config!";
						MCE_WARN(os.str());
						xce::distcache::monitor::AlertManager::instance().alert( os.str() );
					}
				}
			}

			bool confirmChangeWriteNode( const std::string& name , std::ostringstream& os )
			{
				if( m_writeServicesAlarmFilter.find( name ) == m_writeServicesAlarmFilter.end() )
				{
					m_writeServicesAlarmFilter.insert( make_pair( name, xce::distcache::node::ICEWriteNodePtr(new xce::distcache::node::ICEWriteNode)) );
					os<<" first time!";
					MCE_WARN(os.str());
				}else
				{
					os<<" second time delete and Alarm !";
					if( m_clusterManager.getWriteNodesOffline().size() <= 0 )
					{
						//To do
						os<<" Only one writeservice, do not delete!";
						MCE_WARN( os.str() );
						xce::distcache::monitor::AlertManager::instance().alert( os.str() );
						return false;
					}
				}
				
				if( DistCacheSwitcherAdapter::instance().changeNodeStatus(m_name,name, 0, OFFLINE, false) )
				{
					if( DistCacheSwitcherAdapter::instance().changeNodeStatus(m_name, m_clusterManager.getWriteNodesOffline().at(0)->getName(), 0, ONLINE , false) )
					{
						renewWriteServices();
						alarmIfNeedWriteServices();
						os<<" Delete success!";
						xce::distcache::monitor::AlertManager::instance().alert( os.str() );
						MCE_WARN(os.str());
						return true;
					}else
					{
						// To do alarm
						os<<" Can not upgrade new online write service!";
						xce::distcache::monitor::AlertManager::instance().alert( os.str() );
						MCE_WARN(os.str());
						return false;
					}
				}else
				{
					// To do alarm
					os<<"Can not delete old online write service!";
					xce::distcache::monitor::AlertManager::instance().alert( os.str() );
					MCE_WARN(os.str());
					return false;
				}
				return false;
			}

			bool confirmChangeReadNode( const int block, const std::string& name , std::ostringstream& os)
			{
				MemcachedStatsInfoBlockMap::iterator findIt = m_memcachedAlarmFilter.find( block );

				if( findIt != m_memcachedAlarmFilter.end() )
				{
					MemcachedStatsInfoMap::iterator findMIt = findIt->second.find( name );

					if( findMIt != findIt->second.end() )
					{
						os<<" second time delete and Alarm !";
						if( m_memcachedStatsMap.at(block).size() <= 1 )
						{
							os<<" Only one server not delete!";
							//to do
							xce::distcache::monitor::AlertManager::instance().alert( os.str() );
							MCE_WARN( os.str() );
							return false;
						}
					}else
					{
						os<<" first time not delete!";
						findIt->second.insert( make_pair( name, new MemcachedStatsInfo ) );
						MCE_WARN( os.str() );
						return false;
					}
				}else
				{
					os<<" first time not delete!";
					MemcachedStatsInfoMap tempMap;
					tempMap.insert( make_pair( name, new MemcachedStatsInfo ) );
					m_memcachedAlarmFilter.insert( make_pair( block, tempMap ) );
					MCE_WARN( os.str() );
					return false;
				}
				
				if( DistCacheSwitcherAdapter::instance().changeNodeStatus(m_name,name, block, OFFLINE, true) )
				{
					os<<"Delete old online read service!";
					xce::distcache::monitor::AlertManager::instance().alert( os.str() );
					MCE_WARN(os.str());
				}else
				{
					// To do alarm
					os<<"Can not delete old online read service!";
					MCE_WARN(os.str()); 
					xce::distcache::monitor::AlertManager::instance().alert( os.str() );
					return false;
				}
				return true;

			}
			
		private: 
		
			xce::distcache::ClusterManager m_clusterManager; 
			int m_blockSize;
			std::string m_name;
			MemcachedStatsInfoBlockMap m_memcachedStatsMap;
			MemcachedStatsInfoBlockMap m_lastMemcachedStatsMap;
			MemcachedStatsInfoBlockMap m_memcachedAlarmFilter;
			
			WriteNodesMap m_writeServicesMap; 
			WriteNodesMap m_lastWriteServicesMap; 
			WriteNodesMap m_writeServicesAlarmFilter; 
	};

	typedef IceUtil::Handle<ServiceAgent> ServiceAgentPtr;
	typedef std::map<std::string, ServiceAgentPtr> AgentMap;

	class ServiceMonitor : public IceUtil::Thread,  virtual public MyUtil::Singleton<ServiceMonitor> 
	{
		public:
			ServiceMonitor()
			{
				try
				{
					m_zk = zookeeper_init("UserZooKeeper1:2181,UserZooKeeper2:2181,UserZooKeeper3:2181,UserZooKeeper4:2181,UserZooKeeper5:2181/DistCache", NULL, 10000, 0, 0, 0);
				}catch(...)
				{
					MCE_WARN("ServiceMonitor : Can't init zk!");
				}
				
			}

		public:

			void setFlag( const std::string& nameStr, bool flag )
			{
				IceUtil::Mutex::Lock lock( m_mutex );
				
				AgentMap::const_iterator findIt = m_agentMap.find( nameStr );

				if( findIt != m_agentMap.end() )
				{
					findIt->second->setFlag( flag );
				}
			}
		
		protected:
		
			void renew()
			{
				struct String_vector node_names;
				
				int rc = zoo_get_children(m_zk, "/", NULL, &node_names);
				if (rc != ZOK) 
				{
					MCE_WARN("Can not get children from zk!");
					return;
					//To do
				}

				AgentMap tempMap;

				for (int node_name = 0; node_name < node_names.count; ++node_name)
				{
					AgentMap::iterator findIt = m_agentMap.find( node_names.data[node_name] );  
					if( findIt == m_agentMap.end() )
					{
						ServiceAgentPtr agent = new ServiceAgent(node_names.data[node_name]);

						agent->renewWriteServices();
						
						agent->renewMemcachedStats();

						tempMap.insert( make_pair( node_names.data[node_name], agent ) );
					}else
					{
						tempMap.insert( *findIt );
					}
				}

				m_agentMap.swap( tempMap );
			}
			
			void run()
			{
				while(true)
				{
					sleep( 10 );

					{
						IceUtil::Mutex::Lock lock( m_mutex );
						renew();
						for( AgentMap::const_iterator it = m_agentMap.begin(); it != m_agentMap.end(); ++it )
						{
							it->second->check();
						}
					}
				}
			}
		
		private: 

			IceUtil::Mutex m_mutex;

			zhandle_t* m_zk;

			AgentMap m_agentMap;
	};

}
}
}

#endif
