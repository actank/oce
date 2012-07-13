#ifndef _SCORE_CACHE_ICE_
#define _SCORE_CACHE_ICE_

#include <ObjectCache.ice>
#include <Ice/BuiltinSequences.ice>
module xce {
module scorecache {

	struct ScoreData{
		int id;
		///
		int historyLoginDays;
		int continueLoginDays;
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
	};
        dictionary<int, ScoreData> ScoreDataMap;

	class ScoreCacheData{
		int historyLoginDays;
		int continueLoginDays;
		long lastLoginTime;
		int loginType;
		///
		int score;
		///
		int awardCount;
		///
		/// vip<0: disable
		/// vip>0: vip_level(enable)
		/// vip==0: 0==1
		int vip;
		///
		long sign;
	};

////////////////////////////////////////////////////////////////////////////////////

	struct ScoreDataN{
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
	};
        dictionary<int, ScoreDataN> ScoreDataNMap;

	class ScoreCacheDataN{
		int historyLoginDays;
		int continueLoginDays;
		int activeDays;
		long lastLoginTime;
		int loginType;
		///
		int score;
		///
		int awardCount;
		///
		/// vip<0: disable
		/// vip>0: vip_level(enable)
		/// vip==0: 0==1
		int vip;
		///
		long sign;
	};

	interface ScoreCacheNManager {
		ScoreDataN getScoreDataN(int userId);
		ScoreDataNMap getScoreDataNMap(MyUtil::IntSeq userIds);

		ScoreDataNMap getScoreDataNMapNotNull(MyUtil::IntSeq userIds);
		ScoreDataN  getScoreDataNNotNull(int userid);

		Ice::ObjectSeq setLoginDays( int userId, int historyLoginDays, int continueLoginDays, long lastLoginTime, int loginType );
		Ice::ObjectSeq updateLoginType(int userId, int loginType);

		Ice::ObjectSeq setScore(int userId, int score );
		Ice::ObjectSeq incScore(int userId, int inc );
		Ice::ObjectSeq decScore(int userId, int dec );

		Ice::ObjectSeq setAwardCount(int userId, int awardCount );
		Ice::ObjectSeq incAwardCount(int userId, int inc );
		Ice::ObjectSeq decAwardCount(int userId, int dec );

		Ice::ObjectSeq setOn(int userId, int pos);
		Ice::ObjectSeq setOff(int userId, int pos);

		Ice::ObjectSeq updateVipState(int userId, int state);
		Ice::ObjectSeq updateVipLevel(int userId, int level);
		/** 
		 * ScoreLoader::load invoke this.
		 **/
		void load(int userId,Object o);

		void setMask(long mask);
		bool isValid();
		void setValid(bool valid);
		void setData(MyUtil::ObjectResult datas);
	};

	interface ScoreLoaderN
	{
		void setLoginDays(int userId, int historyLoginDays, int continueLoginDays, long lastLoginTime, int loginType );
		void updateLoginType(int userId, int loginType);

		ScoreDataN setLoginDaysWithRes(int userId, int historyLoginDays, int continueLoginDays, long lastLoginTime, int loginType );
		ScoreDataN updateLoginTypeWithRes(int userId, int loginType);

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

		void reload(int userId);
		void reloadBatch(MyUtil::IntSeq userIds);

		void setMask(long mask);
	};
};
};

#endif
