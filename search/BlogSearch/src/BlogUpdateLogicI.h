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



using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace IceUtil;

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
            WriteUpdateDataTask(IndexUpdateDataSeq infos);
            virtual void handle();
          private:
            IndexUpdateDataSeq _infos;
        };

        class AllBlog2File  {
          public:
            AllBlog2File(int size);
            void setReadMutex();
            void setWriteMutex();
            bool readFile(const std::string& file);
            bool writeFile(IndexUpdateDataSeq& data, bool way);
            bool parseStr(const std::string& record, IndexUpdateData & info);
            bool writeBuffer(IndexUpdateDataSeq& infos);

            void config(std::string & file);
            int getBlogSize();
            void clearBlog();
            IndexUpdateDataSeq  update_data_;
          private:
            IceUtil::RWRecMutex update_data_rw_lock_;
            std::string file_;
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
                  MCE_INFO("Thread alive"); 
                  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
                  if (_cache.size() < kMaxNumEachProcess) {
                    _mutex.timedWait(IceUtil::Time::seconds(kInterval));
                  }
                  batch.swap(_cache); 
                }

                if (!batch.empty())  {
                  MCE_INFO("Invoked success!");
                  TaskManager::instance().execute(new WriteIndexTask(_index_mod, batch));
                  TaskManager::instance().execute(new WriteUpdateDataTask(batch));
                }
                batch.clear();
              }
            }
          private:
            int _index_mod; 
            IceUtil::Monitor<IceUtil::Mutex> _mutex;
            IndexUpdateDataSeq _cache;
        };


        class BlogUpdateLogicI : virtual public IndexUpdateLogic,public MyUtil::Singleton<BlogUpdateLogicI>  {
          public:
            BlogUpdateLogicI();

            virtual void update(const IndexUpdateData&  info, const Ice::Current& ic = Ice::Current());            
            
            virtual void setValid(bool newState, const Ice::Current& ic= Ice::Current());
            
            virtual bool isValid(const Ice::Current& ic = Ice::Current());

            virtual void setRecovery(bool newState, const Ice::Current& ic = Ice::Current());//  ice file not add

            virtual bool isRecovery(const Ice::Current& ic = Ice::Current());

            void bootstrap(int ha_service_cluster, int ha_service_mod, int index_cluster, std::string file, std::string zookeeper_addr, std::string container_path);
            
            AllBlog2File & getBlog2File();

          private:

            bool _valid;

            IceUtil::RWRecMutex _validMutex;

            IceUtil::Monitor<IceUtil::Mutex> _setMutex;

            int _ha_service_cluster;

            int _ha_service_mod;

            int _index_cluster;

            std::string _zookeeper_addr;

            std::string _container_path;

            vector<MessageQueueThread *> _threads;
        
            AllBlog2File _allBlogFile; 
        };
      }
    }
  }     
}
#endif
