#ifndef Dict_h
#define Dict_h
#include "darts.h"
#include <cstdio>
#include <stdlib.h>
#include<vector>
#include<map>
#include<string>
#include<iostream>
#include<fstream>
#include<algorithm>

class Dict  {
    public:
       std::string file;
       std::string stop_words_file;
       std::string person_name_file;
       Darts::DoubleArray m_da;
       Dict(std::string filename,std::string stop_file,std::string person_file,std::string indexname="./data/DoubleArray.lib")  { 
         file=filename; stop_words_file=stop_file; person_name_file=person_file; index=indexname;
       };
       ~Dict() { };  
       bool static Cmp(const char *p1,  const char *p2)  {
             char  i = 0;
             while(1)  {
                unsigned char pu1 = *(p1+i);
                unsigned char pu2 = *(p2+i);
                if (pu1 == pu2)   {
                      if(pu1 == 0) 
                          break;
                      i++;
                      }
                      else  {
                          return pu1<pu2;
                      }
                   }
              return true;
            };
       int Construct();
       void GenerateSequencedDict();    
       std::string GetIndex();
       std::map<std::string,int> StopWordsList;
       std::map<std::string,int> PersonNameList;
    private:
       std::string index;//默认原始词典输出二进制存储文件
} ; 
#endif    
