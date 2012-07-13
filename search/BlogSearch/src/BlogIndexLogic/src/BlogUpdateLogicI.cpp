/*
 * =====================================================================================
 *
 *       Filename:  BlogUpdateLogicI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年10月12日 17时56分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */

#include "BlogUpdateLogicI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "XceCache.h"
//
using namespace com::xiaonei::xce;
using namespace xce::serverstate;

namespace MyUtil  {
  void initialize()  {
    ServiceI& service = ServiceI::instance();


    int ha_service_cluster = service.getCommunicator()->getProperties()->getPropertyAsInt("BlogUpdateLogic.ServiceCluster");

    int ha_service_mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BlogUpdateLogic.ServiceMod");

    int index_cluster = service.getCommunicator()->getProperties()->getPropertyAsInt("BlogUpdateLogic.IndexCluster");

    int batch_process = service.getCommunicator()->getProperties()->getPropertyAsInt("BlogUpdateLogic.BatchProcess");

    int time_interval = service.getCommunicator()->getProperties()->getPropertyAsInt("BlogUpdateLogic.TimeInterval");

  /* std::string file = service.getCommunicator()->getProperties()->getProperty("BlogUpdateLogic.CacheFile");  */
    
  /* std::string file_op = service.getCommunicator()->getProperties()->getProperty("BlogUpdateLogic.OperationFile"); */
    
    std::string common_dir = service.getCommunicator()->getProperties()->getProperty("BlogUpdateLogic.CommonIndexDir");

    std::string * index_path = new std::string[index_cluster];
    for(int i = 0; i < index_cluster; i++)  {
      char num[5];
      itoa(i, num);
      index_path[i] = common_dir + (string)(num); 
      MCE_INFO("indexPath "<<i<<" : "<<index_path[i]);
    }
    
    std::string del_file = service.getCommunicator()->getProperties()->getProperty("BlogUpdateLogic.DelFile");
    std::string update_file = service.getCommunicator()->getProperties()->getProperty("BlogUpdateLogic.UpdateFile"); 



    /*    std::string zookeeper_addr = service.getCommunicator()->getProperties()->getProperty("BlogUpdateLogic.ZookeeperAddr");

    std::string container_path = service.getCommunicator()->getProperties()->getProperty("BlogUpdateLogic.ContainerPath");
  */  
    com::xiaonei::search5::logicserver::kMaxNumEachProcess = batch_process;

    com::xiaonei::search5::logicserver::kInterval = (double) time_interval;

    service.getAdapter()->add(&com::xiaonei::search5::logicserver::BlogUpdateLogicI::instance(),service.createIdentity(com::xiaonei::search5::logicserver::BLOG_INDEX_LOGIC_MANAGER, ""));

    ServerStateSubscriber::instance().initialize("ControllerBlogIndexLogic", &com::xiaonei::search5::logicserver::BlogUpdateLogicI::instance(), ha_service_mod);

    com::xiaonei::search5::logicserver::BlogUpdateLogicI::instance().setValid(true);

    TaskManager::instance().config(com::xiaonei::search5::logicserver::TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
    TaskManager::instance().config(com::xiaonei::search5::logicserver::TASK_LEVEL_FILL_BASIC, ThreadPoolConfig(1, 64));
    TaskManager::instance().config(com::xiaonei::search5::logicserver::TASK_LEVEL_FILL_CONFIG, ThreadPoolConfig(1, 8));
    TaskManager::instance().config(com::xiaonei::search5::logicserver::TASK_LEVEL_UPDATE, ThreadPoolConfig(32, 64));
   
    time_t now;
    struct tm *p;
    time(&now);
    p = gmtime(&now);
    char mon_char[5], day_char[5];
    itoa(p->tm_mon+1, mon_char);
    itoa(p->tm_mday, day_char);
    dataNow = (string)mon_char;
    if(p->tm_mon>=0 && p->tm_mon<9)  {
      dataNow = "0" + dataNow;
    }
    if(p->tm_mday>0 && p->tm_mday<=9)  {
      dataNow += "0";
      dataNow += (string)day_char; 
    }  else  {
      dataNow += (string)day_char;
    }
    dataPre = dataNow; 

    com::xiaonei::search5::logicserver::BlogUpdateLogicI::instance().bootstrap(ha_service_cluster, ha_service_mod, index_cluster, /*file, file_op,*/ index_path, del_file, update_file /*  , zookeeper_addr, container_path*/);


    service.registerXceObserver(new com::xiaonei::search5::logicserver::ServiceSchemaI);  //注册与没注册的区别
  }
}


namespace com  {
  namespace xiaonei  {
    namespace search5  {
      namespace logicserver  {

