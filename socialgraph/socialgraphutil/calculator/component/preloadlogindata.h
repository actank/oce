#ifndef _PRELOAD_NEGTIVE_USER_H__
#define _PRELOAD_NEGTIVE_USER_H__

#include <string>
#include <map>
#include <vector>
#include <bitset>
#include <sstream>
#include <fstream>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "ObjectCacheI.h"
#include "QueryRunner.h"

#include "socialgraph/socialgraphutil/calculator/component.h"

namespace xce {
namespace socialgraph {

const static int NEGTIVEDAYS = 15;
const static int ACTIVEDAYS = 15;
const static int NOLOGINDAYS = 7;

//********************************************************************************************
//加载数据类
class PreloadLoginData: public Component {
public:
	void LoadFromDB();
	void GetNegtiveList(std::vector<int>& negtive_list);
	std::map< int, std::bitset<32> > GetUserLogin(const std::vector<int>& uids);

	void Load() {
		LoadFromDB();
	}

private:
	void LoadFirstTable(const std::string& tableName);
	void LoadSecondTable(const std::string& tableName);
	void BuildBitMap();
	void BuildNegtiveUserList();

	std::map< int, std::vector<int> > _userLoginMap;
	std::map< int, std::bitset<32> > _userLoginBitMap;
	std::vector<int> _negtiveUserList;
	IceUtil::RWRecMutex init_flag_mutex_;
};

//********************************************************************************************

class FirstLoginReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	FirstLoginReadHandler(int& last, std::map< int, std::vector<int> >& userLoginMap): _last(last), _userLoginMap(userLoginMap) {}	
	virtual bool handle(mysqlpp::Row& row) const;	

private:
	int& _last;
	std::map< int, std::vector<int> >& _userLoginMap;
};

//********************************************************************************************

class SecondLoginReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	SecondLoginReadHandler(int& last, std::map< int, std::vector<int> >& userLoginMap): _last(last), _userLoginMap(userLoginMap) {}	
	virtual bool handle(mysqlpp::Row& row) const;	

private:
	int& _last;
	std::map< int, std::vector<int> >& _userLoginMap;
};

}
}

#endif
