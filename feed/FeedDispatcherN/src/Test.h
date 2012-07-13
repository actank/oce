/*
 * Test.h
 *
 *  Created on: Dec 27, 2010
 *      Author: yejingwei
 */

#ifndef TEST_H_
#define TEST_H_
#include "ExpressionParser.h"
#include "IceLogger.h"
namespace xce {
namespace feed {
using namespace MyUtil;

class PrintHelper{
public:
  template<typename T>
  static string Print(const T & container) { //将容器内容打印成字符串
    ostringstream ios;
    ios << "(";
    bool first_item = true;
    int i = 0;
    for (typename T::const_iterator itr = container.begin(); itr
        != container.end(); ++itr,++i) {
      if (first_item == true) {
        first_item = false;
      } else {
        ios << ",";
      }
        ios << *itr;
    }
    ios << ")";
    return ios.str();
  }
};

class TestFactor: public XiaoZuFactor{
public:
  TestFactor(char c = 'z', int groupid = 1 ):XiaoZuFactor(c,groupid){

  }
};
class TestRunner{
public:
  void Run(){
    MCE_INFO("Run begin");
    TestFactor factor('z',100006);
    int tmp[] = {3};
//    set<int> ids(tmp[0],tmp + sizeof(tmp) / sizeof(tmp[0]));
    set<int> ids;
    ids.insert(1);
    ids.insert(328822503);
    ids.insert(3);
    factor.Minus(ids);
    MCE_INFO("result:" << PrintHelper::Print(ids) );
    MCE_INFO("Run end");
  }
};
}
}

#endif /* TEST_H_ */
