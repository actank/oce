#ifndef _GROUP_H_
#define _GROUP_H_

#include <string>
#include <vector>

struct Group {
	std::string name;
	std::vector<int> ids;
};

typedef std::vector<Group> TotalGroup;

#endif
