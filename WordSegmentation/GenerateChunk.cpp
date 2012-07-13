#include<string>
#include<vector>
#include<map>
#include<queue>
#include<math.h>
#include<iostream>
#include "GenerateChunk.h"
#include "Dict.h"
using namespace std;



GenerateChunk::GenerateChunk(Dict *temp )  {
}

GenerateChunk::~GenerateChunk()  {  
}

int GenerateChunk::CheckDict(std::string substrings, Dict * dictionary)  {   //查词典找出字符串substrings中所有可能前缀序列
   if(substrings=="") return 0;
   Darts::DoubleArray::result_pair_type  result_pair[1024*6];
   Darts::DoubleArray::key_type        key[1024*6];
   std::strcpy(key,substrings.c_str());
   size_t num = dictionary->m_da.commonPrefixSearch(key, result_pair, sizeof(result_pair));
   if(num>0)
      return result_pair[num-1].length;
   else  return substrings.length();
}
