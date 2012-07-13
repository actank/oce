#ifndef __KeepUseI_h__
#define __KeepUseI_h__

#include <KeepUse.h>

#include "TaskManager.h"
#include "ServiceI.h"
#include <bitset>

namespace xce
{

namespace keepuse
{

typedef std::bitset<2147483647> UserBits;

class KeepUseManagerI : virtual public KeepUseManager
{
public:
	KeepUseManagerI() : can_unset_(false), switch_count_(0){};
	void initialize(int size);
	virtual void todayuse(::Ice::Int,
                          const Ice::Current&);

    virtual void use(::Ice::Int, const ::xce::keepuse::Int2BoolMap&,
                                               const Ice::Current&);

    virtual ::xce::keepuse::Int2BoolMap isused(::Ice::Int, ::Ice::Int,
                                               const Ice::Current&);

    virtual bool newday(const ::std::string&,
                        const Ice::Current&);
	virtual bool canUnset(const Ice::Current&);
	virtual void setCanUnset(bool, const Ice::Current&);

	virtual ~KeepUseManagerI();
private:
	IceUtil::RWRecMutex mutex_;
	string current_day_;
	int current_pos_;
	int pos_max_;
	std::vector<UserBits*> datas_;

	int switch_count_;
	bool can_unset_;
};

class SetStateTask : virtual public MyUtil::Task {
public:
	SetStateTask(int id) : id_(id) {};
	virtual void run();
private:
	int id_;
};

class UnsetStateTask : virtual public MyUtil::Task {
public:
	UnsetStateTask(int id) : id_(id) {};
	virtual void run();
private:
	int id_;
};

}

}

#endif

