#include<string>
#include<vector>
#include<map>
#include<queue>
#include<math.h>
#include<iostream>
#include "GenerateChunk.h"
#include "Dict.h"
#define MAX "max"
#define MIX "mix"
#define MIN "min"
#define MIN_MAX "min_max"
using namespace std;



GenerateChunk::GenerateChunk(Dict *temp )  {
}

GenerateChunk::~GenerateChunk()  {  
}

/*查词典找出字符串substrings中所有可能前缀序列*/
vector<int> & GenerateChunk::CheckDict(std::string substrings, Dict * dictionary, std::string type)  { 
   if(substrings=="") {SegmentIndexList.clear();return SegmentIndexList;}
   int len=substrings.length();
   Darts::DoubleArray::result_pair_type result_pair[len+1];
   Darts::DoubleArray::key_type       key[len+1];
   std::strcpy(key,substrings.c_str());
   size_t num = dictionary->m_da.commonPrefixSearch(key, result_pair, sizeof(result_pair));
   SegmentIndexList.clear();
   if(type == MAX)  {
      if(num>0)  { 
         SegmentIndexList.push_back(result_pair[num-1].length);
         return SegmentIndexList;
      }
      else  {
         if((substrings[0]&0xFC) == 0xFC)  {
             SegmentIndexList.push_back(6);  
             return SegmentIndexList;
         }
         else if((substrings[0]&0xF8) == 0xF8)  {
             SegmentIndexList.push_back(5);
             return SegmentIndexList;
         }
         else if((substrings[0]&0xF0) == 0xF0)  {
             SegmentIndexList.push_back(4);
             return SegmentIndexList;
         }
         else if((substrings[0]&0xE0) == 0xE0)  {
             if(dictionary->PersonNameList.find(substrings.substr(0,3))!=dictionary->PersonNameList.end())  {
                  if(substrings.length()==3)  SegmentIndexList.push_back(3);
                  else if(substrings.length()==4)  SegmentIndexList.push_back(4);
                  else if(substrings.length()==5)  SegmentIndexList.push_back(5);
                  else if(substrings.length()==6)  SegmentIndexList.push_back(6);
                  else if(substrings.length()==7)  SegmentIndexList.push_back(7);
                  else if(substrings.length()==8)  SegmentIndexList.push_back(8);
                  else if(substrings.length()==9)  SegmentIndexList.push_back(9);
                  else if(substrings.length()>9)  {
                      if((substrings[3]&0xE0) == 0xE0)  {
                         std::strcpy(key,substrings.substr(6,substrings.length()-6).c_str());
                         num = dictionary->m_da.commonPrefixSearch(key, result_pair, sizeof(result_pair));
                         if(num>0)  SegmentIndexList.push_back(6);
                         else  SegmentIndexList.push_back(9);
                      }
                      else   SegmentIndexList.push_back(3);
                  }
             }
             else  SegmentIndexList.push_back(3);
             return SegmentIndexList;
         }
         else if((substrings[0]&0xC0) == 0xC0)  {
             SegmentIndexList.push_back(2);
             return SegmentIndexList;
         }
         else  {
             SegmentIndexList.push_back(substrings.length());
             return SegmentIndexList;
         }
      }
   }
   else if(type == MIX)  {
      if(num>0)  {
         if(num==1)
           SegmentIndexList.push_back(result_pair[0].length);
         else  {
           SegmentIndexList.push_back(result_pair[0].length);
           SegmentIndexList.push_back(result_pair[num-1].length);
         }
         return SegmentIndexList;
      }
      else  {
         if((substrings[0]&0xFC) == 0xFC)  {
             SegmentIndexList.push_back(6);
             return SegmentIndexList;
         }
         else if((substrings[0]&0xF8) == 0xF8)  {
             SegmentIndexList.push_back(5);
             return SegmentIndexList;
         }
         else if((substrings[0]&0xF0) == 0xF0)  {
             SegmentIndexList.push_back(4);
             return SegmentIndexList;
         }
         else if((substrings[0]&0xE0) == 0xE0)  {
             if(dictionary->PersonNameList.find(substrings.substr(0,3))!=dictionary->PersonNameList.end())  {
                  if(substrings.length()==3)  SegmentIndexList.push_back(3);
                  else if(substrings.length()==4)  SegmentIndexList.push_back(4);
                  else if(substrings.length()==5)  SegmentIndexList.push_back(5);
                  else if(substrings.length()==6)  SegmentIndexList.push_back(6);
                  else if(substrings.length()==7)  SegmentIndexList.push_back(7);
                  else if(substrings.length()==8)  SegmentIndexList.push_back(8);
                  else if(substrings.length()==9)  SegmentIndexList.push_back(9);
                  else if(substrings.length()>9)  {  
                      if((substrings[3]&0xE0)== 0xE0)  {
                        std::strcpy(key,substrings.substr(6,substrings.length()-6).c_str());
                        num = dictionary->m_da.commonPrefixSearch(key, result_pair, sizeof(result_pair));
                        if(num>0)  SegmentIndexList.push_back(6);
                        else  SegmentIndexList.push_back(9);
                      }
                      else  SegmentIndexList.push_back(3);
                  }
             }
             else  SegmentIndexList.push_back(3);
             return SegmentIndexList;
         }
         else if((substrings[0]&0xC0) == 0xC0)  {
             SegmentIndexList.push_back(2);
             return SegmentIndexList;
         }
         else  {
             SegmentIndexList.push_back(substrings.length());
             return SegmentIndexList;
         }
      }      
   }
   else if(type == MIN)  {
      if(num>0)  {
         SegmentIndexList.push_back(result_pair[0].length);
         return SegmentIndexList;
      }
      else  {
         if((substrings[0]&0xFC) == 0xFC)  {
             SegmentIndexList.push_back(6);
             return SegmentIndexList;
         }
         else if((substrings[0]&0xF8) == 0xF8)  {
             SegmentIndexList.push_back(5);
             return SegmentIndexList;
         }
         else if((substrings[0]&0xF0) == 0xF0)  {
             SegmentIndexList.push_back(4);
             return SegmentIndexList;
         }
         else if((substrings[0]&0xE0) == 0xE0)  {
             if(dictionary->PersonNameList.find(substrings.substr(0,3))!=dictionary->PersonNameList.end())  {
                  if(substrings.length()==3)  SegmentIndexList.push_back(3);
                  else if(substrings.length()==4)  SegmentIndexList.push_back(4);
                  else if(substrings.length()==5)  SegmentIndexList.push_back(5);
                  else if(substrings.length()==6)  SegmentIndexList.push_back(6);
                  else if(substrings.length()==7)  SegmentIndexList.push_back(7);
                  else if(substrings.length()==8)  SegmentIndexList.push_back(8);
                  else if(substrings.length()==9)  SegmentIndexList.push_back(9);
                  else if(substrings.length()>9)  {
                      if((substrings[3]&0xE0) == 0xE0)  {
                        std::strcpy(key,substrings.substr(6,substrings.length()-6).c_str());
                        num = dictionary->m_da.commonPrefixSearch(key, result_pair, sizeof(result_pair));
                        if(num>0)  SegmentIndexList.push_back(6);
                        else  SegmentIndexList.push_back(9);
                     }
                     else SegmentIndexList.push_back(3);
                  }
             }
             else  SegmentIndexList.push_back(3);
             return SegmentIndexList;
         }
         else if((substrings[0]&0xC0) == 0xC0)  {
             SegmentIndexList.push_back(2);
             return SegmentIndexList;
         }
         else  {
             SegmentIndexList.push_back(substrings.length());
             return SegmentIndexList;
         }
      }
   }
   else if(type == MIN_MAX)  return SegmentIndexList;
}
