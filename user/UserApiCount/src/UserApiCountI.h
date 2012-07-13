#ifndef __USER_API_COUNT_I_H__
#define __USER_API_COUNT_I_H__

#include "ServiceI.h"
#include "UserApiCount.h"
#include "BatchUpdaterAdapter.h"
#include "QueryRunner.h"

namespace xce {
namespace userapicount {

const int UAC = 0;
const int TASK_LEVEL_CREATE = 1;

class UserApiCounterI: public UserApiCounter, public IceUtil::Mutex {
public:
	UserApiCounterI()	{};
	virtual ~UserApiCounterI() {};

	/* brief	类型为type的count增加step
	 * param 	type	信息类型
	 * param	step	增加的数量
	 */
	void inc(int type, int step);

	/* brief	类型为type的count减少step
	 * param	type	信息类型
	 * param	step	减少的数量
	 */
	void dec(int type, int step);

	/* brief	将类型为type的count赋为step
	 * param	type	信息类型
	 * param	value	赋值数值
	 */
	void set(int type, int value);

	/* breif	取得类型为type的count
	 * param	type	信息类型
	 * return	类型对应的count
	 */
	int get(int type);

private:
	/* brief	取得类型type是否则数组中
	 * param	type	信息类型
	 * param	pos	如果该类型已存在，为该类型在keys中的位置;否则为该类型可以在keys中插入的位置
	 * return	该类型是否已在数组中
	 * remarks	必须在已持有锁的前提下才能调用
	 */
	bool find(int type, int& pos);

	/* brief	插入新的信息对
	 * param	type	插入的信息类型
	 * param	value	插入的信息值
	 * param	pos	信息插入位置
	 * return	keys和values数组是否正确对应
	 * remarks	必须在已持有锁的前提下才能调用
	 */
	bool insert(int type, int step, int pos);
};

typedef IceUtil::Handle<UserApiCounterI> UserApiCounterIPtr;

class UserApiCountManagerI : virtual public MyUtil::Singleton<UserApiCountManagerI>,
	virtual public UserApiCountManager
{
public:

    virtual void inc(::Ice::Int id,
                     ::Ice::Int type,
                     ::Ice::Int step,
                     const Ice::Current& = Ice::Current());

    virtual void dec(::Ice::Int id,
                     ::Ice::Int type,
                     ::Ice::Int step,
                     const Ice::Current& = Ice::Current());

    virtual ::Ice::Int get(::Ice::Int id,
                           ::Ice::Int type,

			   const Ice::Current& = Ice::Current());
    virtual void set(::Ice::Int id,
                     ::Ice::Int type,
                     ::Ice::Int value,
                     const Ice::Current& = Ice::Current());

    virtual ::xce::userapicount::UserApiCounterPtr getAll(::Ice::Int id,
		     const Ice::Current& = Ice::Current());

    virtual void reload(::Ice::Int id,
                        const Ice::Current& = Ice::Current());
private:
	friend class MyUtil::Singleton<UserApiCountManagerI>;
	UserApiCountManagerI()	{
		_ctx.insert(make_pair("FROM_USERAPICOUNT", "1"));
	}
	UserApiCountManagerI(const UserApiCountManagerI&)  {
	}

	UserApiCounterIPtr locateCount(Ice::Int id);
	UserApiCounterIPtr findCount(Ice::Int id);
	MyUtil::Str2StrMap _ctx;
};

class CreateTask: public MyUtil::Task{
public:
        CreateTask(Ice::Int id,int level=TASK_LEVEL_CREATE);
        virtual void handle();
private:
	Ice::Int _id;
};


class UserApiCountFactory : public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

class UserApiCountResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	UserApiCountResultHandler(UserApiCounterIPtr& counter);
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	UserApiCounterIPtr& _counter;
};


}
}

#endif
