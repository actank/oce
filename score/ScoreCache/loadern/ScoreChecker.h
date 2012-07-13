#ifndef  _SCORECHECKER_H_
#define  _SCORECHECKER_H_

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

//***************************************************************************

class CheckerHelper: public MyUtil::Singleton<CheckerHelper> {
public:
        void initialize();
        void invokeDelay( Ice::Int );
        void invokeNow( Ice::Int );
        void swap(std::vector<int>& empty_vector);
private:
        IceUtil::Monitor<IceUtil::Mutex> mutex_;
        std::vector<int> datas_;

        class CheckerThread: virtual public IceUtil::Thread {
        public:
                CheckerThread(CheckerHelper& helper) :
                        helper_(helper) {
				tzset();
				time_t _currtime = time(NULL);

				struct tm *local;
				local = localtime(&_currtime);

				local->tm_hour = 0;
				local->tm_min = 0;
				local->tm_sec = 0;

				_zerotime = timelocal(local);
                        }
                virtual void run();
        private:
                CheckerHelper& helper_;
	private:
		long _zerotime;

		int _ok_stat;
		int _allmiss_stat;
		int _somemiss_stat;
		int _login_err_stat;
		int _score_err_stat;
		int _award_err_stat;
		int _vip_err_stat;
		int _sign_err_stat;
	private:
		MyUtil::ObjectResultPtr getScoreDataFromDB(const std::vector<int>& ids,int);
		std::vector<xce::scorecache::ScoreDataNMap> getScoreDataFromAllPrx(const std::vector<int>& ids);
		void verify(MyUtil::ObjectResultPtr, const std::vector<xce::scorecache::ScoreDataNMap>&, bool, bool );
		xce::scorecache::ScoreCacheDataNPtr findone(const std::vector<xce::scorecache::ScoreDataN>&,bool,bool&,bool&,bool&,bool&,bool&);
		void savetheone(Ice::Int,const xce::scorecache::ScoreCacheDataNPtr&,bool,bool,bool,bool,bool);
        };
        typedef IceUtil::Handle<CheckerThread> CheckerThreadPtr;

        CheckerThreadPtr checker_;
};

#endif
