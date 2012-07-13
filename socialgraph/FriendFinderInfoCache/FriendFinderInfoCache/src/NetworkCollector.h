#ifndef _NETWORKCOLLECTOR_H_
#define _NETWORKCOLLECTOR_H_
#include "QueryRunner.h"
#include <ObjectCacheI.h>

namespace xce {
namespace friendfinder {

class NetworkCollector {
public:
	NetworkCollector();
	virtual ~NetworkCollector();
	void getUnivInfo(int userId, vector< vector<string> >& univInfo);
	void getHighschoolInfo(int userId, vector< vector<string> >& highschoolInfo);
	void getJuniorschoolInfo(int userId, vector< vector<string> >& juniorschoolInfo);
	void getElementaryschoolInfo(int userId, vector< vector<string> >& elementaryschoolInfo);
	void getWorkplaceInfo(int userId, vector< vector<string> >& workplaceInfo);
	void getUserAge(int userId, int& age);
	void getUserAgeBatch(const MyUtil::IntSeq& userIds, MyUtil::Int2IntMap& userAgeMap);
};

//***********************************************************************************************

class UnivReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	UnivReadHandler(std::vector< std::vector<string> >& univInfo): _univInfo(univInfo) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector< std::vector<string> >& _univInfo;
};

//***********************************************************************************************

class HighschoolReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	HighschoolReadHandler(std::vector< std::vector<string> >& highschoolInfo): _highschoolInfo(highschoolInfo) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector< std::vector<string> >& _highschoolInfo;
};

//***********************************************************************************************

class JuniorschoolReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	JuniorschoolReadHandler(std::vector< std::vector<string> >& juniorschoolInfo): _juniorschoolInfo(juniorschoolInfo) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector< std::vector<string> >& _juniorschoolInfo;
};

//***********************************************************************************************

class ElementaryschoolReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	ElementaryschoolReadHandler(std::vector< std::vector<string> >& elementaryschoolInfo): _elementaryschoolInfo(elementaryschoolInfo) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector< std::vector<string> >& _elementaryschoolInfo;
};

//***********************************************************************************************

class WorkplaceReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	WorkplaceReadHandler(std::vector< std::vector<string> >& workplaceInfo): _workplaceInfo(workplaceInfo) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector< std::vector<string> >& _workplaceInfo;
};
	
//***********************************************************************************************

class UserAgeReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	UserAgeReadHandler(int& age): _age(age) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	int& _age;
};

//***********************************************************************************************

class UserAgeBatchReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	UserAgeBatchReadHandler(MyUtil::Int2IntMap& userAgeMap): _userAgeMap(userAgeMap) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	MyUtil::Int2IntMap& _userAgeMap;
};

}
}

#endif

