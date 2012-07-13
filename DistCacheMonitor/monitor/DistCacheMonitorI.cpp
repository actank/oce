#include "DistCacheMonitorI.h"
#include "AlertManager.h"
#include <util/cpp/TimeCost.h>
#include <DistCacheMonitorAdapter.h>

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

void MyUtil::initialize() {
	MyUtil::ServiceI& service = MyUtil::ServiceI::instance();
	service.getAdapter()->add(&xce::distcache::monitor::DistCacheMonitorI::instance(), service.createIdentity(xce::distcache::monitor::DISTCACHE_MONITOR, ""));

	xce::distcache::monitor::DistCacheMonitorI::instance().m_idx = service.getCommunicator()->getProperties()->getPropertyAsInt("DistCacheMonitor.Index");
	
	xce::distcache::monitor::DistCacheMonitorI::instance().m_repNums = service.getCommunicator()->getProperties()->getPropertyAsInt("DistCacheMonitor.Replica");
	
	xce::distcache::monitor::AlertManager::instance().reloadConfig();
	
	xce::distcache::monitor::ServiceMonitor::instance().start().detach();
	
	xce::distcache::monitor::ZooKeeperMonitor::instance().start().detach();
	
	xce::distcache::monitor::DistCacheMonitorI::instance().start().detach();
	
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerDistCacheMonitor", &xce::distcache::monitor::DistCacheMonitorI::instance(), 0, 120);

}

void xce::distcache::monitor::DistCacheMonitorI::reloadConfig(const Ice::Current& current)
{
	std::ostringstream os;
	os<<"DistCacheMonitorI::reloadConfig";
	MyUtil::TimeCost tc = MyUtil::TimeCost::create(os.str());
	xce::distcache::monitor::AlertManager::instance().reloadConfig();
}

void xce::distcache::monitor::DistCacheMonitorI::setFlag( const std::string& nameStr, bool flag, const Ice::Current& current)
{
	std::ostringstream os;
	os<<"DistCacheMonitorI::setFlag service name = "<< nameStr<<" flag = "<<flag;
	MyUtil::TimeCost tc = MyUtil::TimeCost::create(os.str());
	xce::distcache::monitor::ServiceMonitor::instance().setFlag( nameStr, flag );
}

void xce::distcache::monitor::DistCacheMonitorI::run()
{
	while(true)
	{
		sleep(10);
		
		std::vector<xce::distcache::monitor::DistCacheMonitorPrx> prxs; 
		
		xce::distcache::monitor::DistCacheMonitorPrx checkPrx; 
		
		try
		{
			prxs = xce::distcache::monitor::DistCacheMonitorAdapter::instance().getProxySeq(0);

			if( prxs.size() != m_repNums )
			{
				std::ostringstream os;

				os<<" DistCacheMonitorI : repconfig is " << m_repNums <<" but now is "<< prxs.size();
				MCE_WARN(os.str());
				//xce::distcache::monitor::AlertManager::instance().alertForMonitorService( os.str() );
			}

			for( std::vector<DistCacheMonitorPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it )
			{
				checkPrx = (*it);
				(*it)->ice_ping();
			}
		}catch( Ice::Exception& e )
		{
			std::ostringstream os;

			os<<" DistCacheMonitorI : can not reache prx " << checkPrx->ice_toString()<<" for Exception : "<<e.what();
			
			MCE_WARN(os.str());
			//xce::distcache::monitor::AlertManager::instance().alertForMonitorService( os.str() );
		}catch( ... )
		{
			std::ostringstream os;

			os<<" DistCacheMonitorI : can not reache prx " << checkPrx->ice_toString()<<" for UnKnow Exception";
			
			MCE_WARN(os.str());
			//xce::distcache::monitor::AlertManager::instance().alertForMonitorService( os.str() );
		}
	}
}


