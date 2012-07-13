#ifndef _XCE_ALGOSTUFF_H__
#define _XCE_ALGOSTUFF_H__
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <algorithm>

namespace xce {

// INSERT_ELEMENT (collection, first, last)
// fill values from first to last into the collection
// NOTE: NO half-open range
template <typename T>
inline void INSERT_ELEMENT(T& coll, int first, int last) {
	for (int i = first; i <= last; ++i) {
		coll.insert(coll.end(), i);
	}
}

template <typename T>
inline void PRINT_ELEMENTS(const T& coll, const char* optcstr = "") {
	typename T::const_iterator pos;
	std::cout << optcstr;
	for (pos = coll.begin(); pos != coll.end(); ++pos) {
		std::cout << *pos << " ";
	}
	std::cout << std::endl;
}


template <typename T, typename K>
inline std::string Comb(const T& t, const K& k) {
  std::ostringstream oss;
  oss << t << k;
  return oss.str();  
}

template <typename T, typename K>
inline std::string Comb2(const T& t, const K& k) {
  std::ostringstream oss;
  oss << t << "_" << k;
  return oss.str();  
}

template <typename T, typename K>
inline void PRINT_KEY2LIST(const std::map<T, K>& resource) {
  for (typename std::map<T, K>::const_iterator iter = resource.begin();
      iter != resource.end(); ++iter) {
    std::cout << iter->first << ":" << std::endl;
    for (typename K::const_iterator it = iter->second.begin();
        it != iter->second.end(); ++it) {
      std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
  }
}

template <typename T, typename K>
inline void PRINT_KEY2VALUE(const std::map<T, K>& resource) {
  for (typename std::map<T, K>::const_iterator iter = resource.begin();
      iter != resource.end(); ++iter) {
    std::cout << iter->first << " : " << iter->second << std::endl;
  }
}

// intersection of two vector<int>
int Intersection(const std::vector<int>& t1, const std::vector<int>& t2,	std::vector<int>& result);

int GetTextFileLines(const std::string& path, std::vector<std::string>& lines);

}

#endif

