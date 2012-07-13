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

#include "BlogSearchRebuild.h"
using namespace com::xiaonei::xce;
    LoadUrlThread::LoadUrlThread(int db_no, std::ofstream & url_set) : _db_no(db_no), _url_set(url_set) {
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
          table_prefix = "blog_";
          itoa(i, number);
          table_prefix += (std::string) number;
          tables.push_back(table_prefix);
        }
      } else  {
        char num_suffix[10];
        itoa(_db_no, number);
        for(int i = 0; i < 10; i++)  {
          table_prefix = "blog_";
          table_prefix += (std::string) number;
          itoa(i, num_suffix);
          table_prefix += (std::string) num_suffix;
          tables.push_back(table_prefix);
        }  
      }
      for(int i = 0; i < tables.size(); i++)  {
        std::cout<<tables.at(i)<<std::endl;
      }
      std::string db = "adminsearch";
      int count = 0;
      for(std::vector<std::string>::iterator iter = tables.begin(); iter != tables.end(); iter++)  {
         //std::string beginUrl = "";
         //std::string maxUrl = beginUrl;
         long beginUrl = 0;
         long maxUrl = 0;
          
        while(beginUrl == 0 || maxUrl < beginUrl)  {
          maxUrl = beginUrl;
          Statement sql;
          std::string num = (*iter).substr(6, (*iter).length() -6);
          int i = atoi(num.c_str());
          //std::cout<<i<<std::endl;
          sql << "SELECT id, owner, body, title, time, uptime, view_count, blogControl FROM blog_" << i << " WHERE id >= \""<< beginUrl << "\" ORDER BY id LIMIT "<< BATCH_URL_SIZE;
          //sql << "SELECT id, url_md5, title FROM share_" << i << " WHERE id >= \""<< beginUrl << "\" ORDER BY id LIMIT "<< BATCH_URL_SIZE;
          BatchUrlMd5ResultHandlerI handler(beginUrl, _url_set);
          QueryRunner(db, CDbRServer, *iter).query(sql, handler);
          std::cout<<" cycle :"<<count<<std::endl;
         count++; 
         if(count >= 100)  {
           break;
         }
        }
      }
      std::cout<<"Fill task done!"<<std::endl;
    }
    
  
     


    BatchUrlMd5ResultHandlerI::BatchUrlMd5ResultHandlerI(long& maxUrl, 
        std::ofstream& output) :_maxUrl(maxUrl), _output(output)  {

    }

    bool BatchUrlMd5ResultHandlerI::handle(mysqlpp::Row& row) const  {   
      long blog_id = (long) row["id"];
      int owner_id = (int) row["owner"];
      std::string content = (std::string) row["body"];
      std::string title = (std::string) row["title"];
      std::string time = (std::string) row["time"];
      std::string uptime = (std::string) row["uptime"];
      int view_count = (int) row["view_count"];
      int blogControl = (int) row["blogControl"];
      bool sign1 = true, sign2 = true;
      //std::cout<<blog_id<<std::endl;
      //std::cout<<content<<std::endl;
      //std::cout<<title<<std::endl;
      //replace1(content);
      //replace2(content);
      //replace3(content);
      
      
      //sign1 = filter(content);
      //sign2 = filter(title);
      //if(!sign1 || !sign2)  {
       // std::cout<<"bad "<<std::endl;    
       // return true;
     // }
      _output<<"<Blog>"<<std::endl;
      _output<<"<id>"<<std::endl;
      _output<<blog_id<<std::endl;
      _output<<"</id>"<<std::endl;
      _output<<"<owner>"<<std::endl;
      _output<<owner_id<<std::endl;
      _output<<"</owner>"<<std::endl;
       _output<<"<body>"<<std::endl;
           _output<<content<<std::endl;
      _output<<"</body>"<<std::endl;
       _output<<"<title>"<<std::endl;
      _output<<title<<std::endl;
      _output<<"</title>"<<std::endl;
_output<<"<time>"<<std::endl;
      _output<<time<<std::endl;
      _output<<"</time>"<<std::endl;
      _output<<"<uptime>"<<std::endl;
      _output<<uptime<<std::endl;
      _output<<"</uptime>"<<std::endl;
      _output<<"<view_count>"<<std::endl;
      _output<<view_count<<std::endl;
      _output<<"</view_count>"<<std::endl;
      _output<<"<blogControl>"<<std::endl;
      _output<<blogControl<<std::endl;
      _output<<"</blogControl>"<<std::endl;
      
      _output<<"</Blog>"<<std::endl;
      
      //long id = (long) row["id"];
      //std::string key = urlmd5;
      //std::string title = (std::string) row["title"];
    /*   key += title;
      if(urlmd5 != "")  {
        MD5 md5_engine;
        md5_engine.reset();
        md5_engine.update(key);
        key = md5_engine.toString();
        _allUrlSet.insert(key);
      }*/
      
      _maxUrl = _maxUrl > blog_id ? _maxUrl:blog_id;
      //_maxUrl = _maxUrl > id ? _maxUrl:id;
      return true;
    }
   
   int main()  {
      std::string file("/data/xce/BlogSearch/Data/data_final.xml");
      std::ofstream output;
      output.open(file.c_str(), ios::app);       
      LoadUrlsVec _vecLoadUrls;
      com::xiaonei::xce::ConnectionPoolManager::instance().initialize();
      for(int i = 0; i < THREAD_NUM; i++)  {
        output<<"<xml>"<<std::endl;
        _vecLoadUrls.push_back(new LoadUrlThread(i, output));  
      } 

      for(int i = 0; i < THREAD_NUM; i++)  {
        _vecLoadUrls.at(i)->start();
      }      

      //output<<"</xml>"<<std::endl;
     sleep(3000000);
      output<<"</xml>"<<std::endl;
      std::cout<<"FINISH BOOT"<<std::endl;
      return 0;
    }




