#ifndef Dict_h
#define Dict_h
#include "darts_tmp.h"
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
       struct UnigramRecord  {
          std::string key;
          int  count;
       };
       typedef struct NumFq  {
           int NumCode;
           int Freq;
       }  CodeFreq;
       std::string file;
       Darts::DoubleArray m_da;
       Dict(std::string filename,std::string indexname="./data/DoubleArray.lib")  { file=filename; index=indexname; };
       ~Dict() { };  
       bool static Cmp(const UnigramRecord &p1,  const UnigramRecord &p2)  {
             char  i = 0;
             while(1)  {
                unsigned char pu1 = p1.key[i];
                unsigned char pu2 = p2.key[i];
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
       std::map<std::string,CodeFreq *> CharaFreq;
    private:
       std::string index;//默认原始词典输出二进制存储文件
} ; 
#endif    
