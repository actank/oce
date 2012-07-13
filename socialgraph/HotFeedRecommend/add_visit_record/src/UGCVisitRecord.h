#ifndef UGC_VISIT_RECORD_H_
#define UGC_VISIT_RECORD_H_

#include "ObjectCacheI.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <set>

class UGCVisitRecord : virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
	void LoadData(const std::string& fileName);	
	bool IsVisit(int userId, long ugcId);
	std::set<long> GetVisitRecord(int userid);
	void PrintMap();

private:
	IceUtil::RWRecMutex init_flag_mutex_;
	std::map< int, std::set<long> > record_;
};

#endif


