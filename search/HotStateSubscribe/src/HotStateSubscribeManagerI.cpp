/*
 * =====================================================================================
 *
 *       Filename:  KeyStateWordsExtraction.cpp
 *
 *    Description:  Extract the states in which contain the specified key words
 *
 *        Version:  1.0
 *        Created:  04/25/2011 06:38:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */

#include "../include/HotStateSubscribeManagerI.h"
using namespace com::xiaonei::xce;
using namespace xce::serverstate;

int itoa(int i, char* string)  {
  int power,j; j=i;
  int count = 0;
  if(i == 0)  {
    strcpy(string, "0");
    return 1;
  } 
  for(power=1;j>=10;j/=10) power*=10;
  for(;power>0;power/=10) {
    *string++ ='0' + i/power;
    i%=power;
    count ++;
  }
  *string='\0';
  return count;
}

string getCurrentTime()  {
  time_t t;
  struct tm * a;
  time(&t);
  a=localtime(&t);
  char current_time[30];
  sprintf(current_time, "%4d-%02d-%02d %02d:%02d:%02d", 1900+a->tm_year,1+a->tm_mon,a->tm_mday,a->tm_hour,a->tm_min,a->tm_sec);
  return (string)current_time;
}

namespace MyUtil  {
  void initialize()  {
    ServiceI& service = ServiceI::instance();
    string service_name = service.getName();
    service_name = service_name.substr(0, service_name.size() -2);
    string key_words_file,  ip_address_file, controller_name = "ControllerStateKeyWords";
    int time_interval, batch_process;
    MCE_INFO("service_name:"<<service_name);
    key_words_file = service.getCommunicator()->getProperties()->getProperty(service_name + ".KeyStateWordsFile");
    time_interval = service.getCommunicator()->getProperties()->getPropertyAsInt(service_name + ".TimeInterval");
    batch_process = service.getCommunicator()->getProperties()->getPropertyAsInt(service_name + ".BatchProcess");
    ip_address_file = service.getCommunicator()->getProperties()->getProperty(service_name + ".IpAddressFile");
    com::xiaonei::search::state::kInterval = (double) time_interval;
    com::xiaonei::search::state::kMaxNumEachProcess = batch_process;
    ServerStateSubscriber::instance().initialize(controller_name, &com::xiaonei::search::state::HotStateSubscribeManagerI::instance(), 0);

    //com::xiaonei::search::state::HotStateSubscribeManagerI::instance().setValid(true);
    MCE_INFO("key_words_file is "<<key_words_file<<"  ip_address_file is "<<ip_address_file);
    com::xiaonei::search::state::HotStateSubscribeManagerI::instance().bootstrap(key_words_file, ip_address_file);

  }
}

namespace com  {
  namespace xiaonei  {
    namespace search  {
      namespace state  {

        using namespace MyUtil;
        using namespace IceUtil;
        using namespace com::xiaonei::xce;

       ExtractKeyWordsTask::ExtractKeyWordsTask(const IndexUpdateDataSeq& infos) :   _infos(infos)  {
        }

       void ExtractKeyWordsTask::handle()  {
         try  {
             for(IndexUpdateDataSeq::iterator iter = _infos.begin(); iter != _infos.end();  iter++)  {
               string output_data = "state=&address=";
               HotStateSubscribeManagerI::instance().KeyWordsExtract(*iter, output_data);
               /*   First set the URL that is about to receive our POST. This URL can
                *        *         just as well be a https:// URL if that is what should receive the
                *             *                data. */
               if(output_data != "state=&address=")  {
                 ostringstream cmd;

                 cmd << "curl -d \""<< output_data.c_str() 
                   << "\" http://adsupport.renren.com/aerbeisi/admin/saveState.do";
                 system(cmd.str().c_str());
                 
                 MCE_INFO("HttpPost data : "<<output_data);
               }
             }
         }  catch  (...) {
           MCE_INFO("KeyStateWordsAdapter exception");
         }
       }
      
       bool SubScribeInfoHandler::handle(mysqlpp::Row& res) const {
         boost::shared_ptr<sub_type> sub_ptr(new sub_type);
         string type = (string) res[BUSINESS_TYPE.c_str()];
         sub_ptr->type = type;
         sub_ptr->key_words = (string) res[KEY_WORDS.c_str()];
         MCE_INFO("pattern read from db "<<res[KEY_WORDS.c_str()].c_str() );
         sub_ptr->real_time_num = (int) res[REALTIME_NUM.c_str()];
         sub_ptr->subscribe_time = (string) res[SUBSCRIBE_TIME.c_str()];
         sub_ptr->max_state_num = (int) res[MAX_STATE_NUM.c_str()];
         sub_ptr->subscribe_control =  (short) res[SUBSCRIBE_CONTROL.c_str()];
         _infos[type] = sub_ptr; 
         return true; 
       }



