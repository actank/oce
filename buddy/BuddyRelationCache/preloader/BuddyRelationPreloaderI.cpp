/*
 * =====================================================================================
 *
 *       Filename:  BuddyRelationPreloaderI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年07月15日 14时54分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#include "ServiceI.h"
#include "BuddyRelationCache.h"
#include "BuddyRelationPreloaderI.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "buddy/BuddyRelationCache/share/BuddyRelationShare.h"

using namespace MyUtil;
using namespace xce::generalpreloader;
using namespace xce::buddy;


void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&BuddyRelationPreloader::instance(), service.createIdentity("PL", ""));
		
        string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("BuddyRelationCache.Controller", "ControllerBuddyRelationCache");
        int cInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 60);
        int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyRelationCache.Proxy.Timeout", 300);
        int setDataQueueSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyRelationPreloader.SetDataQueueSize", 100000);
        int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyRelationPreloader.ProduceBatchSize", 10000);
        int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyRelationPreloader.ConsumeBatchSize", 3000);
        int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyRelationPreloader.WriteBatchSize", 3000);
        int threadSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyRelationPreloader.ThreadSize", 5);

	int cluster = 1;

	MCE_DEBUG("ctlEndpoint: " << ctlEndpoint << " cluster: " << cluster << " setDataQueueSize: " << setDataQueueSize);
	MCE_DEBUG("cInterval: " << cInterval << " timeout : " << timeout << " produceBatchSize: " << produceBatchSize);
	MCE_DEBUG("consumBatchSize: " << consumeBatchSize << "writeBatchSize: " << writeBatchSize << " threadSize: " << threadSize);
	GeneralPreloader<BuddyRelationCacheManagerPrx>::instance().initialize(
                new BatchUsersIdProducer,
                &BuddyRelationFactory::instance(),
                ctlEndpoint,
		cluster,
		cInterval,
                timeout,
		setDataQueueSize,
                produceBatchSize,
                consumeBatchSize ,
		writeBatchSize ,
                threadSize);
}
