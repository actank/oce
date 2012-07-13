#ifndef __BULLETIN_ADMIN_I_H__
#define __BULLETIN_ADMIN_I_H__

#include "Bulletin.h"
#include "IceExt/src/ServiceI.h"
#include "UtilCxx/src/Singleton.h"

namespace xce {
namespace blt {

/* 公告主类 */
class BulletinAdminI : virtual public BulletinAdmin,
	public MyUtil::Singleton<BulletinAdminI>
{
public:
    virtual void add(const ::xce::blt::BulletinPtr& blt, const Ice::Current&);

    virtual void remove(::Ice::Int bulletinId, const Ice::Current&);

    virtual void modify(::Ice::Int bulletinId, const ::xce::blt::BulletinPtr& blt, const Ice::Current&);

    virtual void online(::Ice::Int bulletin, const Ice::Current&);

    virtual void offline(::Ice::Int bulletin, const Ice::Current&);

    void initialize();

private:
    friend class MyUtil::Singleton<BulletinAdminI>;
    BulletinAdminI() {
    };
};

}
}

#endif
