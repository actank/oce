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
#include <ScoreCache.h>

const int DB_CLUSTER = 100;
const std::string DB_NAME = "user_score";
const std::string LOGIN_TABLE_NAME = "score_login_";
const std::string SIGN_TABLE_NAME = "score_sign_";

class ScoreCheckWorker
{
public:
	ScoreCheckWorker();
	virtual ~ScoreCheckWorker();
	void check( int u_, bool sc_ );
	void work( int d_, bool sc_ );
private:
	MyUtil::ObjectResultPtr getScoreDataFromDB(const std::vector<int>& ids,int);
	std::vector<xce::scorecache::ScoreDataNMap> getScoreDataFromAllPrx(const std::vector<int>& ids);

	void verify(MyUtil::ObjectResultPtr, const std::vector<xce::scorecache::ScoreDataNMap>&, bool, bool );

	xce::scorecache::ScoreCacheDataNPtr findone(const std::vector<xce::scorecache::ScoreDataN>&,bool,bool&,bool&,bool&,bool&,bool&);
	void savetheone(Ice::Int,const xce::scorecache::ScoreCacheDataNPtr&,bool,bool,bool,bool,bool);
private:
	long _zerotime;
	time_t _currtime;

	int _ok_stat;
	int _allmiss_stat;
	int _somemiss_stat;
	int _login_err_stat;
	int _score_err_stat;
	int _award_err_stat;
	int _vip_err_stat;
	int _sign_err_stat;
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

#endif
