/*
 * =====================================================================================
 *
 *       Filename:  DataCenter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年04月07日 10时10分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef __DATACENTER_H__
#define __DATACENTER_H__

#include <boost/shared_ptr.hpp>
#include "ContentSearch.h"
#include "ServiceI.h"
#include "IceUtil/IceUtil.h"
#include "QueryRunner.h"
#include "OceCxxAdapter/src/FlushSearchDataAdapter.h"
#include "OceCxxAdapter/src/SubIndexAdapter.h"
#include <stdio.h>
#include <sstream>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "XceCache.h"
#include "util/cpp/TimeCost.h"

using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace IceUtil; 
using namespace std;
  
int itoa(int i, char* string);


void initConf(ServiceI& service, const string& service_name, string& business_type, string& service_controller_name, string& flush_controller_name, int & service_cluster, int & batch_process, int & time_interval);



static string PRIFIELDKEY = "";
static string USERFIELDKEY = "";
static string SERVICENAME = "";

namespace com  {
  namespace xiaonei {
    namespace search5  {
      namespace logicserver  {

        const int TASK_LEVEL_FILL = 1; 
        const int TASK_LEVEL_FILL_BASIC = 2;
        const int TASK_LEVEL_FILL_CONFIG = 3;
        const int TASK_LEVEL_UPDATE = 8;
        static int kMaxNumEachProcess;
        static double kInterval;
                
              
        class MessageQueueThread : virtual public IceUtil::Thread {

          public:
            
            MessageQueueThread(const string& business_type, int service_mod) {
              _service_mod = service_mod;
              char a[5];
              itoa(_service_mod, a);
              _service_string = (string)a;
              _business_type = business_type; 
            }
           
            void invoke(const IndexUpdateData& unit);
            void run();
            
        private:
        int _service_mod;
        string _business_type; 
        string _service_string;
        IceUtil::Monitor<IceUtil::Mutex> _mutex;
        IndexUpdateDataSeq _cache;
      };

        class IndexUpdateLogicI : virtual public IndexUpdateLogic,public MyUtil::Singleton<IndexUpdateLogicI>  {

          public:
            IndexUpdateLogicI();
            virtual void update(const IndexUpdateData&  info, const Ice::Current& ic = Ice::Current());            
            virtual void setValid(bool newState, const Ice::Current& ic= Ice::Current());
            virtual bool isValid(const Ice::Current& ic = Ice::Current());
            virtual void setRecovery(bool newState, const Ice::Current& ic = Ice::Current());//  ice file not add
            virtual bool isRecovery(const Ice::Current& ic = Ice::Current());
            void bootstrap(const string& business_type, const string& controller_name, int service_cluster);
          private:
            string _business_type;
            string _controller_name;
            bool _valid;
            IceUtil::RWRecMutex _valid_mutex;
            IceUtil::Monitor<IceUtil::Mutex> _set_mutex;
            int _service_cluster;
            vector<MessageQueueThread *> _threads;
        };


      }
    }
  }     
}
#endif
