#ifndef __OBJECTCACHE_ADAPTER_H__
#define __OBJECTCACHE_ADAPTER_H__

#include <ObjectCache.h>

#include <Channel.h>

#include <iostream>

#include <LogWrapper.h>

namespace xce
{
	namespace adapter
	{
		namespace objectcache
		{
			class ObjectCacheAdapter
			{
				public:

				static void reload( int userId, int cluster, int level, const string& endpoints, const MyUtil::ChannelPtr& channel)
				{
					int mod = userId % (cluster-1);

					ostringstream os;

					os<< endpoints << mod; 
				
					try
					{
						channel->uncheckedCast<MyUtil::ObjectCachePrx>(channel->locate("SC", os.str(), MyUtil::TWO_WAY, 300))->reloadObject( level, userId);
					}catch(...)
					{
						MCE_WARN("ObjectCacheAdatper::reload Exception can't reload id = " << userId << " for endpoints = " <<endpoints);
					}
				}
				
				static void reload( const MyUtil::IntSeq& userIds, int cluster, int level, const string& endpoints, const MyUtil::ChannelPtr& channel)
				{
					std::vector<long> idChunk[cluster];

					for(MyUtil::IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it)
					{
						idChunk[(*it) % (cluster-1) ].push_back(*it);
					}

					for(int i =0; i < cluster; ++i)
					{
						if( idChunk[i].size() > 0)
						{
							ostringstream os;
		
							os<< endpoints << i; 
						
							try
							{
								channel->uncheckedCast<MyUtil::ObjectCachePrx>(channel->locate("SC", os.str(), MyUtil::TWO_WAY, 300))->reloadObjects( level, idChunk[i]);
							}catch(...)
							{
								ostringstream warnOs;
								for( std::vector<long>::const_iterator it = idChunk[i].begin(); it != idChunk[i].end(); ++it)
								{
									warnOs << *it <<" , ";
								}
								MCE_WARN("ObjectCacheAdatper::reload Exception can't reload idlist = " << os.str() << " for endpoints = " <<endpoints);
							}
						}
					}
				}
			};
		}
	}
}

#endif
