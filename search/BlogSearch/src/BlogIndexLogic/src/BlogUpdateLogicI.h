/*
 * =====================================================================================
 *
 *       Filename:  ShareIndexLogicI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年10月12日 17时56分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef __BLOGINDEXLOGICI_H__
#define __BLOGINDEXLOGICI_H__

#include <boost/shared_ptr.hpp>
#include "ContentSearch.h"
#include "ServiceI.h"
#include "IceUtil/IceUtil.h"
#include "QueryRunner.h"
#include "BlogSubIndexAdapter.h"
#include "time.h"


using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace IceUtil;

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

string dataPre;
string dataNow; 


namespace com  {
  namespace xiaonei {
    namespace search5  {
      namespace logicserver  {

        
        const std::string BLOG_INDEX_LOGIC_MANAGER = "BIL";
        const int TASK_LEVEL_FILL = 1; 
        const int TASK_LEVEL_FILL_BASIC = 2;
        const int TASK_LEVEL_FILL_CONFIG = 3;
        const int TASK_LEVEL_UPDATE = 8;
        int kMaxNumEachProcess;
        double kInterval;
       
         
        
        class ServiceSchemaI: public MyUtil::Descriptor  {
          public:
            virtual void configure(const Ice::PropertiesPtr& props);
        };

       class MonitorPrintLogTask : virtual public MyUtil::Task  {
         public:
           MonitorPrintLogTask();
           virtual void handle();
       };

        class WriteIndexTask : virtual public MyUtil::Task  {

          public:
            WriteIndexTask(int mod, IndexUpdateDataSeq infos);
            virtual void handle();
          private:
            int _mod; 
            IndexUpdateDataSeq _infos;
        };

        class WriteUpdateDataTask : virtual public MyUtil::Task  {
          public:
            WriteUpdateDataTask(IndexUpdateDataSeq infos, int mod);
            virtual void handle();
          private:
            IndexUpdateDataSeq _infos;
            int _mod;
        };

        
        class WriteDelUpdateFileTask : virtual public MyUtil::Task  {
          public:
            WriteDelUpdateFileTask(vector<string> info_del, IndexUpdateDataSeq info_update, int mod);
            virtual void handle();
          private:
            vector<string> _info_del;
            IndexUpdateDataSeq _info_update;
            int _mod;
        };
       


        class AllBlog2File  {
          public:
            AllBlog2File(int size);
            void setReadMutex();
            void setWriteMutex();
            void setUnlock();
            bool readFile(const std::string& file);
            bool writeFile(IndexUpdateDataSeq& data, int mod,  bool way);
            bool writeDelFile(vector<string>& data, int mod);
            bool writeUpdateFile(IndexUpdateDataSeq& data, int mod); 
            
           // bool writeOperation(IndexUpdateDataSeq& data, bool way);
            bool parseStr(const std::string& record, IndexUpdateData & info);
            bool writeBuffer(IndexUpdateDataSeq infos);

            void config(std::string & del_file, std::string & update_file, std::string * index_path/*std::string & file, std::string & file_op*/);
            int getBlogSize();
            void clearBlog();
            IndexUpdateDataSeq  update_data_;
          private:
            IceUtil::RWRecMutex update_data_rw_lock_;
            string * index_path_;
            string del_file_;
            string update_file_; 
            
            //std::string file_;
            //std::string file_op_;
        };

        class BatchBlogResultHandlerI: public com::xiaonei::xce::ResultHandler {
          public:
            BatchBlogResultHandlerI(IndexUpdateData& data);
            virtual bool handle(mysqlpp::Row& row) const;
          private:
            IndexUpdateData & _data;
        };

        class MessageQueueThread : virtual public IceUtil::Thread {

          public:
            MessageQueueThread(int index_mod) {
              _index_mod = index_mod;
            }
           
            void invoke(const IndexUpdateData& unit) {
              {
                IndexUpdateData tmp = unit;
                IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
                if(tmp.operationType ==2)  {
                  _del_identity_set.insert(tmp.index["id"]);
                }
                else if(tmp.operationType == 1)  {
                  /*  string db_name = "adminsearch";
                  int owner_id = atoi(tmp.index["owner"].c_str());
                  int db_mod = (owner_id / 100) % 5;
                  char blog_no[10];
                  itoa(db_mod, blog_no);
                  string blog_table_name = "blog_"+(string)(blog_no);
                  Statement sql;
                  sql << "SELECT id, owner, name, headurl, tinyurl, body,comment_count, title, time, uptime, view_count, month, blog_control, cmd_count, univ, blog_pic_id, draft_flag, stage, type, original_url, category_id FROM blog_" << db_mod << " WHERE owner= "<< owner_id;
                  BatchBlogResultHandlerI handler(tmp);
                  QueryRunner(db_name, CDbRServer, blog_table_name).query(sql, handler);*/
                  _update_map[tmp.index["id"]] = tmp;
                } 
                _cache.push_back(tmp);
                MCE_INFO("[Mod="<<_index_mod<<"]"<<" Vec Len:"<<_cache.size());
                if (_cache.size() > kMaxNumEachProcess) {
                  _mutex.notify();
                }
              }
            }

            void run() {

              IndexUpdateDataSeq batch;
              while(true) {
                {
                  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
                  if (_cache.size() < kMaxNumEachProcess) {
                    _mutex.timedWait(IceUtil::Time::seconds(kInterval));
                  }
                  batch.swap(_cache); 
                }

                if (!batch.empty())  {
                  MCE_INFO("start writing data to file!");

                  TaskManager::instance().execute(new WriteIndexTask(_index_mod, batch));
                  TaskManager::instance().execute(new WriteUpdateDataTask(batch, _index_mod));
                  if(dataNow != dataPre)  {
                    vector<string> del_list;
                    IndexUpdateDataSeq update_list;
                    for(set<string>::iterator iter = _del_identity_set.begin(); iter != _del_identity_set.end(); iter++)  {
                      del_list.push_back(*iter);
                    }
                    for(map<string, IndexUpdateData>::iterator iter = _update_map.begin(); iter != _update_map.end(); iter++)  {
                      if(_del_identity_set.find((iter->second).index["id"]) == _del_identity_set.end())  {
                         update_list.push_back(iter->second);
                      }
                    }
                    TaskManager::instance().execute(new WriteDelUpdateFileTask(del_list, update_list, _index_mod));
                    _del_identity_set.clear();
                    _update_map.clear(); 
                  }
                }
                batch.clear();
              }
            }
          private:
            int _index_mod; 
            IceUtil::Monitor<IceUtil::Mutex> _mutex;
            IndexUpdateDataSeq _cache;
            std::set<std::string> _del_identity_set;
            std::map<std::string, IndexUpdateData> _update_map;
        };


        class BlogUpdateLogicI : virtual public IndexUpdateLogic,public MyUtil::Singleton<BlogUpdateLogicI>  {
          public:
            BlogUpdateLogicI();

            virtual void update(const IndexUpdateData&  info, const Ice::Current& ic = Ice::Current());            
            
            virtual void setValid(bool newState, const Ice::Current& ic= Ice::Current());
            
            virtual bool isValid(const Ice::Current& ic = Ice::Current());

            virtual void setRecovery(bool newState, const Ice::Current& ic = Ice::Current());//  ice file not add

            virtual bool isRecovery(const Ice::Current& ic = Ice::Current());

            void bootstrap(int ha_service_cluster, int ha_service_mod, int index_cluster, std::string * index_path,     std::string del_file, std::string update_file /*  std::string file, std::string file_op  , std::string zookeeper_addr, std::string container_path*/);
            
            AllBlog2File & getBlog2File();

          private:

            bool _valid;

            IceUtil::RWRecMutex _validMutex;

            IceUtil::Monitor<IceUtil::Mutex> _setMutex;

            int _ha_service_cluster;

            int _ha_service_mod;

            int _index_cluster;

          //  std::string _zookeeper_addr;

          //  std::string _container_path;

            vector<MessageQueueThread *> _threads;
        
            AllBlog2File _allBlogFile; 
        };
      }
    }
  }     
}
#endif
