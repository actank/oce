/*
 * =====================================================================================
 *
 *       Filename:  BuddyByApplicantPreloader.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年06月28日 12时04分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#include "ServiceI.h"
#include "BuddyByApplicantPreloaderI.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "buddy/BuddyByApplicantCache/share/BuddyByApplicantFactory.h"

using namespace MyUtil;
using namespace xce::generalpreloader;
using namespace xce::buddy;


void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&BuddyByApplicantPreloader::instance(), service.createIdentity("PL", ""));
		
        string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("BuddyByApplicantCache.Controller", "ControllerBuddyByApplicantCache");
        int cInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 60);
        int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByApplicantCache.Proxy.Timeout", 300);
        int setDataQueueSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByApplicantPreloader.SetDataQueueSize", 100000);
        int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByApplicantPreloader.ProduceBatchSize", 10000);
        int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByApplicantPreloader.ConsumeBatchSize", 3000);
        int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByApplicantPreloader.WriteBatchSize", 1000);
        int threadSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByApplicantPreloader.ThreadSize", 5);

	int cluster = 10;

	MCE_DEBUG("ctlEndpoint: " << ctlEndpoint << " cluster: " << cluster << " setDataQueueSize: " << setDataQueueSize);
	MCE_DEBUG("cInterval: " << cInterval << " timeout : " << timeout << " produceBatchSize: " << produceBatchSize);
	MCE_DEBUG("consumBatchSize: " << consumeBatchSize << "writeBatchSize: " << writeBatchSize << " threadSize: " << threadSize);
	GeneralPreloader<xce::buddy::BuddyByApplicantCacheManagerPrx>::instance().initialize(
                new BatchUsersIdProducer,
                &BuddyByApplicantFactory::instance(),
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
