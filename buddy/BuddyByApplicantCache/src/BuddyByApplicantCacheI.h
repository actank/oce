/*
 * =====================================================================================
 *
 *       Filename:  BuddyByApplicantCacheI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年06月24日 11时15分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __BUDDY_BYAPPLICANT_CACHE_I_H__
#define __BUDDY_BYAPPLICANT_CACHE_I_H__

#include <algorithm>
#include <BuddyByApplicantCache.h>
#include <ObjectCacheI.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce {
namespace buddy {

const int BUDDY_BYAPPLICANT_DATA_I = 0;

const uint RELOAD_TIME_LIMIT = 7200;
const uint RELOAD_SIZE_LIMIT = 100;
const uint ACCEPTER_SIZE_LIMIT = 100;

class BuddyByApplicantDataI : virtual public Ice::Object {
public:
	BuddyByApplicantDataI(const BuddyByApplicantDataPtr& data);
	BuddyByApplicantDataI();
	virtual ~BuddyByApplicantDataI() {}

	BuddyByApplicantDataPtr get(int id, int begin, int limit);
       	void add(const AccepterInfo& accepter);
	void remove(int accepterId);
	void clear();

	uint getTotalCount();
	void setExtra(bool extra);
	bool hasExtra();
private:
	BuddyByApplicantDataPtr _data;
	IceUtil::Mutex _mutex;
	time_t _basicTime;
	bool _extra;
};
typedef IceUtil::Handle<BuddyByApplicantDataI> BuddyByApplicantDataIPtr;

class BuddyByApplicantCacheManagerI : virtual public BuddyByApplicantCacheManager,
	virtual public MyUtil::Singleton<BuddyByApplicantCacheManagerI> {
public:
	virtual BuddyByApplicantDataPtr getAccepters(Ice::Int, Ice::Int,
			Ice::Int, const Ice::Current& = Ice::Current());
	virtual BuddyByApplicantDataPtr getAcceptersSync(Ice::Int, Ice::Int,
			Ice::Int, const Ice::Current& = Ice::Current());
	virtual void addAccepter(Ice::Int, const AccepterInfo&,
			const Ice::Current& = Ice::Current());
	virtual void removeAccepter(Ice::Int, Ice::Int,
			const Ice::Current& = Ice::Current());
	virtual void reload(Ice::Int, const Ice::ObjectPtr&, const Ice::Current& = Ice::Current());
	virtual void clear(Ice::Int, const Ice::Current& = Ice::Current());

	virtual bool isValid(const Ice::Current&);
	virtual void setValid(bool valid,const Ice::Current& = Ice::Current());
        virtual void setData(const ::MyUtil::ObjectResultPtr& BuddyData, const Ice::Current&);
private:
	BuddyByApplicantDataIPtr locateBuddyByApplicantCache(Ice::Int);
	BuddyByApplicantDataIPtr findBuddyByApplicantCache(Ice::Int);

	void syncAddAccepter(Ice::Int, const AccepterInfo&);

	bool _isValid;
	IceUtil::RWRecMutex _validMutex;
};

class BuddyByApplicantCacheFactory : virtual public MyUtil::ServantFactory,
	virtual public MyUtil::Singleton<BuddyByApplicantCacheFactory> {
public:
	virtual Ice::ObjectPtr create(Ice::Int);
};

struct equal_accepter : public std::binary_function<AccepterInfo, AccepterInfo, bool> {
        bool operator()(const AccepterInfo& x, const AccepterInfo& y) const {
                return (x.accepter == y.accepter);
        }
};

};
};

#endif
