#ifndef __TIPS_ADMIN_I_H__
#define __TIPS_ADMIN_I_H__

#include "Tips.h"
#include "IceExt/src/ServiceI.h"
#include "UtilCxx/src/Singleton.h"

namespace xce {
namespace tps {

/* 提示主类 */
class TipsAdminI : virtual public TipsAdmin,
	public MyUtil::Singleton<TipsAdminI>
{
public:
    virtual void add(const ::xce::tps::TipsPtr& tps, const Ice::Current&);

    virtual void remove(::Ice::Int tipsId, const Ice::Current&);

    virtual void modify(::Ice::Int tipsId, const ::xce::tps::TipsPtr& tps, const Ice::Current&);

    virtual void online(::Ice::Int tips, const Ice::Current&);

    virtual void offline(::Ice::Int tips, const Ice::Current&);

    void initialize();

private:
    friend class MyUtil::Singleton<TipsAdminI>;
    TipsAdminI() {
    };
};

}
}

#endif
