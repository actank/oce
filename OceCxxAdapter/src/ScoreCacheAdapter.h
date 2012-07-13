#ifndef __SCORE_CACHE_ADAPTER_H__
#define __SCORE_CACHE_ADAPTER_H__

#include <boost/lexical_cast.hpp>
#include <IceUtil/RWRecMutex.h>
#include <ScoreCache.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>
#include <TaskManager.h>

namespace xce {
namespace scorecache {
namespace adapter {

const int MAX_LEVEL = 45;

struct ClientScoreDataN : public xce::scorecache::ScoreDataN {
	ClientScoreDataN( xce::scorecache::ScoreDataN& sdn ):
		id(sdn.id),
		historyLoginDays(sdn.historyLoginDays),
		continueLoginDays(sdn.continueLoginDays),
		activeDays(sdn.activeDays),
		lastLoginTime(sdn.lastLoginTime),
		loginType(sdn.loginType),
		score(sdn.score),
		level(sdn.level),
		nextLevelScore(sdn.nextLevelScore),
		awardCount(sdn.awardCount),
		vip(sdn.vip),
		sign(sdn.sign) {
			
			if(level<=0) return;
			if(level>=MAX_LEVEL)level = MAX_LEVEL;
			int doLevel = level;
			std::ostringstream active;
			active << (continueLoginDays>=7?"levelHot-":"level-");
			for (int i = 0; i < doLevel / 15; i++){
				icons.push_back(active.str()+"4");
			}
			int next = doLevel % 15;
			if (next > 0) {
				string nextStr = (next >= 10) ? "3" : (next >= 5) ? "2" : "1";
				icons.push_back(active.str()+nextStr);
			}

	};
	int id;
	///
	int historyLoginDays;
	int continueLoginDays;
	int activeDays;
	long lastLoginTime;
	int loginType;
	///
	int score;
	int level;
	int nextLevelScore;
	///
	int awardCount;
	///
	/// vip<0: disable
	/// vip>0: vip_level(enable)
	/// vip==0: 0==1
	int vip;
	///
	long sign;
	std::vector<std::string> icons;
};

class ScoreCacheNAdapter : public MyUtil::ReplicatedClusterAdapterI<ScoreCacheNManagerPrx>, public MyUtil::Singleton<ScoreCacheNAdapter> {
public:
	ScoreCacheNAdapter() :
		MyUtil::ReplicatedClusterAdapterI <ScoreCacheNManagerPrx>("ControllerScoreCacheN", 120, 300) {
	}
	xce::scorecache::ScoreDataN getScoreDataN(int userId);
	xce::scorecache::ScoreDataNMap getScoreDataNMap(const MyUtil::IntSeq& userIds);

	xce::scorecache::ScoreDataN getScoreDataNNotNull(int userId);
	xce::scorecache::ScoreDataNMap getScoreDataNMapNotNull(const MyUtil::IntSeq& userIds);

	ClientScoreDataN getClientScoreDataN(int userId);

	std::vector<xce::scorecache::ScoreDataN> getScoreDataNAllPrx(int userId);
	std::vector<xce::scorecache::ScoreDataNMap> getScoreDataNMapAllPrx(const MyUtil::IntSeq& userIds);

	Ice::ObjectSeq setLoginDays( int userId, int historyLoginDays, int continueLoginDays, long lastLoginTime, int loginType, bool& succ );
	Ice::ObjectSeq updateLoginType( int userId, int loginType, bool& succ );

	Ice::ObjectSeq setScore(int userId, int score, bool& succ);
	Ice::ObjectSeq incScore(int userId, int inc, bool& succ);
	Ice::ObjectSeq decScore(int userId, int dec, bool& succ);

	Ice::ObjectSeq setAwardCount(int userId, int awardCount, bool& succ);
	Ice::ObjectSeq incAwardCount(int userId, int inc, bool& succ);
	Ice::ObjectSeq decAwardCount(int userId, int dec, bool& succ);

	Ice::ObjectSeq setOn(int userId, int pos, bool& succ);
	Ice::ObjectSeq setOff(int userId, int pos, bool& succ);

	Ice::ObjectSeq updateVipState(int userId, int state, bool& succ);
	Ice::ObjectSeq updateVipLevel(int userId, int level, bool& succ);

	int getPrxCount();

	/** 
	 * ScoreLoader invoke this. 
	 **/
        void load(Ice::Int,const Ice::ObjectPtr&);

	void setData(const MyUtil::ObjectResultPtr&);

	void setMask(long mask);
};

class ScoreLoaderNAdapter : public MyUtil::ReplicatedClusterAdapterI<ScoreLoaderNPrx>, public MyUtil::Singleton<ScoreLoaderNAdapter> {
public:
        ScoreLoaderNAdapter() :
                MyUtil::ReplicatedClusterAdapterI <ScoreLoaderNPrx>("ControllerScoreLoaderN", 120, 300) {
        }

	void setLoginDays(int userId, int historyLoginDays, int continueLoginDays, long lastLoginTime, int loginType );
	void updateLoginType( int userId, int loginType );

	xce::scorecache::ScoreDataN setLoginDaysWithRes(int userId, int historyLoginDays, int continueLoginDays, long lastLoginTime, int loginType );
	xce::scorecache::ScoreDataN updateLoginTypeWithRes( int userId, int loginType );

	void setScore(int userId, int score );
	void incScore(int userId, int inc );
	void decScore(int userId, int dec );

	void setAwardCount(int userId, int awardCount );
	void incAwardCount(int userId, int inc );
	void decAwardCount(int userId, int dec );

	void setOn(int userId, int pos);
	void setOff(int userId, int pos);

	void updateVipState(int userId, int state);
	void updateVipLevel(int userId, int level);

	/** 
	 * ScoreLoaderN invoke this. 
	 **/
        void reload(Ice::Int);
        void reloadBatch(const MyUtil::IntSeq&);

	void setMask(long mask);
};

};
};
};

#endif
