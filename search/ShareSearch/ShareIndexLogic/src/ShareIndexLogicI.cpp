/*
 * =====================================================================================
 *
 *       Filename:  ShareIndexLogicI.cpp
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

#include "ShareIndexLogicI.h"
//#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "XceCache.h"
#include "ShareIndexContainerSubAAdapter.h"
#include "ShareIndexContainerSubBAdapter.h"
using namespace com::xiaonei::xce;
//using namespace xce::serverstate;

namespace MyUtil  {
  void initialize()  {
    ServiceI& service = ServiceI::instance();

    int cluster = service.getCommunicator()->getProperties()->getPropertyAsInt("ShareIndexLogic.Cluster");

    int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("ShareIndexLogic.Mod");
    
    service.getAdapter()->add(&xce::sharesearch::ShareIndexLogicManagerI::instance(),service.createIdentity(xce::sharesearch::SHARE_INDEX_LOGIC_MANAGER, ""));

    std::string recov_file = service.getCommunicator()->getProperties()->getProperty("ShareIndexLogic.RecovFile");

    std::string modify_title_file = service.getCommunicator()->getProperties()->getProperty("ShareIndexLogic.ModifyTitleFile");

    //ServerStateSubscriber::instance().initialize("ControllerShareIndexLogic", &xce::sharesearch::ShareIndexLogicManagerI::instance(), mod);

    xce::sharesearch::ShareIndexLogicManagerI::instance().setValid(true);

    TaskManager::instance().config(xce::sharesearch::TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
    TaskManager::instance().config(xce::sharesearch::TASK_LEVEL_FILL_BASIC, ThreadPoolConfig(1, 64));
    TaskManager::instance().config(xce::sharesearch::TASK_LEVEL_FILL_CONFIG, ThreadPoolConfig(1, 8));
    TaskManager::instance().config(xce::sharesearch::TASK_LEVEL_UPDATE, ThreadPoolConfig(32, 64));

    xce::sharesearch::ShareIndexLogicManagerI::instance().bootstrap(cluster, mod, recov_file, modify_title_file);

   service.registerXceObserver(new xce::sharesearch::ServiceSchemaI);  //注册与没注册的区别
  }
}


namespace xce  {
  namespace sharesearch  {
    using namespace MyUtil;
    using namespace IceUtil;
    using namespace com::xiaonei::xce;
    //const int URL_SET_SIZE = 10000000;
    AllUrl2File::AllUrl2File(int size)  {
      update_urls_.reserve(size);
    }

   
    void AllUrl2File::setReadMutex()  {
      IceUtil::RWRecMutex::RLock lock(update_urls_rw_lock_);
    }

    void AllUrl2File::setWriteMutex()  {
      IceUtil::RWRecMutex::WLock lock(update_urls_rw_lock_);
    }

    void AllUrl2File::config(std::string & file)  {
      file_ = file;
    }
    bool AllUrl2File::parseStr(const std::string& record, ShareIndexInfo & info)  { 
      if(record != "")  { 
        int len, index_start, index_end, count = 0; 
        std::string value; 
        index_start = 0;
        len = record.length(); 
        while(index_start < len)  { 
          index_end = record.find(" ", index_start);
          if ( count < 17)  { 
            if(count == 16 && index_end == record.npos)  {
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
            case 0:  { info.md5 = value; break;} 
            case 1:  { info.url = value; break;}
            case 2:  { info.type = atoi(value.c_str()); break;}
            case 3:  { info.resourceId = atol(value.c_str()); break;}
            case 4:  { info.resourceUserId = atoi(value.c_str()); break;}
            case 5:  { info.thumbUrl = value; break;}
            case 6:  { info.title = value; break;}
            case 7:  { info.summary = value; break;}
            case 8:  { info.commentCount = atoi(value.c_str()); break;}
            case 9:  { info.viewCount = atoi(value.c_str()); break;}
            case 10: { info.zanCount = atoi(value.c_str());  break;}
            case 11: { info.shareCount = atoi(value.c_str()); break;}
            case 12: { info.collectionCount = atoi(value.c_str()); break;}
            case 13: { info.status = atoi(value.c_str()); break; }
            case 14: { info.creationData = atol(value.c_str()); break;}
            case 15: { info.shareId = atol(value.c_str()); break;}
            case 16: { info.shareUserId = atoi(value.c_str()); break;} 
          } 
          count++; 
        }
      }  else {
        return false;
      }
    }

    bool AllUrl2File::readFile(const std::string & file)  {
      ifstream input(file.c_str());//同时读写文件冲突？
      int line = 0;
      string record;
      ShareIndexInfoSeq info_seq;
      while(!input.eof())  {
        if(info_seq.size() >= BATCH_URL_SIZE) {
          ShareIndexLogicManagerI::instance().update(info_seq, true);
          //ShareIndexContainerAdapter::instance().update(info_seq);
          info_seq.clear();
        }
        getline(input,record, '\n');
        if(record != "")  {
          ShareIndexInfo info_tmp;
          if(parseStr(record, info_tmp))  {
            info_seq.push_back(info_tmp);            
          }
        }
        line++; 
      }
      if(info_seq.size() > 0)  {
        ShareIndexLogicManagerI::instance().update(info_seq, true);
        //ShareIndexContainerAdapter::instance().update(info_seq);
        info_seq.clear();
      }
      setWriteMutex();//锁是否要释放?
      info_seq.swap(update_urls_);
      ShareIndexLogicManagerI::instance().update(info_seq, true);
      //ShareIndexContainerAdapter::instance().update(info_seq);
      info_seq.clear();
      return true;
    }
  
   bool AllUrl2File::writeFile(ShareIndexInfoSeq& urls, bool way)  {
     setReadMutex();
     if(!way)  {
     ofstream output;
     output.open(file_.c_str(), ios::app);
     for(ShareIndexInfoSeq::iterator iter = urls.begin(); iter != urls.end(); iter++)  {
       if(iter->md5 =="" )  {
         break;
       }
       output<<iter->md5<<" "<<iter->url<<" "<<iter->type<<" "<<iter->resourceId<<" "
         <<iter->resourceUserId<<" "<<iter->thumbUrl<<" "<<iter->title<<" "
         <<iter->summary<<" "<<iter->commentCount<<" "<<iter->viewCount<<" "
         <<iter->zanCount<<" "<<iter->shareCount<<" "<<iter->collectionCount<<" "
         <<iter->status<<" "<<iter->creationData<<" "
         <<iter->shareId<<" "<<iter->shareUserId<<endl;
     }
     }  else  {
          ofstream output(file_.c_str());
          for(ShareIndexInfoSeq::iterator iter = urls.begin(); iter != urls.end(); iter++)  {
            if(iter->md5 == "")  {
              break;
            }
            output<<iter->md5<<" "<<iter->url<<" "<<iter->type<<" "<<iter->resourceId<<" "
              <<iter->resourceUserId<<" "<<iter->thumbUrl<<" "<<iter->title<<" "
              <<iter->summary<<" "<<iter->commentCount<<" "<<iter->viewCount<<" "
              <<iter->zanCount<<" "<<iter->shareCount<<" "<<iter->collectionCount<<" "
              <<iter->status<<" "<<iter->creationData<<" "
              <<iter->shareId<<" "<<iter->shareUserId<<endl;
          }
     }
     return true;
   }

   bool AllUrl2File::writeBuffer(ShareIndexInfoSeq & infos)  {
     setWriteMutex();
     int original_size = update_urls_.size();
     int infos_size = infos.size();
     update_urls_.insert(update_urls_.end(), infos.begin(), infos.end());
     if(update_urls_.size() != original_size + infos_size)  {
       MCE_INFO("writBuffer of AllUrl2File wrong!");
       return false;
     }  else  {
          return true;
     }
   }

   int AllUrl2File::getUrlSize() {
     setReadMutex();
     return update_urls_.size();
   }

   void AllUrl2File::clearUrl()  {
     setWriteMutex();
     update_urls_.clear();
   }
   

    MonitorRecovDataTask::MonitorRecovDataTask(string file, bool & recov_symbol, bool& rebuild_symbol):MyUtil::Task(TASK_LEVEL_FILL), _file(file), 
                                                _recov_symbol(recov_symbol), _rebuild_symbol(rebuild_symbol)  {

    }

    void MonitorRecovDataTask::handle()  {
      while(true)  {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock0(_recovMutex);
        if(_recov_symbol)  {
          ShareIndexLogicManagerI::instance().getUrl2File().readFile(_file);
        }
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock1(_rebuildMutex);
        if(_rebuild_symbol)  {
          ShareIndexLogicManagerI::instance().getUrl2File().clearUrl();
        }
        sleep(60);
      }
    }

    void ServiceSchemaI::configure(const Ice::PropertiesPtr& props)  {
      TaskManager::instance().config(props, "Fill", TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
    }

    ShareIndexLogicManagerI::ShareIndexLogicManagerI() : _allUrlFile(BATCH_UPDATE_SIZE), _rebuild(false)  {

    }
    void ShareIndexLogicManagerI::setValid(bool newState, const Ice::Current& ic)  {
      MCE_INFO("ShareIndexLogicManagerI::setValid-> "<<newState);
      IceUtil::RWRecMutex::WLock lock(_validMutex);
      _valid = newState;
      /* if(_valid)  {
        ServerStateSubscriber::instance().setStatus(1);
      }  else  {
        ServerStateSubscriber::instance().setStatus(0);
      }*/
    }

    bool ShareIndexLogicManagerI::isValid(const Ice::Current& ic)  {
      RWRecMutex::RLock lock(_validMutex);
      return _valid;
    }

    void ShareIndexLogicManagerI::setRecovery(bool newState, const Ice::Current& ic)  {
      MCE_INFO("ShareIndexLogicManagerI::setRecovery-> "<<newState); 
      RWRecMutex::WLock lock(_recoveryMutex); 
      _recovery = newState;
    }


    void ShareIndexLogicManagerI::setRebuild(bool newState, const Ice::Current& ic)  {
      RWRecMutex::WLock lock(_rebuildMutex);  
      _rebuild = newState;

    }
   
    bool ShareIndexLogicManagerI::isRebuild(const Ice::Current& ic)  {
      RWRecMutex::RLock lock(_rebuildMutex);
      return _rebuild;
    }


    bool ShareIndexLogicManagerI::isRecovery(const Ice::Current& ic)  {
      RWRecMutex::RLock lock(_recoveryMutex);
      return _recovery;
    }


    void ShareIndexLogicManagerI::bootstrap(int cluster, int mod, std::string recov_file, std::string modify_title_file)  {

      _cluster = cluster;

      _mod = mod;

      _recov_file = recov_file;

      _modify_title_file = modify_title_file;

      _recovery = false;

      _rebuild = false;

      _allUrlFile.config(_recov_file);

      /*for(int i = 0; i < THREAD_NUM; i++)  {
        _vecLoadUrls.push_back(new LoadUrlThread(i, _allUrlSet[i]));  
      } 

      for(int i = 0; i < THREAD_NUM; i++)  {
        _vecLoadUrls.at(i)->start();
      } */     

      TaskManager::instance().execute(new MonitorRecovDataTask(_recov_file, _recovery, _rebuild));
      MCE_INFO("FINISH BOOT");
    }


    void ShareIndexLogicManagerI::update(const ShareIndexInfo&  info, bool isNew, bool newState, const Ice::Current& ic)  {// 异步调用单个接口
      ShareIndexInfo final;
     
      /*int len = (info.md5).length();
      int mod;
      std::string back2num = (info.md5).substr(len-2, 2);
      sscanf(back2num.c_str(), "%2x", &mod);
      mod %= 100;
      mod /= THREAD_NUM;
      std::string key = info.md5 + info.title;
      MD5 md5_engine;
      md5_engine.reset();
      md5_engine.update(key);
      key = md5_engine.toString();
       */
      //if(_allUrlSet[mod].find(key) == _allUrlSet[mod].end())  {
          final = info;
          //RWRecMutex::WLock lock(_setMutex);
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_setMutex);
        //  _allUrlSet[mod].insert(info.md5);
          if(newState)  {//HA中的两份ShareIndexLogic,只更新其中一份
            _infos.push_back(final);
            MCE_INFO("url+title unique shareinfo: md5="<<final.md5<<"; title="<<final.title);
          }
          if(_infos.size() >= BATCH_UPDATE_SIZE)  {//批量处理
            MCE_INFO("Batch update exceed BATCH_URL_SIZE");
            ShareIndexInfoSeq tmp;
            MCE_INFO("size "<<_infos.size());
            tmp.swap(_infos);
            MCE_INFO("size " <<tmp.size());
            TaskManager::instance().execute(new WriteIndexTask(tmp));
            TaskManager::instance().execute(new WriteUrlLogTask(_allUrlFile, tmp));
          }   /*  else {
            MCE_INFO("Waiting time over 30 s");
            
            _setMutex.timedWait(IceUtil::Time::seconds(30));
            ShareIndexInfoSeq tmp;
            tmp.swap(_infos);
            TaskManager::instance().execute(new WriteIndexTask(tmp));
            TaskManager::instance().execute(new WriteUrlLogTask(_allUrlFile, tmp));
          }*/
          if(isNew)  { //改标题的站外分享，进行另外存储
            MCE_INFO("更改标题的分享为: "<<final.md5);
          //  //IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_setMutex0);
            _modify_titles.push_back(final);
            if(_modify_titles.size() >= BATCH_UPDATE_SIZE)  {
              ShareIndexInfoSeq tmp;
              tmp.insert(tmp.end(), _modify_titles.begin(), _modify_titles.end()); 
              _modify_titles.clear();
              TaskManager::instance().execute(new UpdateModifyTitleTask(_modify_title_file, tmp));
            }
          }
     /*  }  else  {
        MCE_INFO("重复的md5！");
        //final = NULL;
        return;
      }*/
    }



    void ShareIndexLogicManagerI::update(const ShareIndexInfoSeq&  infos, bool newState, const Ice::Current& ic)  {// 异步调用批量接口
      std::set<ShareIndexInfo>  info_set;
      ShareIndexInfoSeq final;
      /*for(ShareIndexInfoSeq::const_iterator iter = infos.begin(); iter != infos.end(); iter++)  {
        int len = (iter->md5).length();
        int mod;
        std::string back2num = (iter->md5).substr(len-2, 2);
        sscanf(back2num.c_str(), "%2x", &mod);
        mod %= 100;
        mod /= THREAD_NUM;
        
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_setMutex);
        if(_allUrlSet[mod].find(iter->md5) == _allUrlSet[mod].end())  {
          info_set.insert(*iter);
          //RWRecMutex::WLock lock(_setMutex);
          _allUrlSet[mod].insert(iter->md5);
        }
      }*/
      if(newState)  {
        for(std::set<ShareIndexInfo>::iterator it = info_set.begin(); it != info_set.end(); it++) {
          final.push_back(*it);
          MCE_INFO(it->md5);
        }
        if(final.size() > 0)  {
          TaskManager::instance().execute(new WriteIndexTask(final));
          //TaskManager::instance().execute(new WriteUrlLogTask(_allUrlFile, final));
        }
        MCE_INFO(final.size());
      }
      MCE_INFO("UPDATE SUCCESS!");   
    }

    AllUrl2File & ShareIndexLogicManagerI::getUrl2File()  {
      return _allUrlFile;
    }

    LoadUrlThread::LoadUrlThread(int db_no, std::set<std::string> & url_set) : _db_no(db_no), _url_set(url_set) {
    }
 
    int LoadUrlThread::itoa(int i, char* string)  {
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
    
    void LoadUrlThread::run()  {
      std::vector<std::string> tables;
      std::string table_prefix;
      char number[10];
      if(_db_no == 0)  {
        for(int i = 0; i < 10; i++)  {
          table_prefix = "share_";
          itoa(i, number);
          table_prefix += (std::string) number;
          tables.push_back(table_prefix);
        }
      } else  {
        char num_suffix[10];
        itoa(_db_no, number);
        for(int i = 0; i < 10; i++)  {
          table_prefix = "share_";
          table_prefix += (std::string) number;
          itoa(i, num_suffix);
          table_prefix += (std::string) num_suffix;
          tables.push_back(table_prefix);
        }  
      }
      for(int i = 0; i < tables.size(); i++)  {
        MCE_INFO(tables.at(i));
      }
      std::string db = "adminsearch";
      for(std::vector<std::string>::iterator iter = tables.begin(); iter != tables.end(); iter++)  {
         std::string beginUrl = "";
         std::string maxUrl = beginUrl;
         //long beginUrl = 0;
         //long maxUrl = 0;
          
        while(beginUrl == "" || maxUrl < beginUrl)  {
          maxUrl = beginUrl;
          Statement sql;
          std::string num = (*iter).substr(6, (*iter).length() -6);
          int i = atoi(num.c_str());
          MCE_INFO(i);
          sql << "SELECT md5, title FROM url_" << i << " WHERE md5 >= \""<< beginUrl << "\" ORDER BY md5 LIMIT "<< BATCH_URL_SIZE;
          //sql << "SELECT id, url_md5, title FROM share_" << i << " WHERE id >= \""<< beginUrl << "\" ORDER BY id LIMIT "<< BATCH_URL_SIZE;
          BatchUrlMd5ResultHandlerI handler(beginUrl, _url_set);
          QueryRunner(db, CDbRServer, *iter).query(sql, handler);
          MCE_INFO("set size is: "<< _url_set.size());
          MCE_INFO(beginUrl); 
        }
      }
      MCE_INFO("Fill task done!");
    }
    
  
    WriteIndexTask::WriteIndexTask(ShareIndexInfoSeq infos) :  _infos(infos)  {

    }

    void WriteIndexTask::handle()  {
      //TaskManager::instance().execute(new WriteUrlLogTask(_url2file, _infos));
      MCE_INFO("Enter into updating thread and info size "<<_infos.size());

      ShareIndexContainerSubBAdapter::instance().update(_infos); 
      ShareIndexContainerSubAAdapter::instance().update(_infos); 
    }

    WriteUrlLogTask::WriteUrlLogTask(AllUrl2File & url2file, ShareIndexInfoSeq infos) : _url2file(url2file), _infos(infos)  {
    }

    void WriteUrlLogTask::handle()  {
      _url2file.setWriteMutex();
      _url2file.writeBuffer(_infos);
      if(_url2file.getUrlSize() > BATCH_UPDATE_SIZE)  {//或等待时间
            ShareIndexInfoSeq tmp;
            tmp.insert(tmp.end(), _url2file.update_urls_.begin(), _url2file.update_urls_.end());
            bool way = ShareIndexLogicManagerI::instance().isRecovery();
            _url2file.writeFile(tmp, way);
            _url2file.clearUrl();    
      }
    }


    UpdateModifyTitleTask::UpdateModifyTitleTask(std::string file, ShareIndexInfoSeq infos) : _modify_title_file(file), _infos(infos)  {
    }

    void UpdateModifyTitleTask::handle()  {
      saveModifyTitle(_infos, false);
    }

    void UpdateModifyTitleTask::setReadMutex()  {
      IceUtil::RWRecMutex::RLock lock(_modify_title_lock);
    }

    bool UpdateModifyTitleTask::saveModifyTitle(ShareIndexInfoSeq urls, bool way)  {
      setReadMutex();
      if(!way)  {
        ofstream output;
        output.open(_modify_title_file.c_str(), ios::app);
        for(ShareIndexInfoSeq::iterator iter = urls.begin(); iter != urls.end(); iter++)  {
          output<<iter->md5<<" "<<iter->url<<" "<<iter->type<<" "<<iter->resourceId<<" "
            <<iter->resourceUserId<<" "<<iter->thumbUrl<<" "<<iter->title<<" "
            <<iter->summary<<" "<<iter->commentCount<<" "<<iter->viewCount<<" "
            <<iter->zanCount<<" "<<iter->shareCount<<" "<<iter->collectionCount<<" "
            <<iter->status<<" "<<iter->creationData<<" "
            <<iter->shareId<<" "<<iter->shareUserId<<endl;
        }
      }  else  {
        ofstream output(_modify_title_file.c_str());
        for(ShareIndexInfoSeq::iterator iter = urls.begin(); iter != urls.end(); iter++)  {
          output<<iter->md5<<" "<<iter->url<<" "<<iter->type<<" "<<iter->resourceId<<" "
            <<iter->resourceUserId<<" "<<iter->thumbUrl<<" "<<iter->title<<" "
            <<iter->summary<<" "<<iter->commentCount<<" "<<iter->viewCount<<" "
            <<iter->zanCount<<" "<<iter->shareCount<<" "<<iter->collectionCount<<" "
            <<iter->status<<" "<<iter->creationData<<" "
            <<iter->shareId<<" "<<iter->shareUserId<<endl;
        }
      }
      return true;
    }



    BatchUrlMd5ResultHandlerI::BatchUrlMd5ResultHandlerI(std::string& maxUrl, 
        std::set<std::string>& allUrlSet) :_maxUrl(maxUrl), _allUrlSet(allUrlSet)  {

    }

    bool BatchUrlMd5ResultHandlerI::handle(mysqlpp::Row& row) const  {   
      std::string urlmd5 = (std::string) row["md5"];
      //long id = (long) row["id"];
      std::string key = urlmd5;
      std::string title = (std::string) row["title"];
      key += title;
      if(urlmd5 != "")  {
        MD5 md5_engine;
        md5_engine.reset();
        md5_engine.update(key);
        key = md5_engine.toString();
        _allUrlSet.insert(key);
      }
      _maxUrl = _maxUrl > urlmd5 ? _maxUrl:urlmd5;
      //_maxUrl = _maxUrl > id ? _maxUrl:id;
      return true;
    }
  }
}
