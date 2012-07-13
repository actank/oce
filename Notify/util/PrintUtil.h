/*
 * MergeConfig.h
 *
 *  Created on: Nov 25, 2010
 *      Author: yejingwei
 */

#ifndef _XCE_NOTIFY_PRINT_UTIL_H_
#define _XCE_NOTIFY_PRINT_UTIL_H_

#include <set>
using namespace std;
namespace xce {
namespace notify {



class PrintHelper{
public:
  template<typename T>
  static string Print(const T & container,int limit = 1000) { //将容器内容打印成字符串
    ostringstream ios;
    ios << "(";
    bool first_item = true;
    int i = 0;
    int cnt = 0;
    for (typename T::const_iterator itr = container.begin(); itr
        != container.end(); ++itr,++i) {
      if (first_item == true) {
        first_item = false;
      } else {
        ios << ",";
      }
      ios << *itr;
      if(cnt >= limit){
        break;
      }
    }
    ios << ")";
    return ios.str();
  }
};
}
}

#endif /* MERGECONFIG_H_ */
