#ifndef  _BASICREADER_H_
#define  _BASICREADER_H_
#include "QueryRunner.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <string.h>
#include "stdlib.h"
#include <iconv.h>
#include <errno.h>
#include <IceUtil/IceUtil.h>
#include <ext/hash_map>

struct registerNode{
	int userId;
	std::string timeStr;
	int timeInt;
};

class FriendFinderNewComerCollector
{
public:
	FriendFinderNewComerCollector();
	virtual ~FriendFinderNewComerCollector();
	void deployByUser(int userId);
	void deployByDays(int inDays);
};

class NewComerCollectorHandler : public com::xiaonei::xce::ResultHandler
{
public:
	NewComerCollectorHandler(int deadline, int& minTimeInt, std::string& minTimeStr, std::vector<registerNode>& results):
		_deadline(deadline), _minTimeInt(minTimeInt), _minTimeStr(minTimeStr), _results(results){
	}
	virtual bool handle(mysqlpp::Row& row) const;
private:
        int _deadline;
        int& _minTimeInt;
        std::string& _minTimeStr;
	std::vector<registerNode>& _results;
};

#endif