        HotStateSubscribeManagerI::HotStateSubscribeManagerI()  {

        }

        unsigned int HotStateSubscribeManagerI::ip2int(string ip)  {
          unsigned int first, second, third;
          first = ip.find(".");
          second = ip.find(".", first + 1);
          third = ip.find(".", second + 1);
          unsigned int forth_value = atoi(ip.substr(0, first).c_str())*256*256*256;
          unsigned int third_value = atoi(ip.substr(first+1, second-first-1).c_str())*256*256;
          unsigned int second_value = atoi(ip.substr(second+1, third - second - 1).c_str())*256;
          unsigned int first_value = atoi(ip.substr(third+1, ip.length() - third - 1).c_str());
          unsigned int value = first_value + second_value + third_value + forth_value;
          if(value < 0)  {
            MCE_WARN("ip: "<<ip<<" multiply value is negative");
          }
          return value;
        }
           
        void HotStateSubscribeManagerI::init(string file)  {
          ifstream input(file.c_str());
          string line, name;
          int tab_first, tab_first_start, tab_second, tab_second_start, tab_third;
          unsigned int start_ip, end_ip; 
          while(!input.eof())  {
            getline(input, line, '\n');
            tab_first = line.find(" ");
            tab_first_start = tab_first;
            while(line[tab_first_start] == ' ')  {
              tab_first_start++;
            }
            if((size_t)tab_first_start == string::npos)  {
              continue;
            }
            tab_second = line.find(" ", tab_first_start);
            tab_second_start = tab_second;
            if((size_t)tab_second == string::npos)  {
              continue;
            }
            while(line[tab_second_start] == ' ')  {
              tab_second_start++;
            }
            if((size_t)tab_second_start == string::npos)  {
              continue;
            }
            tab_third = line.find(" ", tab_second_start);
            if((size_t)tab_third == string::npos)  {
              continue;
            }
            start_ip = ip2int(line.substr(0, tab_first));
            end_ip = ip2int(line.substr(tab_first_start, tab_second - tab_first_start));
            name = line.substr(tab_second_start, tab_third - tab_second_start);
            if(start_ip < 0 || end_ip < 0)  {
              MCE_INFO("start_ip : "<<start_ip<<" end_id : "<<end_ip<<" ip is negative");
            }
            if(name.size() <= 0)  {
              continue;
            }
            ip_unit * unit1 = new ip_unit();
            unit1->ip = start_ip;
            unit1->addrs = name;
            ip_unit *  unit2 = new ip_unit();
            unit2->ip = end_ip;
            unit2->addrs = name;
            _ip_db.push_back(unit1);
            _ip_db.push_back(unit2);
          }
        }


        bool HotStateSubscribeManagerI::check(unsigned int value, string& addrs)  {
          if(_ip_db.size() > 0)  {
            unsigned int begin = 0, end = _ip_db.size() - 1;
            while(begin < end)  {
              unsigned int mid = (begin + end)/2;
              cout<<"mid "<<_ip_db.at(mid)->ip<<endl;
              if(mid == 0 && value == _ip_db.at(0)->ip) {
                addrs = _ip_db.at(0)->addrs;
                return true;
              }
              if(mid == _ip_db.size() - 1 && value == _ip_db.at(mid)->ip)  {
                addrs = _ip_db.at(mid)->addrs;
                return true;
              }
              if((_ip_db.at(mid)->ip  <=  value && value <= _ip_db.at(mid+1)->ip) ||
                  (_ip_db.at(mid)->ip  >=  value && value >= _ip_db.at(mid-1)->ip))  {
                addrs = _ip_db.at(mid)->addrs;
                return true;
              }
              if(_ip_db.at(mid)->ip > value)  {
                end = mid;
              }
              if(_ip_db.at(mid)->ip < value)  {
                begin = mid;
              }
            }
          }
          addrs = "";
          return false;
        }

