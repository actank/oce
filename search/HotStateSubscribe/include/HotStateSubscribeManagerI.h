/*
 * =====================================================================================
 *
 *       Filename:  KeyWordExtraction.h
 *
 *    Description:  Extract the states in which contain the specified key words
 *
 *        Version:  1.0
 *        Created:  04/25/2011 04:08:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef __HOTSTATESUBSCRIBE_H__
#define __HOTSTATESUBSCRIBE_H__

#include <boost/shared_ptr.hpp>
#include <boost/circular_buffer.hpp>
#include "time.h"
#include "ContentSearch.h"
#include "HotStateSubscribe.h"
#include "ServiceI.h"
#include "IceUtil/IceUtil.h"
#include "QueryRunner.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "XceCache.h"
//#include "curl/curl.h"
//#include "search/Util/MD5.h"
#include <boost/regex.hpp>

using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace IceUtil; 
using namespace std;
using namespace com::xiaonei::search5::logicserver; 
static const string STATE_CONTENT = "content";
static const string STATE_ROOT_CONTENT = "rootContent";
static const string USER_FIELD = "userid";
static const string IP = "ip";
static const string ADDRESS = "addrs";

static const string BUSINESS_TYPE = "business_type";
static const string KEY_WORDS = "key_words";
static const string REALTIME_NUM = "real_time_num";
static const string SUBSCRIBE_TIME = "subscribe_time";
static const string MAX_STATE_NUM = "max_state_num";
static const string SUBSCRIBE_CONTROL = "subscribe_control";
static const string SUBSCRIBE_DB = "common"; 

static const int UPDATE_INTERVAL = 5*60;//flush the REALTIME_NUM to db in message_queue every five minute;


int itoa(int i, char* string); 

string getCurrentTime();

namespace com  {
  namespace xiaonei {
    namespace search  {
      namespace state  {

        static double kInterval;
        static int kMaxNumEachProcess;

        struct ip_unit  {
          unsigned int ip;
          string addrs;
        };
 
        struct sub_type  {
          string type;
          string key_words;
          int real_time_num;
          string subscribe_time;
          int max_state_num;
          short subscribe_control;
        };


        class ExtractKeyWordsTask : virtual public MyUtil::Task  {
          public:
            ExtractKeyWordsTask(const IndexUpdateDataSeq& infos);
            virtual void handle();
          private:
            IndexUpdateDataSeq _infos;
            int _service_mod;
        };


        class SubScribeInfoHandler : virtual public com::xiaonei::xce::ResultHandler,
        public MyUtil::Singleton<SubScribeInfoHandler> {
          public:
            SubScribeInfoHandler(map<string, boost::shared_ptr<sub_type> >& infos): _infos(infos){}
          protected:
            virtual bool handle(mysqlpp::Row& res) const;
          private:
            map<string, boost::shared_ptr<sub_type> >& _infos;
        };

        /* 处理推送数据请求 */
        class MessagePushThread : virtual public IceUtil::Thread {

          public:
            MessagePushThread() {
            }

            void invoke(const IndexUpdateData& unit) {
              {
                IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
                _cache.push_back(unit);
                //MCE_INFO("[MessagePushThread]"<<" Vec Len:"<<_cache.size());
                if ((int)_cache.size() > kMaxNumEachProcess) {
                  _mutex.notify();
                }
              }
            }

            void run() {

              while(true) {
              IndexUpdateDataSeq batch;
                {
                  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
                  _mutex.timedWait(IceUtil::Time::seconds((IceUtil::Int64)kInterval));
                  batch.swap(_cache); 
                }

                if (!batch.empty())  {
                  TaskManager::instance().execute(new ExtractKeyWordsTask(batch));
                }
              }
            }
          private:
            IceUtil::Monitor<IceUtil::Mutex> _mutex;
            IndexUpdateDataSeq _cache;
        };


       /*消息队列，供外部Adapter取数据 */
        class MessageQueueThread : virtual public IceUtil::Thread {

          public:
            MessageQueueThread(string pattern, int max_size, bool subscribe_control) {
              _pattern = pattern;
              _max_size = max_size;
              boost::shared_ptr<char> time_char(new char[25]);
              _time_stamp = "0";
              _subscribe_control = subscribe_control;
              _time_count = 0;
              _message_queue = boost::circular_buffer<IndexUpdateData>(max_size);
            }
            
            ~MessageQueueThread()  {
            }
          

            string getTimeStamp()  {
              IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
              return _time_stamp;
            } 

            void setSubscribeControl(short control)  {
              IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
              _subscribe_control = control;  
            }

            short getSubscribeControl()  {
              IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
              return _subscribe_control; 
            }

            
            int getQueueMaxSize() {
              IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
              return _max_size;
            }
            
            int getQueueCurrentSize()  {
              return _message_queue.size(); 
            }

            bool check(const string& pattern, const string& content)  {
              boost::regex oRegex(pattern.c_str());
              boost::smatch oResults;
              if(boost::regex_search(content, oResults, oRegex))  {
                return true;
              } else  {
                return false;
              }
            }

            void updateConfig2Db(map<string, string>  update_value)  {
              Statement sql;
              MCE_INFO("updateConfig2Db enter...");
              sql << "UPDATE IGNORE state_subscribe SET "; 
              for(map<string, string>::iterator iter = update_value.begin(); iter != update_value.end(); iter++)  {
                if(iter->first == KEY_WORDS)  {
                  sql << KEY_WORDS << "=" << iter->second; 
                } else if(iter->first == REALTIME_NUM)  {
                  sql << REALTIME_NUM << "=" << iter->second;
                } else if(iter->first == SUBSCRIBE_TIME)  {
                  sql << SUBSCRIBE_TIME << "=" << iter->second;
                } else if(iter->first == MAX_STATE_NUM)  {
                  sql << MAX_STATE_NUM << "=" << iter->second;
                } else if(iter->first == SUBSCRIBE_CONTROL)  {
                  sql << SUBSCRIBE_CONTROL << "=" << iter->second;
                }  else {
                  continue;
                }
              }
              QueryRunner(SUBSCRIBE_DB, CDbWServer).execute(sql);
            }

            void deleteConfig2Db(const string & primary_key)  {
              Statement sql;
              sql << "DELETE FROM state_subscribe WHERE " << BUSINESS_TYPE << "=" <<mysqlpp::quote<<primary_key; 
              QueryRunner(SUBSCRIBE_DB, CDbWServer).execute(sql);
            }



            void invoke(const IndexUpdateDataSeq& unit) {
              for(IndexUpdateDataSeq::const_iterator iter = unit.begin(); iter != unit.end(); iter++)  {
                string state_content = "", state_root_content = "";
                if(iter->index.find(STATE_CONTENT) != iter->index.end())  {
                  state_content = iter->index.find(STATE_CONTENT)->second;
                }
                if(iter->index.find(STATE_ROOT_CONTENT) != iter->index.end())  {
                  state_root_content = iter->index.find(STATE_ROOT_CONTENT)->second;
                }
                string sentence = state_content + " " +state_root_content;
                if(sentence != " ")  {
              //MCE_INFO("MessageQueueThread::check before sentence = "<<sentence<<" pattern = "<< _pattern);
                  if(check(_pattern, sentence))  { 
                    _message_queue.push_back(*iter);
                   MCE_INFO("check pattern : "<<_pattern<<" 命中state: "<<sentence); 
                  }
                }  
              }
            }
           
            void push(const IndexUpdateData& data)  {
              IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
              _common_data.push_back(data);   
            } 

            IndexUpdateDataSeq getNewState(int limit)  {
              IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
              IndexUpdateDataSeq info_list;
                if(!_message_queue.empty())  {
                  int message_size = (int)_message_queue.size(); 
                  int len = limit > message_size ? message_size: limit;
                  int i, j = 0; 
                  for(i = message_size-1; i >= 0 && j < len; i--,j++)  {
                    info_list.push_back(_message_queue[i]);
                  }
                }  else  {
                  MCE_WARN("MessageQueueThread is empty!");
                }
              return info_list; 
            }

            void run() {
              while(true) {
                IndexUpdateDataSeq tmp; 
                {  
                  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
                  _mutex.timedWait(IceUtil::Time::seconds(1));
                  if(_common_data.size() > 0)  {
                  _common_data.swap(tmp);
                  if(  _time_count != 0 && _time_count % UPDATE_INTERVAL == 0)  {
                    map<string, string> update_field;
                    char buffer[30];
                    int size = getQueueCurrentSize();
                    itoa(size , buffer);
                    update_field[REALTIME_NUM] = (string) buffer;
                    updateConfig2Db(update_field);
                    _time_count = 0;
                  }
                    invoke(tmp); 
                  _time_count++;
                  }
                }  
                
                if(!_subscribe_control)  {
                  break;
                }
              }
            }
          
          private:
            string _pattern;
            int _max_size; 
            int _time_count;
            string _time_stamp;
            short _subscribe_control; 
            boost::circular_buffer<IndexUpdateData> _message_queue;
            vector<IndexUpdateData>  _common_data;
            IceUtil::Monitor<IceUtil::Mutex> _mutex;
        };


        class HotStateSubscribeManagerI : virtual public HotStateSubscribe, public MyUtil::Singleton<HotStateSubscribeManagerI>  {
          public:
            HotStateSubscribeManagerI();

            virtual void update(const IndexUpdateData&  info, const Ice::Current& ic = Ice::Current());            
            virtual bool test(const string& pattern, const string& inputState, const Ice::Current& ic = Ice::Current());  
           
            virtual vector<map<string, string> > getNewState(const string& type, int limit, const Ice::Current& ic = Ice::Current());

            virtual bool subscribe(const string& type, const string& pattern, int max_size, const string& passwd, const Ice::Current& ic = Ice::Current());

            virtual bool unSubscribe(const string& type, const Ice::Current& ic = Ice::Current());

            virtual vector<map<string, string> > showTypeInfos(const Ice::Current& ic = Ice::Current());

            virtual map<string, string> showTypeInfo(const string& type, const Ice::Current& ic = Ice::Current());

            void bootstrap(string& key_words_file, string& ip_address_file);
            void KeyWordsExtract(const IndexUpdateData& unit, string& output_data);
            unsigned int ip2int(string ip);
            bool check(unsigned int value, string& addrs);
            void init(string file);
          
          
          private:
            map<string, MessageQueueThread *> _queue_thread; 
            map<string, boost::shared_ptr<sub_type> > _type_subs;
            bool _valid;
            IceUtil::RWRecMutex _validMutex;
            IceUtil::Monitor<IceUtil::Mutex> _setMutex;
            MessagePushThread * _push_thread;
            vector<string> _key_words;
            vector<ip_unit *> _ip_db;
        };
      
         
      }
    }
  }     
}
#endif

