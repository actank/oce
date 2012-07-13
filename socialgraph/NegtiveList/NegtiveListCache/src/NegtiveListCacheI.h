#ifndef _NEGTIVELISTCACHEI_H_
#define _NEGTIVELISTCACHEI_H_

#include <NegtiveListCache.h>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "ServiceI.h"
#include <ObjectCacheI.h>
#include <QueryRunner.h>

namespace xce {
namespace socialgraph {

const static int NEGTIVELIST_DATA = 1;

//********************************************************************************

class NegtiveListData : virtual public Ice::Object, virtual IceUtil::Mutex {
public:
	NegtiveListData(::Ice::Int hostId, const MyUtil::IntSeq& negtiveList) :
		_hostId(hostId), _negtiveList(negtiveList) { };
	MyUtil::IntSeq Get(int limit = -1);	
	void Remove(const MyUtil::IntSeq& negtiveList);

private:
	::Ice::Int _hostId;
	MyUtil::IntSeq _negtiveList;
};
typedef IceUtil::Handle<NegtiveListData> NegtiveListDataPtr;

//********************************************************************************

class NegtiveListCacheI : virtual public NegtiveListCache,
		virtual public MyUtil::Singleton<NegtiveListCacheI> {
public:
    virtual void Set(::Ice::Int, const MyUtil::IntSeq&, const Ice::Current&);
    virtual MyUtil::IntSeq Get(::Ice::Int, ::Ice::Int, const Ice::Current&);
    virtual void Remove(::Ice::Int, const MyUtil::IntSeq&, const Ice::Current&);

private:
	NegtiveListDataPtr FindNegtiveListData(::Ice::Int id);
	friend class MyUtil::Singleton<NegtiveListCacheI>;
};

//********************************************************************************

class NegtiveListDataFactory : virtual public MyUtil::ServantFactory {
};

//********************************************************************************

class RemoveTask : virtual public MyUtil::Task {
public:
	RemoveTask(::Ice::Int ownerId, const MyUtil::IntSeq& negtiveList):
		_ownerId(ownerId), _negtiveList(negtiveList) {};
	virtual void handle();
		
private:
	::Ice::Int _ownerId;
	MyUtil::IntSeq _negtiveList;
};

}
}

#endif
