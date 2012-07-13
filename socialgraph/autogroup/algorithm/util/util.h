#ifndef _UTIL_H_
#define _UTIL_H_

#include <iostream>
#include <map>
#include <string>
#include <sstream>

namespace xce {

template <typename T>
inline void PRINT_LIST(const T& col) {
	for (typename T::const_iterator iter = col.begin();
			iter != col.end(); ++iter) {
		std::cout << *iter << " ";
	}
	std::cout << std::endl;
	std::cout << std::endl;
}

template <typename T>
inline void PRINT_ELEMENT(const T& item) {
	std::cout << item << std::endl;
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

template <typename T, typename K>
inline std::string GET_KEY(const T& pre, const K& val) {
	std::ostringstream oss;
	oss << pre << "_" << val;
	return oss.str();
}

}

#endif
