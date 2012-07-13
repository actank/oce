/*
 * =====================================================================================
 *
 *       Filename:  IntegerSeqSerialize.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/03/2011 05:28:00 PM
 *       Revision:  none
 *       Compiler:  g++ * *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#ifndef __INTEGER_SEQ_SERIALIZE_H__
#define __INTEGER_SEQ_SERIALIZE_H__
#include <iostream>
#include <string.h>
#include <vector>

namespace MyUtil {

  using namespace std;

  template<typename T>
  class IntegerSeqSerialize {
    public:
      static string serialize(const vector<T>& nums) {
        string result;
        result.resize(nums.size() * sizeof(T));
        memcpy((void *)(result.data()), &nums[0], nums.size() * sizeof(T));
        return result;
      }

      //c++中一个char占1个Byte，一个int占4个Byte，也就是str的长度必须是4的倍数才可以恰好转换过去，否则最后一个
      //数字可能有误
      static vector<T> unserialize(const string& str) {
        vector<T> result;
        int strLen = str.length();
        int resultLen = strLen / sizeof(T) + strLen % sizeof(T);
        result.resize(resultLen);
        memcpy((void *)(&result[0]), str.data(), strLen);
        return result;
      }

      static vector<T> unserialize(const mysqlpp::Row::value_type& str) {
        vector<T> result;
        int strLen = str.length();
        int resultLen = strLen / sizeof(T) + strLen % sizeof(T);
        result.resize(resultLen);
        memcpy((void *)(&result[0]), str.data(), strLen);
        return result;
      }
  };
}

#endif