        void HotStateSubscribeManagerI::KeyWordsExtract(const IndexUpdateData& unit, string& output_data)  {
          string state_contents = ""; 
          string state_content = "", state_root_content = "";
          if(unit.index.find(STATE_CONTENT) != unit.index.end())  {
            state_content = unit.index.find(STATE_CONTENT)->second;
          }
          if(unit.index.find(STATE_ROOT_CONTENT) != unit.index.end())  {
            state_root_content = unit.index.find(STATE_ROOT_CONTENT)->second;
          }
          
          for(vector<string>::iterator iter = _key_words.begin(); iter != _key_words.end(); iter++)  {
           
            boost::regex oRegex((*iter).c_str());
            boost::smatch oResults;
            if( boost::regex_search(state_content, oResults, oRegex) /*state_content.find(*iter) != string::npos*/)  {
              if(state_root_content.length() > 0)  {
                state_contents += state_root_content;
                state_contents += " ";
              }
              state_contents += state_content;
              output_data = "state=";
              output_data += state_contents;
              
              string addrs =  "", state_ip;
              if(unit.index.find(IP) != unit.index.end())  {
                state_ip = unit.index.find(IP)->second;
                unsigned int ip_value = ip2int(state_ip);
                check(ip_value, addrs);
              }
              output_data += "&address=";
              if(addrs == "" || addrs == "本机地址" || addrs == "纯真网络")  {
                  output_data = "state=&address=";
              }  else  {
                  output_data += addrs;
              }
              MCE_INFO("[HotStateSubscribeManagerI::HttpPost] pattern : "<<*iter<<" 命中state_content: "<<state_content);
              break;  
            }  else if( boost::regex_search(state_root_content, oResults, oRegex) /* state_root_content.find(*iter) != string::npos*/)  {
              if(state_root_content.length() > 0)  {
                state_contents += state_root_content;
                state_contents += " ";
              }
              state_contents += state_content;
              output_data = "state=";
              output_data += state_contents;
              string addrs =  "", state_ip;
              if(unit.index.find(IP) != unit.index.end())  {
                state_ip = unit.index.find(IP)->second;
                unsigned int ip_value = ip2int(state_ip);
                check(ip_value, addrs);
              } 
              output_data += "&address=";
              if(addrs == "" || addrs == "本机地址" || addrs == "纯真网络")  {
                  output_data = "state=&address=";
              }  else  {
                  output_data += addrs;
              }
              MCE_INFO("[HotStateSubscribeManagerI::HttpPost] pattern : "<<*iter<<" 命中state_root_content: "<<state_root_content);
              break;  
            }
          }
        }


        void HotStateSubscribeManagerI::bootstrap(string& key_words_file, string& ip_address_file)  {
          ifstream key_words_stream(key_words_file.c_str());
          string line; 
          while(!key_words_stream.eof())  {
            getline(key_words_stream, line, '\n');
            if(line == "" || line.size() == 0)  {
              continue;
            }
            _key_words.push_back(line);
          } 
          init(ip_address_file);
          
          {
            IceUtil::RWRecMutex::WLock lock(_validMutex);
            SubScribeInfoHandler handler(_type_subs);  
            Statement sql;
            sql << "SELECT business_type, key_words, real_time_num,subscribe_time, max_state_num, subscribe_control FROM state_subscribe";
            QueryRunner("common", CDbRServer).query(sql, handler);
            for(map<string, boost::shared_ptr<sub_type> >::iterator iter = _type_subs.begin(); iter != _type_subs.end(); iter++)  {
              if(iter->second->type != "")  {
                MessageQueueThread * thread = new MessageQueueThread(iter->second->key_words, iter->second->max_state_num, true);
                thread->start();
                _queue_thread[iter->second->type] = thread;
              }
            } 
          } 
          
          _push_thread = new MessagePushThread();
          _push_thread->start();
          MCE_INFO("FINISH BOOT");
        }

        void HotStateSubscribeManagerI::update(const IndexUpdateData&  info, const Ice::Current& ic)  {// 异步调用单个接口
          IndexUpdateData final;
          final = info;
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_setMutex);
          if((string)final.index[USER_FIELD] != "")  {  
            _push_thread->invoke(info);
            for(map<string, MessageQueueThread *>::iterator iter = _queue_thread.begin(); iter != _queue_thread.end(); iter++)  {
              (iter->second)->push(info);
            }
          }
        }


