/*
 * =====================================================================================
 *
 *       Filename:  BuddyByApplicantLoaderI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年06月28日 12时14分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __BUDDY_BYAPPLICANT_LOADER_I_H__
#define __BUDDY_BYAPPLICANT_LOADER_I_H__

#include <ServiceI.h>
#include <BuddyByApplicantCache.h>

namespace xce {
namespace buddy {

const int TASK_LEVEL_RELOAD = 1;

class BuddyByApplicantLoaderI : virtual public xce::buddy::BuddyByApplicantLoader, virtual public MyUtil::Singleton<BuddyByApplicantLoaderI> {
public:
	virtual void addAccepter(Ice::Int, const AccepterInfo&, const Ice::Current& = Ice::Current());
	virtual void removeAccepter(Ice::Int, Ice::Int accepterId, const Ice::Current& = Ice::Current());
	virtual void reload(Ice::Int, const Ice::Current& = Ice::Current());
	virtual void clear(Ice::Int, const Ice::Current& = Ice::Current());
};

//-----------------------------------reloadTask-----------------------------------------------
class ReloadTask : virtual public MyUtil::Task {
public:
	ReloadTask( Ice::Int id, Ice::Int level=TASK_LEVEL_RELOAD ) :
			Task(level),_id(id){
	}
protected:
        virtual void handle();
private:
	Ice::Int _id;
};
//-----------------------------------ReloadTask end-----------------------------------------------

}
}
#endif
