/*
 * =====================================================================================
 *
 *       Filename:  FlushDataService.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年05月17日 14时43分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */

#include "../include/FlushDataService.h"
using namespace com::xiaonei::xce;
using namespace xce::serverstate;


void initConf(ServiceI& service, const string& service_name, string& controller_name, string& common_index_dir, int& batch_process, int& time_interval, string& source_data_log, string & fields_conf_file)  {
  controller_name = service.getCommunicator()->getProperties()->getProperty(service_name + ".ControllerName");
  common_index_dir = service.getCommunicator()->getProperties()->getProperty(service_name + ".CommonIndexDir");
  batch_process = service.getCommunicator()->getProperties()->getPropertyAsInt(service_name + ".BatchProcess");
  source_data_log = service.getCommunicator()->getProperties()->getProperty(service_name + ".SourceDataLog");
  time_interval = service.getCommunicator()->getProperties()->getPropertyAsInt(service_name + ".TimeInterval");
  fields_conf_file = service.getCommunicator()->getProperties()->getProperty(service_name + ".FieldsConfFile");
  PRIFIELDKEY = service.getCommunicator()->getProperties()->getProperty(service_name + ".PrimaryFieldKey");
  USERFIELDKEY = service.getCommunicator()->getProperties()->getProperty(service_name + ".UserFieldKey"); 
}

void setLogger(const string & common_index_dir, const string & log_name, vector<Logger> & all_data_loggers, DailyRollingFileSchedule schedule)  {
    string file = common_index_dir  + "/" + log_name;    
    log4cplus::SharedAppenderPtr append_all(new DailyRollingFileAppender(file, schedule, true, 0));
    append_all->setName(file);
    MCE_INFO("FileName is " << file);
    std::string pattern_all = "%D{%Y-%m-%d\001%H:%M:%S}\001%m%n";
    std::auto_ptr<log4cplus::Layout> layout_all(new PatternLayout(pattern_all));
    append_all->setLayout( layout_all );
    Logger logger_all = Logger::getInstance(log_name);
    logger_all.addAppender(append_all);
    logger_all.setLogLevel(ALL_LOG_LEVEL);
    all_data_loggers.push_back(logger_all);
}


namespace MyUtil  {
  void initialize()  {
    ServiceI& service = ServiceI::instance();
    SERVICENAME = service.getName();
    SERVICENAME = SERVICENAME.substr(0, SERVICENAME.size() -2);
    string controller_name, common_index_dir, source_data_log, fields_conf_file;
    int time_interval, batch_process;
    initConf(service, SERVICENAME, controller_name, common_index_dir, batch_process, time_interval, source_data_log, fields_conf_file);
    MCE_INFO("PrimaryFieldKey is "<<PRIFIELDKEY); 
    MCE_INFO("UserFieldKey is "<<USERFIELDKEY); 
    string command = "mkdir -p " + common_index_dir;
    system(command.c_str()); 
    MCE_INFO("source_data_log is "<<source_data_log);
    setLogger(common_index_dir, source_data_log, kAllDataLoggers, HOURLY);
    com::xiaonei::search5::logicserver::kMaxNumEachProcess = batch_process;
    com::xiaonei::search5::logicserver::kInterval = (double) time_interval;
    ServerStateSubscriber::instance().initialize(controller_name, &com::xiaonei::search5::logicserver::ProcessFileServiceI::instance(), 0, 300, new MyUtil::AddressbookChannel);
    com::xiaonei::search5::logicserver::ProcessFileServiceI::instance().setValid(true);
    com::xiaonei::search5::logicserver::ProcessFileServiceI::instance().bootstrap(fields_conf_file);

}
}

namespace com  {
  namespace xiaonei  {
    namespace search5  {
      namespace logicserver  {

        using namespace MyUtil;
        using namespace IceUtil;
        using namespace com::xiaonei::xce;


        ProcessFileServiceI::ProcessFileServiceI()  {
        }
            
        void ProcessFileServiceI::config(const string & fields_conf_file)  {
          ifstream  fields_stream(fields_conf_file.c_str());
          string content_per_line;
          while(!fields_stream.eof())  {
            getline(fields_stream, content_per_line, '\n'); 
            if(content_per_line.size() > 0)  { 
              fields_.push_back(content_per_line);
            } 
          }
        }



        bool ProcessFileServiceI::writeFile(const IndexUpdateDataSeq& data)  {
          string data_content;
          for(IndexUpdateDataSeq::const_iterator iter = data.begin(); iter != data.end(); iter++)  {
            if(fields_.size() <2 && ((iter->index.find(PRIFIELDKEY))->second == ""
                  || (iter->index.find(USERFIELDKEY))->second == ""))  {//resourceId, userId
              MCE_WARN("writeFile data value is null!");
              continue;
            }
            data_content = "";
            for(int i = 0; i < (int)fields_.size(); i++)  {
              if(iter->index.find(fields_.at(i)) != iter->index.end())  {
                data_content += iter->index.find(fields_.at(i))->second; 
              }
              data_content += (string)"\001";
            }
            if(iter->operationType == 0)  {
              data_content += "0";
            }  else if(iter->operationType == 1)  {
              data_content += "1";
            }  else if(iter->operationType == 2)  {
              data_content += "2";
            }
            LOG4CPLUS_INFO(kAllDataLoggers.at(0), data_content);
          }
          return true;
        } 
        
        void ProcessFileServiceI::bootstrap(const string& fields_conf_file)  {
          config(fields_conf_file);
          _thread = new MessageQueueThread();
          _thread->start();
          MCE_INFO("FINISH BOOT");
        }


        void ProcessFileServiceI::update(const IndexUpdateData&  data, const Ice::Current& ic)  {// 异步调用单个接口
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_set_mutex);
          _thread->invoke(data);
        }

        
        void ProcessFileServiceI::setValid(bool new_state, const Ice::Current& ic)  {
          MCE_INFO("IndexUpdateLogicI::setValid-> "<<new_state);
          RWRecMutex::WLock lock(_valid_mutex);
          _valid = new_state;
          if(_valid)  {
            ServerStateSubscriber::instance().setStatus(1);
          }  else  {
            ServerStateSubscriber::instance().setStatus(0);
          }
        }

        bool ProcessFileServiceI::isValid(const Ice::Current& ic)  {
          RWRecMutex::RLock lock(_valid_mutex);
          return _valid;
        }
        
        void ProcessFileServiceI::setRecovery(bool new_state, const Ice::Current& ic)  {
          return;
        }

        bool ProcessFileServiceI::isRecovery(const Ice::Current& ic)  {
          return true; 
        }    
       
        void MessageQueueThread::invoke(const IndexUpdateData& unit)  {
          {
            IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
            _cache.push_back(unit);
            if ((int)_cache.size() > kMaxNumEachProcess) {
              _mutex.notify();
            }
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
              MCE_WARN("data in _cache swapping...");
            }

            if(!batch.empty())  {
              try { 
                ProcessFileServiceI::instance().writeFile(batch);
              } catch(std::exception & e)  {
                MCE_WARN("DataCenter::MessageQueue run std::exception "<<e.what());
              }  catch(...)  {
                MCE_WARN("DataCenter::MessageQueue run unknow exception!");
              }
            }
          }
        }
        

      }
    }
  }
}
