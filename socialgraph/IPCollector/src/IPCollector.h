#ifndef  _BASICREADER_H_
#define  _BASICREADER_H_
#include "QueryRunner.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <map>
#include <vector>
#include <string.h>
#include "stdlib.h"
#include <iconv.h>
#include <errno.h>
#include <IceUtil/IceUtil.h>
#include <ext/hash_map>

class IPCollector
{
public:
	IPCollector(const std::string& date);
	virtual ~IPCollector();
	void collectForFBI();
	void collectForIR();
private:
	std::string _tableName;
	std::map<int, std::set<long> > _ipdata_byid;
};

class IPDataByIDHandler : public com::xiaonei::xce::ResultHandler
{
public:
	IPDataByIDHandler(int& last_id, std::map<int, std::set<long> >& results):
		_last_id(last_id), _results(results){
	}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	int& _last_id;
	std::map<int, std::set<long> >& _results;
};

#endif
