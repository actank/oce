/*
 * =====================================================================================
 *
 *       Filename:  UserActive.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年12月08日 17时17分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef _USER_ACTIVE_H__
#define _USER_ACTIVE_H__

#include <string>
#include <map>
#include <vector>
#include <bitset>
#include <sstream>
#include <fstream>
#include <IceUtil/IceUtil.h>

#include "QueryRunner.h"

namespace xce {
namespace useractive {

const int LOGIN_SCOPE = 7;
const int UGC_SCOPE = 15;

const int BLOG_WEIGHT = 12;
const int DOING_WEIGHT = 4;
const int GOSSIP_WEIGHT = 1;
const int PHOTO_WEIGHT = 6;
const int SHARE_WEIGHT = 2;

const string LOG_ACTIVE_ON = "setActiveOn";
const string LOG_ACTIVE_OFF = "setActiveOff";

struct Entry {
	int grade;
	deque<int> ids;

	Entry(int grade) : grade(grade) {}
};

struct greater_entry : public std::binary_function<Entry, Entry, bool> {
	bool operator()(const Entry& x, const Entry& y) const {
		return x.grade > y.grade;
	}
};

class UserActive : virtual public IceUtil::Shared {
public:
	void setActive(std::map<int, int>& grade, float percent);
private:
	void getCurrentMap(std::map<int,int>& grade, float percent, std::map<int, bool>& currentActive);
	void fileToMap(std::map<int, bool>& lastActive);
	void mapToFile(map<int, bool>& active);

	string getFilePath();
};

typedef IceUtil::Handle<UserActive> UserActivePtr;
//********************************************************************************************
class Analyzer : virtual public IceUtil::Shared {
public:
	virtual void analyze(std::map<int, int>& grade, time_t now) = 0;
};
typedef IceUtil::Handle<Analyzer> AnalyzerPtr;

class LoginAnalyzer : virtual public Analyzer {
public:
	LoginAnalyzer(int loginDays) : _loginDays(loginDays) {}
	virtual void analyze(std::map<int, int>& grade, time_t now);
private:
	void loadFromDB();
	void loadCurrentTable(const std::string& tableName);
	void loadLastTable(const std::string& tableName);
	void buildBitMap();
	void buildUserActive(int limit_days, int active_days, std::map<int, int>& grade);

	std::map< int, std::vector<int> > _userLoginMap;
	std::map< int, std::bitset<32> > _userLoginBitMap;

	int _loginDays;
};

class UgcAnalyzer : virtual public Analyzer {
public:
	UgcAnalyzer(float ugcPercent) : _ugcPercent(ugcPercent) {} 
	virtual void analyze(std::map<int, int>& grade, time_t now);
private:
	void loadFromLogFile(string path, std::map<int, int>& grade, bool ignore);
	void loadDataFromFile(std::map<int, int>& grade);
	void dumpDataToFile(std::map<int, int>& grade);
	string getLogFilePath(string fileDate);
	string getDataFilePath();
	string getFileDate(time_t time);
	int getScore(string type, int count);

	float _ugcPercent;
};

//********************************************************************************************

class CurrentLoginResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	CurrentLoginResultHandler(int& last, std::map< int, std::vector<int> >& userLoginMap): _last(last), _userLoginMap(userLoginMap) {}	
	virtual bool handle(mysqlpp::Row& row) const;	

private:
	int& _last;
	std::map< int, std::vector<int> >& _userLoginMap;
};

//********************************************************************************************

class LastLoginResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	LastLoginResultHandler(int& last, std::map< int, std::vector<int> >& userLoginMap): _last(last), _userLoginMap(userLoginMap) {}	
	virtual bool handle(mysqlpp::Row& row) const;	

private:
	int& _last;
	std::map< int, std::vector<int> >& _userLoginMap;
};

}
}
#endif
