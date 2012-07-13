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
#include <IceUtil/IceUtil.h>
#include <ext/hash_map>

const int GAP = 7;
const int DB_CLUSTER = 100;
const std::string DB_NAME = "user_score";
const std::string TABLE_NAME = "score_login_";

struct LoginData {
	int user_id;
	int continue_days;
	long last_day;
};

class LoginRecordWorker
{
public:
	LoginRecordWorker();
	virtual ~LoginRecordWorker();
	void work(bool o_,bool oc_,bool r_,bool w_,const std::string& wf_);
private:
	std::vector<LoginData> getLoginRecordData(const std::vector<int>&,int);
	std::map<int,bool> getUserState(const vector<int>& ids);
	std::map<int,bool> getKeepUserConfig(const vector<int>& ids);

	void doCheckOrange(const std::vector<LoginData>&,bool,bool,const std::string&);

	void oranging(const std::map<int,bool>&);
	void recording(bool r_);
private:
	long _today;
	time_t _time;

	std::map<int,int> _continue_stat;
	int _yesterday_login_stat;
	int _yesterday_login_continue_stat;
	int _clean_stat;
	int _orange_err_stat;
	int _black_err_stat;
};

class IdListResultHandler : virtual public com::xiaonei::xce::ResultHandler
{
public:
        IdListResultHandler(int& large_id, vector<int>& result) :
                _large_id(large_id), _result(result) {
        }
protected:
        virtual bool handle(mysqlpp::Row& row) const;
private:
        int& _large_id;
        std::vector<int>& _result;
};

class LoginRecordDataHandler : public com::xiaonei::xce::ResultHandler
{
public:
	LoginRecordDataHandler(std::vector<LoginData>& data):
		_data(data){
	}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	std::vector<LoginData>& _data;
};

class UserStateHandler : public com::xiaonei::xce::ResultHandler
{
public:
	UserStateHandler(std::map<int,bool>& data):
		_data(data){
	}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	std::map<int,bool>& _data;
};

class KeepUserConfigHandler : public com::xiaonei::xce::ResultHandler
{
public:
	KeepUserConfigHandler(std::map<int,bool>& data):
		_data(data){
	}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	std::map<int,bool>& _data;
};

#endif
