#ifndef __ScoreCacheData_I_h__
#define __ScoreCacheData_I_h__

#include "ScoreCache.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <cmath>

namespace xce {
namespace scorecache {

const static int scores[] = { 0, 3000, 8000, 15000, 24000, 35000, 48000, 56000, 80000, 99000,
	183000, 300000, 423000, 552000, 787000, 828000, 975000, 1128000, 1287000, 1452000,
	1623000, 1800000, 1983000, 2172000, 2367000, 2568000, 2775000, 2988000, 3207000, 3432000,
	3663000, 3900000, 4143000, 4392000, 4647000, 4908000, 5175000, 5448000, 5727000, 6012000,
	8000000, 10000000, 12000000, 15000000, 20000000 };

const static int levelCount = (int)sizeof(scores);
static long getLevelAndNextScore(int score) {
	if (score >= 0) {
		for (int i = 0; i < levelCount; i++) {
			if (score < scores[i]) {
				long res = i;
				res = (res<<32)+scores[i];
				return res;
			}
		}
	}
	return -1;
}

class ScoreCacheDataI: public ScoreCacheData{
public:
	ScoreCacheDataI( const ScoreCacheDataPtr& data ){
		if( data ){
			historyLoginDays = data->historyLoginDays;
			continueLoginDays = data->continueLoginDays;
			lastLoginTime = data->lastLoginTime;
			loginType = data->loginType;
			score = data->score;
			awardCount = data->awardCount;
			vip = data->vip;
			sign = data->sign;
		} else{
			historyLoginDays = 1;
			continueLoginDays = 1;
			lastLoginTime = 1000;
			loginType = 4;
			score = 0;
			awardCount = 0;
			vip = -1;
			sign = 0;
		}
	};
	ScoreCacheDataI( int historyLoginDays_,
			int continueLoginDays_,
			long lastLoginTime_,
			int loginType_,
			int score_,
			int awardCount_,
			int vip_,
			long sign_ ){
		historyLoginDays = historyLoginDays_;
		continueLoginDays = continueLoginDays_;
		lastLoginTime = lastLoginTime_;
		loginType = loginType_;
		score = score_;
		awardCount = awardCount_;
		vip = vip_;
		sign = sign_;
	};
	ScoreCacheDataI(){
			historyLoginDays = 1;
			continueLoginDays = 1;
			lastLoginTime = 1000;
			loginType = 4;
			score = 0;
			awardCount = 0;
			vip = -1;
			sign = 0;
	};
	/*
	int historyLoginDays;
	int continueLoginDays;
	long lastLoginTime;
	int loginType;
	*/
	void setLogin( int historyLoginDays_,
			int continueLoginDays_,
			long lastLoginTime_,
			int loginType_){
		historyLoginDays = historyLoginDays_;
		continueLoginDays = continueLoginDays_;
		lastLoginTime = lastLoginTime_;
		loginType = loginType_;
	};
	void updateLoginType( int loginType_ ){
		loginType = loginType_;
	};
	/*
	int score;
	*/
	void setScore( int score_ ){
		if( score < 0 ){
			score = 0;
		} else{
			score = score_;
		}
	};
	void incScore( int inc ){
		score += inc;
	};
	void decScore( int dec ){
		score -= dec;
		if( score < 0 ) score = 0;
	};
	/*
	int awardCount;
	*/
	void setAwardCount( int awardCount_ ){
		if( awardCount_ < 0 ){
			awardCount = 0;
		} else{
			awardCount = awardCount_;
		}
	};
	void incAwardCount( int inc ){
		awardCount += inc;
	};
	void decAwardCount( int dec ){
		awardCount -= dec;
		if( awardCount < 0 ) awardCount = 0;
	};
	///
	/// vip<0: disable
	/// vip>0: vip_level(enable)
	/// vip==0: 0==1
	/*
	int vip;
	*/
	void updateVipState( int state_ ){
		vip = (0!=(state_&(1<<1))) ? abs(vip) : -abs(vip);
	};
	void updateVipLevel( int level_ ){
		if( level_ == 0 ){
			level_ = 1;
		}
		vip = vip>0 ? level_ : -level_;
	};
	/*
	long sign;
	*/
	void setOn( int pos ){
		if( pos<0 || pos>=64 ) return;
		sign |= (((long)1)<<pos);
	};
	void setOff( int pos ){
		if( pos<0 || pos>=64 ) return;
		sign &= (~(((long)1)<<pos));
	};
	bool getSign( int pos ){
		if( pos<0 || pos>=64 ) return false;
		return 0 != (sign & ( ((long)1) << pos ));
	};
	ScoreData getScoreData(int userId,long mask){
		ScoreData data;
		data.id = userId;
		data.historyLoginDays = historyLoginDays;
		data.continueLoginDays = continueLoginDays;
		data.lastLoginTime = lastLoginTime;
		data.loginType = loginType;
		data.score = score;
    		long tmp = getLevelAndNextScore(score);
		data.level = (int)((tmp>>32)&(0xffffffff));
		data.nextLevelScore = (int)(tmp&(0xffffffff));
		data.awardCount = awardCount;
		data.vip = vip;
		data.sign = sign & (~mask);
		return data;
	};
	ScoreCacheDataPtr getScoreCacheData(){
		ScoreCacheDataPtr res = new ScoreCacheData;
		res->historyLoginDays = historyLoginDays;
		res->continueLoginDays = continueLoginDays;
		res->lastLoginTime = lastLoginTime;
		res->loginType = loginType;
		res->score = score;
		res->awardCount = awardCount;
		res->vip = vip;
		res->sign = sign;
		return res;
	};
};
typedef IceUtil::Handle<ScoreCacheDataI> ScoreCacheDataIPtr;

class ScoreCacheDataNI: public ScoreCacheDataN{
private:
        IceUtil::Mutex _mutex;
public:
	ScoreCacheDataNI( const ScoreCacheDataNPtr& data ){
		if( data ){
			historyLoginDays = data->historyLoginDays;
			continueLoginDays = data->continueLoginDays;
			activeDays = data->activeDays;
			lastLoginTime = data->lastLoginTime;
			loginType = data->loginType;
			score = data->score;
			awardCount = data->awardCount;
			vip = data->vip;
			sign = data->sign;
		} else{
			historyLoginDays = 1;
			continueLoginDays = 1;
			activeDays = 2;
			lastLoginTime = 1000;
			loginType = 4;
			score = 0;
			awardCount = 0;
			vip = -1;
			sign = 0;
		}
	};
	ScoreCacheDataNI( int historyLoginDays_,
			int continueLoginDays_,
			int activeDays_,
			long lastLoginTime_,
			int loginType_,
			int score_,
			int awardCount_,
			int vip_,
			long sign_ ){
		historyLoginDays = historyLoginDays_;
		continueLoginDays = continueLoginDays_;
		activeDays = activeDays_;
		lastLoginTime = lastLoginTime_;
		loginType = loginType_;
		score = score_;
		awardCount = awardCount_;
		vip = vip_;
		sign = sign_;
	};
	ScoreCacheDataNI(){
			historyLoginDays = 1;
			continueLoginDays = 1;
			activeDays = 2;
			lastLoginTime = 1000;
			loginType = 4;
			score = 0;
			awardCount = 0;
			vip = -1;
			sign = 0;
	};
	/*
	int historyLoginDays;
	int continueLoginDays;
	int activeDays;
	long lastLoginTime;
	int loginType;
	*/
	int setLogin( int historyLoginDays_,
			int continueLoginDays_,
			long lastLoginTime_,
			int loginType_){
        	IceUtil::Mutex::Lock lock(_mutex);
		//TODO:
		int abs_count;
		if( continueLoginDays_ == 1 ){
			struct tm *tm_local;
			time_t now = time(NULL);
			tm_local = localtime(&now);
			tm_local->tm_hour = 0;
			tm_local->tm_min = 0;
			tm_local->tm_sec = 0;
			time_t time_zero = timelocal(tm_local);
			int days = ( time_zero - lastLoginTime/1000 ) / ( 24*3600 );
			if( days<=0 ){
				abs_count = -1;
			} else if( days>=30 ){
				activeDays = 0+(activeDays%2);
				abs_count = -1;
			} else{
				abs_count = days;
			}
		} else{
			abs_count = 0;
		}
		activeDays = (((((activeDays/2)*2)<<(abs_count+1))+(activeDays%2))|2)&(0x7FFFFFFF);
		historyLoginDays = historyLoginDays_<=0?1:historyLoginDays_;
		continueLoginDays = continueLoginDays_<=0?1:continueLoginDays_;
		lastLoginTime = lastLoginTime_<1000?1000:lastLoginTime_;
		loginType = loginType_==0?4:loginType_;
		return abs_count;
	};
	void updateLoginType( int loginType_ ){
        	IceUtil::Mutex::Lock lock(_mutex);
		loginType = loginType_==0?4:loginType_;
	};
	/*
	int score;
	*/
	void setScore( int score_ ){
        	IceUtil::Mutex::Lock lock(_mutex);
		if( score_ < 0 ){
			score = 0;
		} else{
			score = score_;
		}
	};
	void incScore( int inc ){
        	IceUtil::Mutex::Lock lock(_mutex);
		score += inc;
	};
	void decScore( int dec ){
        	IceUtil::Mutex::Lock lock(_mutex);
		score -= dec;
		if( score < 0 ) score = 0;
	};
	/*
	int awardCount;
	*/
	void setAwardCount( int awardCount_ ){
        	IceUtil::Mutex::Lock lock(_mutex);
		if( awardCount_ < 0 ){
			awardCount = 0;
		} else{
			awardCount = awardCount_;
		}
	};
	void incAwardCount( int inc ){
        	IceUtil::Mutex::Lock lock(_mutex);
		awardCount += inc;
	};
	void decAwardCount( int dec ){
        	IceUtil::Mutex::Lock lock(_mutex);
		awardCount -= dec;
		if( awardCount < 0 ) awardCount = 0;
	};
	///
	/// vip<0: disable
	/// vip>0: vip_level(enable)
	/// vip==0: 0==1
	/*
	int vip;
	*/
	void updateVipState( int state_ ){
        	IceUtil::Mutex::Lock lock(_mutex);
		vip = (0!=(state_&(1<<1))) ? abs(vip) : -abs(vip);
	};
	void updateVipLevel( int level_ ){
		if( level_ == 0 ){
			level_ = 1;
		}
        	IceUtil::Mutex::Lock lock(_mutex);
		vip = vip>0 ? level_ : -level_;
	};
	/*
	long sign;
	*/
	void setOn( int pos ){
		if( pos<0 || pos>=64 ) return;
        	IceUtil::Mutex::Lock lock(_mutex);
		sign |= (((long)1)<<pos);
	};
	void setOff( int pos ){
		if( pos<0 || pos>=64 ) return;
        	IceUtil::Mutex::Lock lock(_mutex);
		sign &= (~(((long)1)<<pos));
	};
	bool getSign( int pos ){
		if( pos<0 || pos>=64 ) return false;
        	IceUtil::Mutex::Lock lock(_mutex);
		return 0 != (sign & ( ((long)1) << pos ));
	};
	ScoreDataN getScoreDataN(int userId,long mask){
		ScoreDataN data;
        	IceUtil::Mutex::Lock lock(_mutex);
		data.id = userId;
		data.historyLoginDays = historyLoginDays;
		data.continueLoginDays = continueLoginDays;
		data.activeDays = activeDays;
		data.lastLoginTime = lastLoginTime;
		data.loginType = loginType;
		data.score = score;
    		long tmp = getLevelAndNextScore(score);
		data.level = (int)((tmp>>32)&(0xffffffff));
		data.nextLevelScore = (int)(tmp&(0xffffffff));
		data.awardCount = awardCount;
		data.vip = vip;
		data.sign = sign & (~mask);
		return data;
	};
	ScoreCacheDataNPtr getScoreCacheDataN(){
		ScoreCacheDataNPtr res = new ScoreCacheDataN;
        	IceUtil::Mutex::Lock lock(_mutex);
		res->historyLoginDays = historyLoginDays;
		res->continueLoginDays = continueLoginDays;
		res->activeDays = activeDays;
		res->lastLoginTime = lastLoginTime;
		res->loginType = loginType;
		res->score = score;
		res->awardCount = awardCount;
		res->vip = vip;
		res->sign = sign;
		return res;
	};
};
typedef IceUtil::Handle<ScoreCacheDataNI> ScoreCacheDataNIPtr;

}
}

#endif