        using namespace MyUtil;
        using namespace IceUtil;
        using namespace com::xiaonei::xce;

        void ServiceSchemaI::configure(const Ice::PropertiesPtr& props)  {
          TaskManager::instance().config(props, "Fill", TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
        }

        AllBlog2File::AllBlog2File(int size)  {
          update_data_.reserve(size);
        }


        void AllBlog2File::setReadMutex()  {
          IceUtil::RWRecMutex::RLock lock(update_data_rw_lock_);
        }

        void AllBlog2File::setWriteMutex()  {
          IceUtil::RWRecMutex::WLock lock(update_data_rw_lock_);
        }

        void AllBlog2File::setUnlock()  {
          update_data_rw_lock_.unlock();
        }
        void AllBlog2File::config(std::string & del_file, std::string & update_file, std::string * index_path/*std::string & file, std::string & file_op*/)  {
          //file_ = file;
          //file_op_ = file_op;
          del_file_ = del_file;
          update_file_ = update_file;
          index_path_ = index_path;
          
        }

        bool AllBlog2File::parseStr(const std::string& record, IndexUpdateData & info)  {
          if(record != "")  {
            int len, index_start, index_end, count = 0;
            std::string value;
            index_start = 0;
            len = record.length();
            while(index_start < len)  {
              index_end = record.find("\001", index_start);
              if ( count < 22)  { 
                if(count == 22 && index_end == record.npos)  {
                  index_end = len;
                }  else {
                  return false;
                }
              }  else  {
                break;
              }
              if( index_start>=0 && index_end <= len && index_end > index_start)  {
                value = record.substr(index_start, index_end - index_start);
              }  else  {
                return false;
              }
              switch(count)  {
                case 0:  { info.index["id"] = value; break;}
                case 1:  { info.index["owner"] = value; break;}
                case 2:  { info.index["name"] = value; break;}
                case 3:  { info.index["headurl"] = value; break;}
                case 4:  { info.index["tinyurl"] = value; break;}
                case 5:  { info.index["body"] = value; break;}
                case 6:  { info.index["commentCount"] = value; break;}
                case 7:  { info.index["title"] = value; break;}
                case 8:  { info.index["time"] = value; break;}
                case 9:  { info.index["uptime"] = value; break;}
                case 10:  { info.index["viewCount"] = value; break;}
                case 11:  { info.index["month"] = value; break;}
                case 12:  { info.index["blogControl"] = value; break;}
                case 13:  { info.index["cmdCount"] = value; break;}
                case 14: { info.index["univ"] = value;  break;}
                case 15: { info.index["blogPicId"] = value; break;}
                case 16: { info.index["draftFlag"] = value; break;}
                case 17: { info.index["stage"] = value; break;}
                case 18: { info.index["type"] = value; break;}
                case 19: { info.index["originalUrl"] = value; break;}
                case 20: { info.index["categoryId"] = value; break;}
                case 21: { info.operationType = (short)atoi(value.c_str()); break;}
              }
              count++;
            }
          }  else {
            return false;
          }
        }

        bool AllBlog2File::readFile(const std::string & file)  {
          MCE_INFO("The filename is "<<file);
          ifstream input(file.c_str());//~P~L~W�读~F~Y~V~G件~F�~A~_
          int line = 0;
          string record;
          IndexUpdateDataSeq info_seq;
          setWriteMutex();//~T~A~X�~P�~A~G~J~T�?
          while(!input.eof())  {
            if(info_seq.size() >= kMaxNumEachProcess) {
              info_seq.clear();
            }
            getline(input,record, '\n');
            if(record != "")  {
              IndexUpdateData info_tmp;
              if(parseStr(record, info_tmp))  {
                info_seq.push_back(info_tmp);
              }
            }
            line++;
          }
          if(info_seq.size() > 0)  {
            info_seq.clear();
          }
          setUnlock();
          return true;
        }

        bool AllBlog2File::writeFile(IndexUpdateDataSeq& data, int mod, bool way)  {
          setWriteMutex();
          if(way)  {
            ofstream output;
            string file = index_path_[mod] + "/data." + dataNow;
            MCE_INFO("writeFile file name : "<<file);
            output.open(file.c_str(), ios::app);
            for(IndexUpdateDataSeq::iterator iter = data.begin(); iter != data.end(); iter++)  {
              if(iter->index["id"] == ""
                  || iter->index["owner"] == "")  {
                break;
              }
              output<<iter->index["id"]<<"\001"<<iter->index["owner"]<<"\001"<<iter->index["name"]<<"\001"
                <<iter->index["headurl"]<<"\001"<<iter->index["tinyurl"]<<"\001"<<iter->index["body"]<<"\001"
                <<iter->index["commentCount"]<<"\001"<<iter->index["title"]<<"\001"<<iter->index["time"]<<"\001"
                <<iter->index["uptime"]<<"\001"<<iter->index["viewCount"]<<"\001"<<iter->index["month"]<<"\001"
                <<iter->index["blogControl"]<<iter->index["cmdCount"]<<"\001"<<iter->index["univ"]<<"\001"
                <<iter->index["blogPicId"]<<"\001"<<iter->index["draftFlag"]<<"\001"
                <<iter->index["stage"]<<"\001"<<iter->index["type"]<<"\001"
                <<iter->index["originalUrl"]<<"\001"<<iter->index["categoryId"]<<"\001"<<iter->operationType<<endl;
            }
          }  else  {
            string file = index_path_[mod] + "/data." + dataNow;
            ofstream output(file.c_str());
            for(IndexUpdateDataSeq::iterator iter = data.begin(); iter != data.end(); iter++)  {
              if(iter->index["id"] == ""
                  || iter->index["owner"] == "")  {
                break;
              }
              output<<iter->index["id"]<<"\001"<<iter->index["owner"]<<"\001"<<iter->index["name"]<<"\001"
                <<iter->index["headurl"]<<"\001"<<iter->index["tinyurl"]<<"\001"<<iter->index["body"]<<"\001"
                <<iter->index["commentCount"]<<"\001"<<iter->index["title"]<<"\001"<<iter->index["time"]<<"\001"
                <<iter->index["uptime"]<<"\001"<<iter->index["viewCount"]<<"\001"<<iter->index["month"]<<"\001"
                <<iter->index["blogControl"]<<iter->index["cmdCount"]<<iter->index["univ"]<<"\001"
                <<iter->index["blogPicId"]<<"\001"<<iter->index["draftFlag"]<<"\001"
                <<iter->index["stage"]<<"\001"<<iter->index["type"]<<"\001"
                <<iter->index["originalUrl"]<<"\001"<<iter->index["categoryId"]<<"\001"<<iter->operationType<<endl;

            }
          }
          setUnlock();
          return true;
        }
        
       bool AllBlog2File::writeDelFile(vector<string>& data, int mod)  { 
         setWriteMutex(); 
         string file = index_path_[mod] + "/" + del_file_;
         ofstream output(file.c_str());
         for(vector<string>::iterator iter = data.begin(); iter != data.end(); iter++)  {
             output<<*iter<<endl;
         }
         setUnlock();
         return true;
       }
       
       bool AllBlog2File::writeUpdateFile(IndexUpdateDataSeq& data, int mod)  { 
         setWriteMutex(); 
         string file = index_path_[mod] + "/" + update_file_;
         ofstream output(file.c_str());
         for(IndexUpdateDataSeq::iterator iter = data.begin(); iter != data.end(); iter++)  {
           if(iter->index["id"] == ""
               || iter->index["owner"] == "")  {
             break;
           }
           output<<iter->index["id"]<<"\001"<<iter->index["owner"]<<"\001"<<iter->index["name"]<<"\001"
             <<iter->index["headurl"]<<"\001"<<iter->index["tinyurl"]<<"\001"<<iter->index["body"]<<"\001"
             <<iter->index["commentCount"]<<"\001"<<iter->index["title"]<<"\001"<<iter->index["time"]<<"\001"
             <<iter->index["uptime"]<<"\001"<<iter->index["viewCount"]<<"\001"<<iter->index["month"]<<"\001"
             <<iter->index["blogControl"]<<iter->index["cmdCount"]<<iter->index["univ"]<<"\001"
             <<iter->index["blogPicId"]<<"\001"<<iter->index["draftFlag"]<<"\001"
             <<iter->index["stage"]<<"\001"<<iter->index["type"]<<"\001"
             <<iter->index["originalUrl"]<<"\001"<<iter->index["categoryId"]<<"\001"<<iter->operationType<<endl;
         }
         setUnlock();
         return true;
       }

        /* bool AllBlog2File::writeOperation(IndexUpdateDataSeq& data, bool way)  { 
          time_t current_time;
          struct tm *p;
          time(&current_time);
          p=gmtime(&current_time);
          char year[10], mon[10], day[10];
          string mon_str, day_str;
          if(p->tm_hour == 23)  {
            way = false; 
          }
          setWriteMutex();
          if(way)  {
            ofstream output;
            output.open(file_op_.c_str(), ios::app);
            for(IndexUpdateDataSeq::iterator iter = data.begin(); iter != data.end(); iter++)  {
              if(iter->index["id"] == ""
                  || iter->index["owner"] == "")  {
                break;
              }
              if(iter->operationType != 0)  { 
                output<<iter->index["id"]<<" "<<iter->index["owner"]<<" "<<iter->operationType<<endl;
              }
            }
          }  else  {
            itoa(1900+p->tm_year, year);
            itoa(1+p->tm_mon, mon);
            if(((string)mon).length() == 1)  {
              mon_str = "0" + (string)mon;
            }
            itoa(p->tm_mday, day);
            day_str = (string)day;
            if(day_str.length() == 1)  {
              day_str = "0" + day_str;
            }
            file_ = file_ + "." + (string)year + mon_str + day_str;
            ofstream output(file_.c_str());
            for(IndexUpdateDataSeq::iterator iter = data.begin(); iter != data.end(); iter++)  {
              if(iter->index["id"] == ""
                  || iter->index["owner"] == "")  {
                break;
              }
              if(iter->operationType != 0)  {
                output<<iter->index["id"]<<" "<<iter->index["owner"]<<" "<<iter->operationType<<endl;
              }
            }
          }
          setUnlock();
          return true;
        }*/

        bool AllBlog2File::writeBuffer(IndexUpdateDataSeq infos)  {
          setWriteMutex();
          int original_size = update_data_.size();
          MCE_WARN("before original_size : "<<original_size);
          int infos_size = infos.size();
          MCE_WARN("before infos_size : "<<infos_size);
          update_data_.insert(update_data_.end(), infos.begin(), infos.end());
          if(update_data_.size() != original_size + infos_size)  {
            MCE_WARN("update_data_ size : "<<update_data_.size());
            MCE_WARN("original_size : "<<original_size);
            MCE_WARN("infos_size : "<<infos_size);
            MCE_WARN("writBuffer of AllBlog2File wrong!");
            setUnlock();
            return false;
          }  else  {
            setUnlock(); 
            return true;
          }
        }

        int AllBlog2File::getBlogSize() {
          setReadMutex();
          int size = update_data_.size();
          //setUnlock();
          return size;
        }

        void AllBlog2File::clearBlog()  {
          setWriteMutex();
          update_data_.clear();
          //setUnlock();
        }
  
        BatchBlogResultHandlerI::BatchBlogResultHandlerI(IndexUpdateData& data) :_data(data) {

        }

        bool BatchBlogResultHandlerI::handle(mysqlpp::Row& row) const  {
          _data.index["id"] = (std::string) row["id"];
          _data.index["owner"] = (std::string) row["owner"];
          _data.index["name"] = (std::string) row["name"];
          _data.index["headurl"] = (std::string) row["headurl"];
          _data.index["tinyurl"] = (std::string) row["tinyurl"];
          _data.index["body"] = (std::string) row["body"];
          _data.index["commentCount"] = (std::string) row["comment_count"];
          _data.index["title"] = (std::string) row["title"];
          _data.index["time"] = (std::string) row["time"];
          _data.index["uptime"] = (std::string) row["uptime"];
          _data.index["viewCount"] = (std::string) row["view_count"];
          _data.index["month"] = (std::string) row["month"];
          _data.index["blogControl"] = (std::string) row["blog_control"];
          _data.index["cmdCount"] = (std::string) row["cmd_count"];
          _data.index["univ"] = (std::string) row["univ"];
          _data.index["blogPicId"] = (std::string) row["blog_pic_id"];
          _data.index["draftFlag"] = (std::string) row["draft_flag"];
          _data.index["stage"] = (std::string) row["stage"];
          _data.index["type"] = (std::string) row["type"];
          _data.index["originalUrl"] = (std::string) row["original_url"];
          _data.index["categoryId"] = (std::string) row["category_id"];
          
          return true;
        }


        MonitorPrintLogTask::MonitorPrintLogTask()  {

        }
      
        void MonitorPrintLogTask::handle()  {
          while(true)  {
            time_t now;
            struct tm* p;
            time(&now);
            p = gmtime(&now);
            dataPre = dataNow; 
            if(p->tm_hour == 0)  {
              char mon_char[5], day_char[5], hour_char[5], minute_char[5];
              itoa(p->tm_mon+1, mon_char);
              itoa(p->tm_mday, day_char);
              itoa(p->tm_hour, hour_char);
              itoa(p->tm_min, minute_char);
              dataNow = (string)mon_char;
              if(p->tm_mon>=0 && p->tm_mon<9)  {
                dataNow = "0" + dataNow;
              }
              dataNow += (string)day_char;
              if(p->tm_mday>0 && p->tm_mday<=9)  {
                dataNow = "0" + dataNow;
              }
              //上线时去掉
              dataNow += (string)hour_char;
              dataNow += (string)minute_char;
              sleep(60*60);              
            }
            sleep(3);
            //sleep(30);  
          }
        }





        WriteUpdateDataTask::WriteUpdateDataTask(IndexUpdateDataSeq infos, int mod) : _infos(infos), _mod(mod) {
        }

        void WriteUpdateDataTask::handle()  {
          
          AllBlog2File & blog2file = BlogUpdateLogicI::instance().getBlog2File();
          
            bool way = true;
            
            MCE_INFO("Write data to file!");

            blog2file.writeFile(_infos, _mod, way);
            
           // blog2file.writeOperation(_infos,  way);
          
            MCE_INFO("Write data end!");

        }

        WriteDelUpdateFileTask::WriteDelUpdateFileTask(vector<string> info_del, IndexUpdateDataSeq info_update, int mod) : _info_del(info_del), _info_update(info_update), _mod(mod) {
        }

        void WriteDelUpdateFileTask::handle()  {

          AllBlog2File & blog2file = BlogUpdateLogicI::instance().getBlog2File();

          MCE_INFO("start writing delete record to file!");

          blog2file.writeDelFile(_info_del, _mod);

          MCE_INFO("End writing delete record to file!");
          
          blog2file.writeUpdateFile(_info_update, _mod);
         
          MCE_INFO("End writing update record to file!");

        }







        

        BlogUpdateLogicI::BlogUpdateLogicI(): _allBlogFile(kMaxNumEachProcess)  {

        }
        void BlogUpdateLogicI::setValid(bool newState, const Ice::Current& ic)  {
          MCE_INFO("BlogUpdateLogicI::setValid-> "<<newState);
          IceUtil::RWRecMutex::WLock lock(_validMutex);
          _valid = newState;
          if(_valid)  {
            ServerStateSubscriber::instance().setStatus(1);
          }  else  {
            ServerStateSubscriber::instance().setStatus(0);
          }
        }

        bool BlogUpdateLogicI::isValid(const Ice::Current& ic)  {
          RWRecMutex::RLock lock(_validMutex);
          return _valid;
        }

        void BlogUpdateLogicI::bootstrap(int ha_service_cluster, int ha_service_mod, int index_cluster, /*std::string file, std::string file_op,*/ std::string * index_path, std::string del_file, std::string update_file /*  , std::string zookeeper_addr, std::string container_path*/)  {

          _ha_service_cluster = ha_service_cluster;

          _ha_service_mod = ha_service_mod;

          _index_cluster = index_cluster;

          //_zookeeper_addr = zookeeper_addr;

          //_container_path = container_path;

          _allBlogFile.config(del_file, update_file,/*file, file_op,*/ index_path);

          for(int i = 0; i < _index_cluster; i++)  {

            MessageQueueThread * thread = new MessageQueueThread(i);

            thread->start();

            _threads.push_back(thread);

          }


          TaskManager::instance().execute(new MonitorPrintLogTask());
          
        //  BlogSubIndexAdapter::instance().setZookeeper(_zookeeper_addr);
        //  BlogSubIndexAdapter::instance().setContainerPath(_container_path);
        //  BlogSubIndexAdapter::instance().setManagerTwoway();

          MCE_INFO("FINISH BOOT");
        }

        void BlogUpdateLogicI::setRecovery(bool newState, const Ice::Current& ic)  {

        }

        bool BlogUpdateLogicI::isRecovery(const Ice::Current& ic)  {

        }
        void BlogUpdateLogicI::update(const IndexUpdateData&  info, const Ice::Current& ic)  {// 异步调用单个接口

          MCE_INFO("HAVA INVOKING...");

          IndexUpdateData final;

          final = info;

          IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_setMutex);

          int owner_id = atoi(((string)final.index["owner"]).c_str());

          _threads.at((owner_id % 100) % _index_cluster)->invoke(final); 

        }

        AllBlog2File & BlogUpdateLogicI::getBlog2File()  {
          return _allBlogFile;
        }


        WriteIndexTask::WriteIndexTask(int mod, IndexUpdateDataSeq infos) :  _mod(mod), _infos(infos)  {

        }

        void WriteIndexTask::handle()  {
          MCE_INFO("Enter into updating thread and info size "<<_infos.size());

          BlogSubIndexAdapter::instance().update(_mod, _infos);
        }

      }
    }
  }
}
