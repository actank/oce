#ifndef  _BASICREADER_H_
#define  _BASICREADER_H_
#include "QueryRunner.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <map>
#include <vector>
#include <string.h>
#include "stdlib.h"
#include <iconv.h>
#include <errno.h>
#include <IceUtil/IceUtil.h>
#include <ext/hash_map>

struct UnivData
{
	int userid;
	std::string university_name;
	std::string department;
	int enroll_year;

	bool has_friends;
	std::string friend_list;
};

class UnivLinkFriends
{
public:
	UnivLinkFriends(string);
	virtual ~UnivLinkFriends();
	void collectUnivData();
private:
	std::map<int, std::map<int,UnivData> > linkFriends(std::map<int, std::map<int,UnivData> >& );
	void writeFile(std::map<int, std::map<int,UnivData> >& );
private:
	bool _succ;
	std::vector<std::ofstream*> _ofss;
};

class UnivDataByIDHandler : public com::xiaonei::xce::ResultHandler
{
public:
	UnivDataByIDHandler(int& last_id, std::map<int, std::map<int,UnivData> >& results):
		_last_id(last_id), _results(results){
	}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	int& _last_id;
	std::map<int, std::map<int,UnivData> >& _results;
};

class FriendsHandler : public com::xiaonei::xce::ResultHandler
{
public:
	FriendsHandler(std::map<int,std::string>& results):
		_results(results){
	}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	std::map<int,std::string>& _results;
};

#endif
