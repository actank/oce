#ifndef _MEMCACHED_CLIENT_POOL_H__
#define _MEMCACHED_CLIENT_POOL_H__

#include <queue>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <libmemcached/memcached.h>

#include <LogWrapper.h>

namespace xce
{
namespace distcache
{
namespace node
{
	class MemcachedClientAgent
	{
		public:
			~MemcachedClientAgent()
			{
				if( client )
				{
					memcached_free(client);
				}
			}
		public:
			bool isUsed; 
			memcached_st *client;
	};

	typedef boost::shared_ptr<MemcachedClientAgent> MemcachedClientAgentPtr;
	
	class MemcachedClientPool
	{
		public:
			MemcachedClientPool(int poolSize, const std::string ipStr, const int portNum)
			{
				boost::unique_lock<boost::mutex> lock(m_mutex);
			
				for( int i = 0; i < poolSize; ++i )
				{
					MemcachedClientAgentPtr agentPtr = createAgent( ipStr, portNum );
					m_agentQueue.push( agentPtr );
				}

				m_currentPoolSize = poolSize; 
			}
		
		public:
			static MemcachedClientAgentPtr createAgent( const std::string ipStr, const int portNum )
			{
				memcached_st* client = memcached_create(NULL);
				memcached_return result = memcached_server_add(client, ipStr.c_str(), portNum);
				
				if (result != MEMCACHED_SUCCESS) {
					MCE_WARN("MemcachedClientAgentPtr::createAgent Error while construct MemcachedClient, see stderr for stack detail!");
					throw std::runtime_error("Error while construct MemcachedClient, see stderr for stack detail.");
				}

				int rc = memcached_behavior_set( client, MEMCACHED_BEHAVIOR_TCP_KEEPALIVE, true );

				if( rc != MEMCACHED_SUCCESS && rc != MEMCACHED_NOT_SUPPORTED )
				{
					MCE_WARN(" MemcachedClientAgentPtr::createAgent Can not set keepalive to memcached client!");
				}
				
				rc = memcached_behavior_set( client, MEMCACHED_BEHAVIOR_SUPPORT_CAS, true );

				if( rc != MEMCACHED_SUCCESS && rc != MEMCACHED_NOT_SUPPORTED )
				{
					MCE_WARN(" MemcachedClientAgentPtr::createAgent Can not set support cas to memcached client!");
				}
				
				rc = memcached_behavior_set( client, MEMCACHED_BEHAVIOR_TCP_NODELAY, true );

				if( rc != MEMCACHED_SUCCESS && rc != MEMCACHED_NOT_SUPPORTED )
				{
					MCE_WARN(" MemcachedClientAgentPtr::createAgent Can not set tcp nodelay to memcached client!");
				}
				
				MemcachedClientAgentPtr agentPtr( new MemcachedClientAgent );
				agentPtr->isUsed = false;
				agentPtr->client = client;
				
				return agentPtr;
			}
		
		public:
			bool get(MemcachedClientAgentPtr& res)
			{
				boost::unique_lock<boost::mutex> lock(m_mutex);

				if( m_currentPoolSize == 0 )
				{
					MCE_WARN(" MemcachedClientAgentPtr::get now size = "<<m_currentPoolSize );
					return false;
				}
				
				res = m_agentQueue.front();
				m_agentQueue.pop();
				--m_currentPoolSize;
				MCE_DEBUG(" MemcachedClientAgentPtr::get now size = "<<m_currentPoolSize );
				return true;

			}
			
			void pushBack(MemcachedClientAgentPtr agentPtr)
			{
				boost::unique_lock<boost::mutex> lock(m_mutex);
				m_agentQueue.push( agentPtr );
				++m_currentPoolSize;
				MCE_DEBUG(" MemcachedClientAgentPtr::pushBack now size = "<<m_currentPoolSize );
			}
		
		private:
			boost::mutex m_mutex;
			int m_currentPoolSize;
			std::queue<MemcachedClientAgentPtr> m_agentQueue;
	};
	
	typedef boost::shared_ptr<MemcachedClientPool> MemcachedClientPoolPtr;
}
}
}

#endif
