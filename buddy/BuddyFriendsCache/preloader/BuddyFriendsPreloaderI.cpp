/*
 * =====================================================================================
 *
 *       Filename:  BuddyFriendsPreloaderI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年05月26日 17时09分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yang.yu@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */
#include "ServiceI.h"
#include "BuddyFriendsPreloaderI.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "buddy/BuddyFriendsCache/share/BuddyFriendsShare.h"

using namespace MyUtil;
using namespace xce::generalpreloader;
using namespace xce::buddyfriendspreloader;
using namespace xce::buddyfriends;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&BuddyFriendsPreloader::instance(), service.createIdentity("PL", ""));

  string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("BuddyFriendsCache.Controller", "ControllerBuddyFriendsCache");
  int cInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 60);
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyFriendsCache.Proxy.Timeout", 300);
  int setDataQueueSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyFriendsPreloader.SetDataQueueSize", 100000);
  int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyFriendsPreloader.ProduceBatchSize", 10000);
  int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyFriendsPreloader.ConsumeBatchSize", 3000);
  int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyFriendsPreloader.WriteBatchSize", 1000);
  int threadSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyFriendsPreloader.ThreadSize", 5);

  int cluster = 10;

  MCE_DEBUG("ctlEndpoint: " << ctlEndpoint << " cluster: " << cluster << " setDataQueueSize: " << setDataQueueSize);
  MCE_DEBUG("cInterval: " << cInterval << " timeout : " << timeout << " produceBatchSize: " << produceBatchSize);
  MCE_DEBUG("consumBatchSize: " << consumeBatchSize << "writeBatchSize: " << writeBatchSize << " threadSize: " << threadSize);
  GeneralPreloader<xce::buddy::BuddyFriendsCacheManagerPrx>::instance().initialize(
      new BatchUsersIdProducer,
      &BuddyFriendsFactory::instance(),
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

