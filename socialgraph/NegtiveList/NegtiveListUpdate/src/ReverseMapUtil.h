#ifndef _REVERSE_MAP_UTIL_H_
#define _REVERSE_MAP_UTIL_H_

#include <vector>
#include <map>

namespace reversemap {

class ReverseMapUtil {
public:
	static void ReverseMap(const std::map<int, std::vector<int> >& resource, std::map<int, std::vector<int> >& destination);
};

}

#endif
