#include "DistUserCacheReloaderAdapter.h"

xce::distcache::usercache::DistUserCacheReloaderAdapter::DistUserCacheReloaderAdapter() : MyUtil::ReplicatedClusterAdapterI <xce::usercache::DistUserCacheReloaderPrx>("ControllerDistUserCacheReloader", 120, 300) 
{
	start(1024*1024).detach();
}

void xce::distcache::usercache::DistUserCacheReloaderAdapter::reload( const MyUtil::IntSeq& ids )
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
	if( m_reloadIds.size() + ids.size() > xce::distcache::usercache::RELOAD_MAZ_LENGTH )
	{
		MCE_WARN("DistUserCacheReloaderAdapter::reload too many ids!");
		return;
	}
	m_reloadIds.insert( m_reloadIds.end(), ids.begin(), ids.end() );
	m_mutex.notify();
}

void xce::distcache::usercache::DistUserCacheReloaderAdapter::run()
{
	while( true )
	{
		MyUtil::IntSeq ids;
		{
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(m_mutex);
			if( m_reloadIds.size() == 0 )
			{
				m_mutex.wait();
			}
			ids.swap( m_reloadIds );
		}
		try
		{
			getProxy(0)->reload( ids );
		} catch (const Ice::Exception& e) {
      MCE_WARN("DistUserCacheReloaderAdapter::run "<< e);
    } catch (const std::exception& e) {
      MCE_WARN("DistUserCacheReloaderAdapter::run "<< e.what());
    } catch(...) {
			MCE_WARN("DistUserCacheReloaderAdapter::run Exception!");
		}
	}
}
