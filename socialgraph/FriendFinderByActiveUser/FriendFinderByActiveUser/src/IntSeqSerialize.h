#ifndef __INTSEQ_SERIALIZE_H__
#define __INTSEQ_SERIALIZE_H__

#include <iostream>
#include <string.h>

using namespace std;

namespace MyUtil {


class IntSeqSerialize {
public:
        static std::string serialize(MyUtil::IntSeq seq){
	        string result;
	        result.resize(seq.size() * sizeof(int));
        	memcpy((void*)(result.data()), &seq[0], seq.size()*sizeof(int));
	        return result;
	}
 
      static MyUtil::IntSeq unserialize(std::string str){
		MyUtil::IntSeq result;
	        int strLen = str.length();
        	int resultLen = (strLen <= 0)?(0):((strLen-1)/sizeof(int)+1);
	        result.resize(resultLen);

	        memcpy((void*)(&result[0]), str.data(), str.length());
        	return result;	
	}
	
	static MyUtil::IntSeq unserialize(const mysqlpp::Row::value_type & str){
                MyUtil::IntSeq result;
                int strLen = str.length();
                int resultLen = (strLen <= 0)?(0):((strLen-1)/sizeof(int)+1);
                result.resize(resultLen);

                memcpy((void*)(&result[0]), str.data(), str.length());
                return result;
        }


};

};

#endif

