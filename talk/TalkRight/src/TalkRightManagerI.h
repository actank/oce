#ifndef TALKRIGHTMANAGERI_H_
#define TALKRIGHTMANAGERI_H_

#include "TalkRight.h"
#include <Ice/Ice.h>
#include "ServiceI.h"
#include "LogWrapper.h"
#include "Singleton.h"
#include <set>

namespace com {
namespace xiaonei {
namespace talk {

using namespace std;
using namespace MyUtil;

template<class T> 
class BlockSet : public Ice::Object {
	set<T> _set;
public:
	void add(T& blockId) {
		_set.insert(blockId);
	}
	;
	void remove(T& blockId){
		_set.erase(blockId);
	};
	bool has(T& blockId) {
		typename set<T>::iterator it;
		it = _set.find(blockId);
		return it != _set.end() ? true : false;
	}
	;
	vector<T> getSeq()
	{
		vector<T> seq;
		for(typename set<T>::iterator it = _set.begin(); it != _set.end(); ++it){
			seq.push_back(*it);
		}
		return seq;
	};
};
typedef IceUtil::Handle< BlockSet<int> > UserBlockSetPtr;
typedef IceUtil::Handle< BlockSet<string> > SysMsgBlockSetPtr;

class HiddenSet : public Ice::Object
{
	set<int> _set;
public:
	void add(int userId) {
		_set.insert(userId);
	};
	void remove(int userId) {
		_set.erase(userId);
	};
	bool has(int userId) {
		set<int>::iterator it = _set.find(userId);
		return it != _set.end() ? true:false;
	};
};

typedef IceUtil::Handle<HiddenSet> HiddenSetPtr;


class UserBlockSetFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
	virtual ObjectResultPtr create(const MyUtil::IntSeq& ids);
};

class SysMsgBlockSetFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
	virtual ObjectResultPtr create(const MyUtil::IntSeq& ids);
};

class UserStatusFactoryI : public ServantFactory{
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
	virtual ObjectResultPtr create(const MyUtil::IntSeq& ids);	
};


class RightManagerI : public virtual RightManager
{
	HiddenSetPtr _hiddenUsers;
public:
	RightManagerI(){
		_hiddenUsers = new HiddenSet();
	}
	//virtual bool verify(Ice::Int from, Ice::Int to , Ice::Int type, const Ice::Current& = Ice::Current());
	virtual MyUtil::IntSeq batchVerify(const VerifySeq& seq, const Ice::Current& = Ice::Current());

	virtual bool isHiddenUser(Ice::Int userId,const Ice::Current& = Ice::Current());
	virtual void addHiddenUser(Ice::Int userId,const Ice::Current& = Ice::Current());
	virtual void removeHiddenUser(Ice::Int userId,const Ice::Current& = Ice::Current());
	virtual void reloadBlockUser(Ice::Int userId,const Ice::Current& = Ice::Current());
	
	virtual void modifySystemMessageBlock(Ice::Int userId, const MyUtil::StrSeq& typeSeq,const Ice::Current& = Ice::Current());
	//virtual void removeSystemMessageBlock(Ice::Int userId, const string& type,const Ice::Current& = Ice::Current());
	virtual MyUtil::StrSeq getSystemMessageBlock(Ice::Int userId,const Ice::Current& = Ice::Current());
	virtual UserStatusPtr getUserStatus(Ice::Int userId,const Ice::Current& = Ice::Current());
	virtual void reloadUserStatus(Ice::Int userId, const Ice::Current& = Ice::Current());
};



};
};
}
;

#endif /*TALKRIGHTMANAGERI_H_*/
