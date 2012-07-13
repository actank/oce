#ifndef USERINTERESTTAG_H_
#define USERINTERESTTAG_H_

#include "ObjectCacheI.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <set>

class UserInterestTag {
public:
	void LoadData(const std::string& fileName);	
	bool IsInterested(int userId, const std::string& interest);
	void PrintMap();
	void GetInterestMap();
	std::set<std::string> GetUserInterest(int userid);

private:
	IceUtil::RWRecMutex init_flag_mutex_;
	typedef std::map< int, std::set<std::string> > TagMapType;
	TagMapType userTagMap_;
};

#endif
