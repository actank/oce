#ifndef _USER_FILTER_H_
#define _USER_FILTER_H_

#include <string>
#include <map>
#include <vector>
#include <bitset>
#include <sstream>
#include <fstream>
#include <IceUtil/IceUtil.h>

#include "QueryRunner.h"


namespace xce {
namespace userfilter {
using namespace std;

const unsigned ALL_ID_COUNT = 2147483647;
//用户id bitmap
typedef bitset <ALL_ID_COUNT> UserBitMap;
//用户一个月的登录信息
typedef bitset<32> LoginInfo;
typedef map< int, LoginInfo > LoginInfoMap;

//从数据库得到用户登录信息，存放地方由参数传入
//Improve Mark
//由于对类模板不是很熟悉，暂时保留原有的写死获取数据的代码
//以后可以修改从要多少数据生成多少数据
class UserLoginInfoLoader : virtual public IceUtil::Shared{
public:
	UserLoginInfoLoader(){}
	void loadFromDB(LoginInfoMap& loginInfoMap, time_t now, unsigned min=0, unsigned max=ALL_ID_COUNT);
private:
	void loadCurrentTable(const std::string& tableName, unsigned min=0, unsigned max=ALL_ID_COUNT);
	void loadLastTable(const std::string& tableName, unsigned min=0, unsigned max=ALL_ID_COUNT);
	void buildBitMap(LoginInfoMap& loginInfoMap);

	std::map< int, std::vector<int> > loginInfoMapOf2Month;
};
typedef IceUtil::Handle<UserLoginInfoLoader> UserLoginInfoLoaderPtr;

//从用户总表中筛选用户数据
class AllUserFilter : virtual public IceUtil::Shared{
public:
	AllUserFilter(){}
	void getAllUserIDs(UserBitMap* id, unsigned min=0, unsigned max=ALL_ID_COUNT);
};
typedef IceUtil::Handle<AllUserFilter> AllUserFilterPtr;

//判断一个用户的ID是否符合需要
class Checker : virtual public IceUtil::Shared{
public:
	Checker(){}
	virtual bool checkID(int id, LoginInfo loginInfo) = 0;
};
typedef IceUtil::Handle<Checker> CheckerPtr;

//判断一个用户的id是否在limit天内登录天数是否超过一定loginDays
class ActiveChecker : public virtual Checker{
public:
	ActiveChecker(int limit, int loginDays) : _limit(limit), _loginDays(loginDays){
		mask.reset();
		for(int i=0; i<_limit; i++){
			mask.set(i);
		}
	}
	virtual bool checkID(int id, LoginInfo loginInfo);
private:
	int _limit;
	int _loginDays;
	std::bitset<32> mask;
};
typedef IceUtil::Handle<ActiveChecker> ActiveCheckerPtr;

//从登录用户信息中筛选用户数据
class UserLoginFilter : virtual public IceUtil::Shared{
public:
	virtual void getLoginUserIDs(UserBitMap* userBitMap, LoginInfoMap& loginInfoMap, CheckerPtr& checker);
};
typedef IceUtil::Handle<UserLoginFilter> UserLoginFilterPtr;

////通过ice设置用户state
//class ZombieUserScanner : virtual public IceUtil::Shared{
//public:
//	void setState(UserBitMap* allUserMap, UserBitMap* notZombieUserMap);
//};
//typedef IceUtil::Handle<ZombieUserScanner> ZombieUserScannerPtr;

//--------------------------------------------------------------
class AllUserResultHandler : public com::xiaonei::xce::ResultHandler{
public:
	AllUserResultHandler(int& last, UserBitMap * userBitMap) : _last(last), _userBitMap(userBitMap){}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	int& _last;
	UserBitMap * _userBitMap;
};

//--------------------------------------------------------------
class CurrentLoginResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	CurrentLoginResultHandler(int& last, std::map< int, std::vector<int> >& userLoginMap): _last(last), _userLoginMap(userLoginMap) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	int& _last;
	std::map< int, std::vector<int> >& _userLoginMap;
};

//--------------------------------------------------------------
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
