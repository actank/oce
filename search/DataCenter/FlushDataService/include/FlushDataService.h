/*
 * =====================================================================================
 *
 *       Filename:  FlushDataService.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年05月17日 14时43分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef __FLUSHDATASERVICE_H__
#define __FLUSHDATASERVICE_H__

#include <boost/shared_ptr.hpp>
#include "ContentSearch.h"
#include "ServiceI.h"
#include "IceUtil/IceUtil.h"
#include "QueryRunner.h"
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <sstream>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "XceCache.h"
#include "util/cpp/TimeCost.h"


using namespace MyUtil;
using namespace IceUtil; 
using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace std;
  
void initConf(ServiceI& service, const string& service_name, string& controller_name, string& common_index_dir, int& batch_process, int& time_interval, string& source_data_log, string & fields_conf_file, string& adapter_name);


void setLogger(const string & common_index_dir, const string & log_name, vector<Logger> & all_data_loggers, DailyRollingFileSchedule schedule);

static vector<Logger> kAllDataLoggers;

static string PRIFIELDKEY = "";
static string USERFIELDKEY = "";
static string SERVICENAME = "";

namespace com  {
  namespace xiaonei {
    namespace search5  {
      namespace logicserver  {

        static int kMaxNumEachProcess;
        static double kInterval;

        class MessageQueueThread : virtual public IceUtil::Thread {
          public:
            MessageQueueThread() {
            }
            void invoke(const IndexUpdateData& unit);
            void run();

          private:
            IceUtil::Monitor<IceUtil::Mutex> _mutex;
            IndexUpdateDataSeq _cache;
        };


        class ProcessFileServiceI: virtual public IndexUpdateLogic,public MyUtil::Singleton<ProcessFileServiceI>    {

          public:
            ProcessFileServiceI();
            void config(const string & fields_conf_file);
            bool writeFile(const IndexUpdateDataSeq& data);
            virtual void update(const IndexUpdateData&  data, const Ice::Current& ic = Ice::Current());            
            virtual void setValid(bool new_state, const Ice::Current& ic= Ice::Current());
            virtual bool isValid(const Ice::Current& ic = Ice::Current());
            virtual void setRecovery(bool new_state, const Ice::Current& ic= Ice::Current());
            virtual bool isRecovery(const Ice::Current& ic = Ice::Current());
            void bootstrap(const string& fields_conf_file);
            friend class MessageQueueThread;
          private:
            bool _valid;
            MessageQueueThread * _thread;
            IceUtil::Monitor<IceUtil::Mutex> _set_mutex;
            IceUtil::RWRecMutex _valid_mutex;
            vector<string> fields_;
        };

      }
    }
  }     
}
#endif
