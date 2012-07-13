/*
 * =====================================================================================
 *
 *       Filename:  DataCenter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年04月07日 10时10分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */

#include "../include/DataCenter.h"
using namespace com::xiaonei::xce;
using namespace xce::serverstate;


int itoa(int i, char* string)  {
  int power,j; j=i;
  int count = 0;
  for(power=1;j>=10;j/=10) power*=10;
  for(;power>0;power/=10) {
    *string++ ='0' + i/power;
    i%=power;
    count ++;
  }
  *string='\0';
  return count;
}

void initConf(ServiceI& service, const string& service_name, string& business_type, string& service_controller_name, string& flush_controller_name, int & service_cluster, int & batch_process, int & time_interval)  {
  business_type = service.getCommunicator()->getProperties()->getProperty(service_name + ".BusinessType");
  service_controller_name = service.getCommunicator()->getProperties()->getProperty(service_name + ".ServiceControllerName");
  flush_controller_name = service.getCommunicator()->getProperties()->getProperty(service_name + ".FlushControllerName");
  service_cluster = service.getCommunicator()->getProperties()->getPropertyAsInt(service_name + ".ServiceCluster");
  batch_process = service.getCommunicator()->getProperties()->getPropertyAsInt(service_name + ".BatchProcess");
  time_interval = service.getCommunicator()->getProperties()->getPropertyAsInt(service_name + ".TimeInterval");
  PRIFIELDKEY = service.getCommunicator()->getProperties()->getProperty(service_name + ".PrimaryFieldKey");
  USERFIELDKEY = service.getCommunicator()->getProperties()->getProperty(service_name + ".UserFieldKey");

}


namespace MyUtil  {
  void initialize()  {
    
    ServiceI& service = ServiceI::instance();
    
    SERVICENAME = service.getName();
    
    SERVICENAME = SERVICENAME.substr(0, SERVICENAME.size() -2);
    
    int service_cluster, batch_process, time_interval;
   
    string business_type = "", service_controller_name = "", flush_controller_name = ""; 
    
    initConf(service, SERVICENAME, business_type, service_controller_name, flush_controller_name, service_cluster, batch_process, time_interval);
    
    MCE_INFO("PrimaryFieldKey is "<<PRIFIELDKEY); 
    
    MCE_INFO("UserFieldKey is "<<USERFIELDKEY); 
    
    com::xiaonei::search5::logicserver::kMaxNumEachProcess = batch_process;

    com::xiaonei::search5::logicserver::kInterval = (double) time_interval;

    ServerStateSubscriber::instance().initialize(service_controller_name, &com::xiaonei::search5::logicserver::IndexUpdateLogicI::instance(),  0);

    MCE_INFO("ServiceControllerName is "<<service_controller_name); 
    
    com::xiaonei::search5::logicserver::IndexUpdateLogicI::instance().setValid(true);

    FlushDataAdapterFactory::loadFlushSearchDataAdapter();
    
    SubIndexAdapterFactory::loadSubIndexAdapter();  
    MCE_INFO("BusinessType is "<<business_type);
    com::xiaonei::search5::logicserver::IndexUpdateLogicI::instance().bootstrap(business_type, flush_controller_name, service_cluster);
  }
}

namespace com  {
  namespace xiaonei  {
    namespace search5  {
      namespace logicserver  {
        using namespace MyUtil;
        using namespace IceUtil;
        using namespace com::xiaonei::xce;
      
        void MessageQueueThread::invoke(const IndexUpdateData& unit)  {
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
          _cache.push_back(unit);
          if ((int)_cache.size() > kMaxNumEachProcess) {
            _mutex.notify();
          }
        }

        void MessageQueueThread::run() {
          while(true) {
            IndexUpdateDataSeq batch;
            {
              IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
              if ((int)_cache.size() < kMaxNumEachProcess) {
                _mutex.timedWait(IceUtil::Time::seconds((IceUtil::Int64)kInterval));
              }
              batch.swap(_cache);
            }
            if(!batch.empty())  {
              try  {
                if(SubIndexAdapterFactory::getNamed(_business_type))  {
                  SubIndexAdapterFactory::getNamed(_business_type)->update(_service_mod, batch);
                }
              }  catch(Ice::Exception & e)  {
                MCE_WARN("DataCenter::MessageQueue run Ice::Exception "<<e);
              }  catch(std::exception & e)  {
                MCE_WARN("DataCenter::MessageQueue run std::exception "<<e.what());
              }  catch(...)  {
                MCE_WARN("DataCenter::MessageQueue run unknow exception!");
              }
            }
          }
        }

        IndexUpdateLogicI::IndexUpdateLogicI()  {
        }
        
        void IndexUpdateLogicI::setValid(bool new_state, const Ice::Current& ic)  {
          MCE_INFO("IndexUpdateLogicI::setValid-> "<<new_state);
          IceUtil::RWRecMutex::WLock lock(_valid_mutex);
          _valid = new_state;
          if(_valid)  {
            ServerStateSubscriber::instance().setStatus(1);
          }  else  {
            ServerStateSubscriber::instance().setStatus(0);
          }
        }

        bool IndexUpdateLogicI::isValid(const Ice::Current& ic)  {
          RWRecMutex::RLock lock(_valid_mutex);
          return _valid;
        }

        void IndexUpdateLogicI::bootstrap(const string& business_type, const string& controller_name, int service_cluster)  {
          _business_type = business_type; 
          _service_cluster = service_cluster;
          _controller_name = controller_name;
          for(int i = 0; i < _service_cluster; i++)  {
            MessageQueueThread * thread = new MessageQueueThread(business_type, i);
            thread->start();
            _threads.push_back(thread);
          }
          MCE_INFO("FINISH BOOT");
        }

        void IndexUpdateLogicI::setRecovery(bool new_state, const Ice::Current& ic)  {
        }

        bool IndexUpdateLogicI::isRecovery(const Ice::Current& ic)  {
          return true;
        }
        
        void IndexUpdateLogicI::update(const IndexUpdateData&  info, const Ice::Current& ic)  {// 异步调用单个接口
          try  {
            FlushDataAdapterFactory::getNamed(_controller_name).get()->update(info);
          } catch(Ice::Exception & e)  {
            MCE_WARN("DataCenter::MessageQueue run Ice::Exception "<<e);
          }  catch(std::exception & e)  {
            MCE_WARN("DataCenter::MessageQueue run std::exception "<<e.what());
          }  catch(...)  {
            MCE_WARN("DataCenter::MessageQueue run unknow exception!");
          }
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_set_mutex);
          if((string)info.index.find(USERFIELDKEY)->second != "")  {  
            if((info.index.find(PRIFIELDKEY)->second.c_str())[0] > '9' ||
               (info.index.find(PRIFIELDKEY)->second.c_str())[0] < '0')  {
              _threads.at(0)->invoke(info);
            }  else  {
               int owner_id = atoi(info.index.find(USERFIELDKEY)->second.c_str());
               int index_mod = owner_id % _service_cluster;
               _threads.at(index_mod)->invoke(info);
            }
          }
        }


      }
    }
  }
}
