#ifndef __I_LIKE_IT_MANAGER_H__
#define __I_LIKE_IT_MANAGER_H__

#include <ILikeIt.h>
#include "IceExt/src/ServiceI.h"
#include "app/ILikeIt/share/ILikeItUtil.h"
#include "UtilCxx/src/TaskManager.h"
#include "DbCxxPool/src/QueryRunner.h"

namespace xce {
namespace ilikeit {

const int TASK_LEVEL_ADD = 1;
const int TASK_LEVEL_REMOVE = 2;
const int LOGPATH_ID_ADD = 0;
const int LOGPATH_ID_REMOVE = 1;

const int SQL_INTERVAL = 30;

// ================= Thread部分 ============================
class InvokeAdd : virtual public IceUtil::Thread {
public:
	void invoke(const LikeshipPtr& ship);
	void invoke(const LikeshipSeq& shipSeq);
protected:
	virtual void run();
private:
	LikeshipSeq _cache;
	IceUtil::Monitor<IceUtil::Mutex> _mutex;
};

class InvokeRemove : virtual public IceUtil::Thread {
public:
	void invoke(const LikeshipPtr& ship);
	void invoke(const LikeshipSeq& shipSeq);
protected:
	virtual void run();
private:
	LikeshipSeq _cache;
	IceUtil::Monitor<IceUtil::Mutex> _mutex;
};

// ===================== ILikeItManagerI主类 ==============================
class ILikeItManagerI : virtual public ILikeItManager,
			public MyUtil::Singleton<ILikeItManagerI>  {
public:
	void initialize();

	virtual void addLike(const LikeshipPtr& ship, const Ice::Current& crt=Ice::Current());	

	virtual LikeInfoPtr addLikeAndGet(const LikeshipPtr& ship, const Ice::Current& crt=Ice::Current());	

	virtual LikeInfo2Ptr addLikeAndGet2(const LikeshipPtr& ship, const Ice::Current& crt=Ice::Current());	

	virtual void addLikeBatch(const LikeshipSeq& shipSeq, const Ice::Current& crt=Ice::Current());

	virtual void removeLike(const LikeshipPtr& ship, const Ice::Current& ctr=Ice::Current());
	
	virtual LikeInfoPtr removeLikeAndGet(const LikeshipPtr& ship, const Ice::Current& ctr=Ice::Current());

	virtual LikeInfo2Ptr removeLikeAndGet2(const LikeshipPtr& ship, const Ice::Current& ctr=Ice::Current());

	virtual void removeLikeBatch(const LikeshipSeq& shipSeq, const Ice::Current& crt=Ice::Current());

private:
	friend class MyUtil::Singleton<ILikeItManagerI>;
	ILikeItManagerI() {
	};

	InvokeAdd _invokeAdd;
	InvokeRemove _invokeRemove;
};



};
};


#endif
