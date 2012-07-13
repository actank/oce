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
#ifndef __SHAREINDEXLOGICI_H__
#define __SHAREINDEXLOGICI_H__

#include <boost/shared_ptr.hpp>
#include "ShareSearch.h"
#include "ServiceI.h"
#include "IceUtil/IceUtil.h"
#include "QueryRunner.h"
#include "MD5.h"

namespace xce  {
    namespace sharesearch  { 
      typedef boost::shared_ptr<ShareIndexInfo> ShareIndexInfoPtr; 
      const std::string SHARE_INDEX_LOGIC_MANAGER = "SIL";
      const int TASK_LEVEL_FILL = 1; 
      const int TASK_LEVEL_FILL_BASIC = 2;
      const int TASK_LEVEL_FILL_CONFIG = 3;
      const int TABLES_NUM = 100;
      const int BATCH_URL_SIZE = 12000;
      const int BATCH_UPDATE_SIZE = 100;
      const int BATCH_BACK_UP = 10000;
      const int THREAD_NUM = 10;
      const int TASK_LEVEL_UPDATE = 8;
      
      class AllUrl2File  {
        public:
          AllUrl2File(int size);
          void setReadMutex();
          void setWriteMutex();
          bool readFile(const std::string& file);
          bool writeFile(ShareIndexInfoSeq& urls, bool way);
          bool parseStr(const std::string& record, ShareIndexInfo & info); 
          bool writeBuffer(ShareIndexInfoSeq& infos);

          void config(std::string & file);
          int getUrlSize();
          void clearUrl();
          ShareIndexInfoSeq  update_urls_;
        private:
          IceUtil::RWRecMutex update_urls_rw_lock_;
          std::string file_;
      }; 

       class ServiceSchemaI: public MyUtil::Descriptor  {
         public:
           virtual void configure(const Ice::PropertiesPtr& props);
       };
       
       class LoadUrlThread : virtual public IceUtil::Thread  {
         public:
           LoadUrlThread(int db_no, std::set<std::string> & url_set);
           void run();
           int itoa(int i,char* string);
         private:
           IceUtil::RWRecMutex _url_set_lock;
           int _db_no;
           std::set<std::string> & _url_set;
       };

       typedef std::vector<LoadUrlThread *>  LoadUrlsVec;

       class ShareIndexLogicManagerI : virtual public ShareIndexLogicManager,public MyUtil::Singleton<ShareIndexLogicManagerI>  {
         public:
            ShareIndexLogicManagerI();
            void update(const ShareIndexInfoSeq& infos, bool newState, const Ice::Current& ic = Ice::Current());

            virtual void update(const ShareIndexInfo&  info, bool isNew, bool newState, const Ice::Current& ic = Ice::Current());            
            virtual void setValid(bool newState, const Ice::Current& ic= Ice::Current());
            virtual bool isValid(const Ice::Current& ic = Ice::Current());
            virtual void setRecovery(bool newState, const Ice::Current& ic = Ice::Current());//  ice file not add
            virtual bool isRecovery(const Ice::Current& ic = Ice::Current());
            virtual void setRebuild(bool newState, const Ice::Current& ic = Ice::Current());
            virtual bool isRebuild(const Ice::Current& ic = Ice::Current());
            void bootstrap(int cluster, int mod, std::string recov_file, std::string modify_title_file);
           
            AllUrl2File & getUrl2File();

         private:
            std::set<std::string>  _allUrlSet[THREAD_NUM];
            bool _valid;
            IceUtil::RWRecMutex _validMutex;
            ShareIndexInfoSeq _infos;
            ShareIndexInfoSeq _modify_titles;
            bool _recovery;
            IceUtil::RWRecMutex _recoveryMutex;
            IceUtil::RWRecMutex _rebuildMutex;
            //IceUtil::RWRecMutex _setMutex;
            IceUtil::Monitor<IceUtil::Mutex> _setMutex;
            IceUtil::Monitor<IceUtil::Mutex> _setMutex0;
            std::string _modify_title_file;
            AllUrl2File  _allUrlFile;
            bool _rebuild;
            std::string _recov_file;
            LoadUrlsVec _vecLoadUrls;
            int _mod;
            int _cluster;
       };
       
       class BatchUrlMd5ResultHandlerI: public com::xiaonei::xce::ResultHandler {
         public:
           BatchUrlMd5ResultHandlerI(std::string& maxUrl, std::set<std::string> & allUrlSet);
           virtual bool handle(mysqlpp::Row& row) const;
         private:
           std::string & _maxUrl;
           //long & _maxUrl;
           std::set<std::string> & _allUrlSet;
       };

       
       class MonitorRecovDataTask : virtual public MyUtil::Task {
         public:
           MonitorRecovDataTask(string file, bool & recov_symbol, bool & rebuild_symbol);
           virtual void handle();
         private:
           string _file;
           bool & _recov_symbol;
           bool & _rebuild_symbol;
           IceUtil::Monitor<IceUtil::Mutex> _recovMutex;
           IceUtil::Monitor<IceUtil::Mutex> _rebuildMutex;
       };

       class WriteIndexTask : virtual public MyUtil::Task  {
         
         public:
           WriteIndexTask(ShareIndexInfoSeq infos);
           virtual void handle();
         private:
           ShareIndexInfoSeq _infos;
       };

       class WriteUrlLogTask : virtual public MyUtil::Task  {
         public:
           WriteUrlLogTask(AllUrl2File & url2file, ShareIndexInfoSeq infos);
           virtual void handle();
         private:
           AllUrl2File & _url2file;
           ShareIndexInfoSeq _infos;
       };

       class UpdateModifyTitleTask : virtual public MyUtil::Task {
         public:
           void setReadMutex();
           bool saveModifyTitle(ShareIndexInfoSeq urls, bool way);
           UpdateModifyTitleTask(std::string file, ShareIndexInfoSeq infos);
           virtual void handle();
         private:
           std::string  _modify_title_file;
           ShareIndexInfoSeq _infos;
           IceUtil::RWRecMutex _modify_title_lock;
       };


    }     
  }
#endif