        bool HotStateSubscribeManagerI::subscribe(const string& type, const string& pattern, int max_size, const string& passwd, const Ice::Current& ic)  {
          /*MD5 transform_md5;
          char mem[12];
          transform_md5.itoa(max_size, mem);
          string final_key = "";
          final_key += type;
          final_key += pattern;
          final_key += (string) mem;
          transform_md5.reset();
          transform_md5.update(final_key);*/
          //if(transform_md5.toString() == passwd)  {//check right)
          if("d21babe935006b7c102019983e322ed4" == passwd)  {//check right)
            Statement sql;
            sql << "INSERT IGNORE into state_subscribe(business_type, key_words, real_time_num, subscribe_time, max_state_num, subscribe_control) values("<<mysqlpp::quote<<type<<","<<mysqlpp::quote<<pattern<<",0,"<<mysqlpp::quote<<getCurrentTime()<<","<<mysqlpp::quote<<max_size<<",1)";

            QueryRunner(SUBSCRIBE_DB, CDbWServer).execute(sql);


            IceUtil::RWRecMutex::WLock lock(_validMutex);

            boost::shared_ptr<sub_type> data(new sub_type);
            
            data->type = type;
            data->key_words = pattern;
            data->real_time_num = 0;
            data->subscribe_time = getCurrentTime();
            data->max_state_num = max_size;
            data->subscribe_control = 1;
            _type_subs[type] = data; 
            
            MessageQueueThread * thread = new MessageQueueThread(pattern, max_size, true);  
            thread->start(); 
            _queue_thread[type] = thread;
            return true; 
          }  else  {
            return false; 
            MCE_WARN("check wrong!");
          }
          return false;
        } 

       bool HotStateSubscribeManagerI::unSubscribe(const string& type, const Ice::Current& ic)  {
         IceUtil::RWRecMutex::WLock lock(_validMutex);
         map<string, MessageQueueThread *>::iterator iter = _queue_thread.find(type);
         if(iter != _queue_thread.end())  {
           //destroy the thread 
           iter->second->deleteConfig2Db(iter->first);
           sleep(2);//delete the record from db before thread destoried 
           iter->second->setSubscribeControl(0);
           _queue_thread.erase(iter);
         }
         map<string, boost::shared_ptr<sub_type> >::iterator it = _type_subs.find(type);
         if(it != _type_subs.end())  {
           _type_subs.erase(it);
         }
         return true; 
       }

       bool HotStateSubscribeManagerI::test(const string& pattern, const string& inputState, const Ice::Current& ic)  {
         boost::regex oRegex(pattern.c_str());
         boost::smatch oResults;
         if(boost::regex_search(inputState, oResults, oRegex))  {
           MCE_INFO("[HotStateSubscribeManagerI::test] pattern : "<<pattern<<" 命中state: "<<inputState);
           return true;
         } else  {
           return false;
         }
       } 


       vector<map<string, string> > HotStateSubscribeManagerI::getNewState(const string& type, int limit, const Ice::Current& ic)  {
         vector<map<string, string> > results;
         IceUtil::RWRecMutex::WLock lock(_validMutex);
         map<string, MessageQueueThread *>::iterator iter = _queue_thread.find(type);
         if(iter != _queue_thread.end())  {
           IndexUpdateDataSeq  info = (iter->second)->getNewState(limit); 
           for(IndexUpdateDataSeq::iterator iter = info.begin(); iter != info.end(); iter++)  {
             map<string, string> unit;
             for(map<string, string>::iterator it = iter->index.begin(); it != iter->index.end(); it++)  {
               unit[it->first] = it->second;
             }

             results.push_back(unit);
           }
         }
         return results; 
       }


       
       /* not implementation */
       vector<map<string, string> > HotStateSubscribeManagerI::showTypeInfos(const Ice::Current& ic)  {

          vector<map<string, string> > tmp;
          return tmp;

       }

       map<string, string> HotStateSubscribeManagerI::showTypeInfo(const string& type, const Ice::Current& ic)  {
         map<string, string> typeinfo; 
         {
           IceUtil::RWRecMutex::WLock lock(_validMutex);
           map<string, boost::shared_ptr<sub_type> >::iterator iter = _type_subs.find(type);
           map<string, MessageQueueThread *>::iterator iter_thread = _queue_thread.find(type);
           if(iter != _type_subs.end() && iter_thread != _queue_thread.end())  {
            // typeinfo.insert(pair<string, string>(KEY_WORDS, iter->second->key_words));
             typeinfo[KEY_WORDS] = iter->second->key_words;
             char real_time_num[10];
             itoa(iter->second->real_time_num, real_time_num);
             typeinfo[REALTIME_NUM] = (string)real_time_num;  //iter_thread->second->getQueueCurrentSize();
             typeinfo[MAX_STATE_NUM] = iter_thread->second->getQueueMaxSize();
             typeinfo[SUBSCRIBE_TIME] = iter_thread->second->getTimeStamp();
             typeinfo[SUBSCRIBE_CONTROL] = iter_thread->second->getSubscribeControl();
           }  else  {
             MCE_WARN("Data in _type_subs adn _queue_thread is not the same!");
           }
         }
         return typeinfo;  
       }



     
      }
    }
  }
}

