/*
 * Common.h
 *
 *  Created on: Feb 23, 2011
 *      Author: yejingwei
 */

#ifndef COMMON_H_
#define COMMON_H_
#include <iostream>
#include <sstream>
#include <string>
#include <map>
using namespace std;

namespace xce {
namespace feed {

template<typename T>
static string Print(const T & container) { //将容器内容打印成字符串
  ostringstream ios;
  ios << "(";
  bool first_item = true;
  int i = 0;
  for (typename T::const_iterator itr = container.begin(); itr
      != container.end(); ++itr, ++i) {
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

static string Print(const map<Ice::Long, int>& container) {
  ostringstream ios;
  ios << "(";
  bool first_item = true;
  int i = 0;
  for (map<Ice::Long, int>::const_iterator itr = container.begin(); itr
      != container.end(); ++itr, ++i) {
    if (first_item == true) {
      first_item = false;
    } else {
      ios << ",";
    }
    ios << itr->first << "/" << itr->second ;
  }
  ios << ")";
  return ios.str();
}

}
}
#endif /* COMMON_H_ */
