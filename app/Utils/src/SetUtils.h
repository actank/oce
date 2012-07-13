/*
 * =====================================================================================
 *
 *       Filename:  SetUtils.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/08/2011 05:08:36 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#ifndef __SETUTILS_H__
#define __SETUTILS_H__

#include <vector>

namespace xce {
namespace utils {
  
  using namespace std;

  class SetUtils {
    public:
      template<typename T>
      static vector<T> getElementsInANotB(const vector<T>& setA, const vector<T>& setB);
  };

  template<typename T>
  vector<T> SetUtils::getElementsInANotB(const vector<T>& setA, const vector<T>& setB) {
    vector<T> result;
    vector<T> sortedB(setB);
    sort(sortedB.begin(), sortedB.end());
    for (int i = 0, size = setA.size(); i < size; ++i) {
      if (!binary_search(sortedB.begin(), sortedB.end(), setA[i])) {
        result.push_back(setA[i]);
      }
    }
    return result;
  }
}
}

#endif
