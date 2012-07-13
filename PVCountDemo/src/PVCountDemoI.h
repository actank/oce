/*#######################################################################
 # Author: tangzhaohui
 # Created Time: 2010年09月15日 星期三 14时55分41秒
 # File Name: PVCountDemoI.h
 # Description:
 #######################################################################*/
#ifndef _PV_COUNT_DEMO_I_H_
#define _PV_COUNT_DEMO_I_H_

#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <string>
#include "PVCountDemo.h"
#include "Evictor.h"
#include "Singleton.h"

namespace Demo {

using namespace std;
using namespace MyUtil;

const string TABLE_PVCOUNT = "yuanfei";
const string DB_INSTANCE = "im_stat";

#define COL_ID "b"
#define COL_PVCOUNT "a"

template<class _Key, class _Object>
class EvictorCache: public MyUtil::SizedEvictor<_Key, _Object> {
public:
	typedef typename MyUtil::SizedEvictor<_Key, _Object>::EvictorMap::iterator EvictorIterator;

	EvictorIterator begin() {
		return MyUtil::SizedEvictor<_Key, _Object>::_map.begin();
	}

	EvictorIterator end() {
		return MyUtil::SizedEvictor<_Key, _Object>::_map.end();
	}

	_Object removeByIterator(const EvictorIterator& pos) {
		_Object obj;
		IceUtil::Mutex::Lock lock(MyUtil::SizedEvictor<_Key, _Object>::_mutex);
		if (pos != MyUtil::SizedEvictor<_Key, _Object>::_map.end()) {
			obj = pos->second.obj;
			MyUtil::SizedEvictor<_Key, _Object>::_queue.erase(pos->second.pos);
			MyUtil::SizedEvictor<_Key, _Object>::_map.erase(pos);
		}
		return obj;
	}

	_Object getObject(const EvictorIterator& pos, const _Object& def) {
		IceUtil::Mutex::Lock lock(MyUtil::SizedEvictor<_Key, _Object>::_mutex);
		if (pos != MyUtil::SizedEvictor<_Key, _Object>::_map.end()) {
			return pos->second.obj;
		}
		return def;
	}
};

class PVCountDemoI: virtual public PVCountDemo, public Singleton<PVCountDemoI> {
public:
	virtual void add(Ice::Int userId, Ice::Int pvCount, const Ice::Current& =
			Ice::Current());
	virtual void update(Ice::Int userId, Ice::Int pvCount, const Ice::Current& =
			Ice::Current());
	virtual void remove(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual Ice::Int getPVCountByUserId(Ice::Int id, const Ice::Current& =
			Ice::Current());
	PVCountDemoI();	// cache size default 10
	PVCountDemoI(int cacheSize);
	void checkCache(); // screen the cache per 10 minute, remove the item which's pvcount is bigger than 5
protected:
	typedef EvictorCache<int, int> DemoCache;
	typedef DemoCache::EvictorIterator CacheIterator;
private:
	DemoCache _cache;
	IceUtil::Mutex _mutex;
};

}
;

#endif //_PV_COUNT_DEMO_I_H_
