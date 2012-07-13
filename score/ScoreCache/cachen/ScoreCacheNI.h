#ifndef __SCORE_CACHE_N_I_H__
#define __SCORE_CACHE_N_I_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ObjectCacheI.h"
#include "IceExt/src/ServiceI.h"
#include "../share/ScoreCacheData.h"
#include "../share/ScoreCacheDataFactory.h"

namespace xce {
namespace scorecache {

const int SCORE_CACHE_DATA_N = 0;

const int TASK_LEVEL_SCORE_CACHE_FILL = 1;
const int TASK_LEVEL_SCORE_CACHE_LOAD = 2;

/*****************************************************************************/

class ScoreCacheNManagerI: virtual public ScoreCacheNManager, virtual public MyUtil::Singleton<ScoreCacheNManagerI> {
public:
	void bootstrap(const string& endpoint, int mod);

	virtual xce::scorecache::ScoreDataN getScoreDataN(Ice::Int userId,const Ice::Current&);
	virtual xce::scorecache::ScoreDataNMap getScoreDataNMap(const MyUtil::IntSeq& userIds,const Ice::Current&);

	virtual xce::scorecache::ScoreDataN getScoreDataNNotNull(Ice::Int userId,const Ice::Current&);
	virtual xce::scorecache::ScoreDataNMap getScoreDataNMapNotNull(const MyUtil::IntSeq& userIds,const Ice::Current&);

	virtual Ice::ObjectSeq setLoginDays( Ice::Int userId, Ice::Int historyLoginDays, Ice::Int continueLoginDays, Ice::Long lastLoginTime, Ice::Int loginType, const Ice::Current& );
	virtual Ice::ObjectSeq updateLoginType(Ice::Int userId, Ice::Int loginType,const Ice::Current&);

	virtual Ice::ObjectSeq setScore(Ice::Int userId, Ice::Int score,const Ice::Current& );
	virtual Ice::ObjectSeq incScore(Ice::Int userId, Ice::Int inc,const Ice::Current& );
	virtual Ice::ObjectSeq decScore(Ice::Int userId, Ice::Int dec,const Ice::Current& );

	virtual Ice::ObjectSeq setAwardCount(Ice::Int userId, Ice::Int awardCount,const Ice::Current& );
	virtual Ice::ObjectSeq incAwardCount(Ice::Int userId, Ice::Int inc,const Ice::Current& );
	virtual Ice::ObjectSeq decAwardCount(Ice::Int userId, Ice::Int dec,const Ice::Current& );

	virtual Ice::ObjectSeq setOn(Ice::Int userId, Ice::Int pos,const Ice::Current&);
	virtual Ice::ObjectSeq setOff(Ice::Int userId, Ice::Int pos,const Ice::Current&);

	virtual Ice::ObjectSeq updateVipState(Ice::Int userId, Ice::Int state,const Ice::Current&);
	virtual Ice::ObjectSeq updateVipLevel(Ice::Int userId, Ice::Int level,const Ice::Current&);

	virtual void setMask(Ice::Long mask,const Ice::Current&);
	virtual bool isValid(const Ice::Current&);
	virtual void setValid(bool valid, const Ice::Current&);
	virtual void setData(const MyUtil::ObjectResultPtr& datas, const Ice::Current&);
	/** 
	 * ScoreLoader::load invoke this.
	 **/
	virtual void load(Ice::Int userId,const Ice::ObjectPtr& data, const Ice::Current&);

private:
	string _endpoint;
	int _mod;

	bool _isValid;
	IceUtil::RWRecMutex _validMutex;

	long _mask;
	IceUtil::RWRecMutex _maskMutex;

	friend class MyUtil::Singleton<ScoreCacheNManagerI>;
	ScoreCacheNManagerI():
		_mask(0) {
	}
	ScoreCacheDataNIPtr findScoreCacheDataNI(Ice::Int userId);
	ScoreCacheDataNIPtr locateScoreCacheDataNI(Ice::Int userId);
};

/*****************************************************************************/

class ScoreCacheNFactory : virtual public MyUtil::ServantFactory, virtual public MyUtil::Singleton<ScoreCacheNFactory> {
protected:
        friend class MyUtil::Singleton<ScoreCacheNFactory>;
public:
        virtual Ice::ObjectPtr create(Ice::Int id);
};

/*****************************************************************************/

class FillTask: virtual public MyUtil::Task {
public:
        FillTask(const MyUtil::ObjectResultPtr& scoreDataN,::Ice::Int level=TASK_LEVEL_SCORE_CACHE_FILL):
                MyUtil::Task(level), _scoreDataN(scoreDataN) {
        }
        virtual void handle();
private:
        MyUtil::ObjectResultPtr _scoreDataN;
};

class LoadTask: virtual public MyUtil::Task {
public:
        LoadTask(Ice::Int id, const Ice::ObjectPtr& obj, Ice::Int level=TASK_LEVEL_SCORE_CACHE_LOAD):
		MyUtil::Task(level), _id(id), _obj(obj)  {
        }
        virtual void handle();
private:
        Ice::Int _id;
        Ice::ObjectPtr _obj;

};

//***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
public:
	virtual void configure(const Ice::PropertiesPtr& props);
};

}
}

#endif
