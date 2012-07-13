#include <iostream>
#include <boost/thread/mutex.hpp>

#include "base/network.h"
#include "test/stressrun.h"
#include "test/stresssuite.h"
#include "test/samples.h"

#include "dbdesc/dbdescI.h"
#include "dbdesc/dbdesc_adapter.h"

#include "arch/adapter.h"
/*
typedef xce::ClientPtr<test::ShopPrx, xce::RegistryLocator, xce::TWO_WAY> ShopAdapterPtr;

inline 
ShopAdapterPtr CreateShopAdapter() {
    using namespace xce;
    RegistryLocator r(RegistryLocator::Default());
    ShopAdapterPtr p_(r);
    bool f = p_.Create("TheShop@bsa1");
    return p_;
}

inline 
ShopAdapterPtr CachedShopAdapter() {
    using namespace xce;
    static ShopAdapterPtr p_(
        RegistryLocator(RegistryLocator::Default())
        , "TheShop@bsa1"
        );
    return p_; 
}
*/

using namespace com::xiaonei::xce;
Ice::CommunicatorPtr g_commuptr;

#if defined(XCE_STRESS_LOCAL)

#include "dbdesc/dbdescI.h"
typedef DbDescriptorI * stress_target_type;
stress_target_type StressTarget() {
  static stress_target_type p = new DbDescriptorI();
  return p;
}

#elif defined(XCE_STRESS_REMOTE)

typedef xce::ClientPtr<DbDescriptorPrx, xce::BoxLocator> stress_target_type;

stress_target_type StressTarget() {
    // xce::BoxLocator b;
    static int ii = 0;
#if 1
    static stress_target_type p = stress_target_type(xce::BoxLocator(g_commuptr), "DbDescriptor:default -h 10.2.18.183 -p 10000");
#else
    xce::BoxLocator b;
    static stress_target_type p(b);
    p.Create("TheShop:default -p 10000");
#endif
    ii++;
    if (!p) {
        assert(0);
    }

    return p;
}
#endif // XCE_STRESS_REMOTE

// 仅仅订阅需要

class DbObserverI : public DbObserver {
public:
    virtual void updated(const DbInstancePtr &instance, const ::Ice::Current& = ::Ice::Current()) {
        std::cout << instance->name << " changle" << std::endl;
    }
};
//<DbObserverPrx, DbObserverI>
//template<typename Prx, typename obs>
    // DbObserverPrx
static DbObserverPrx get_prx() {
    static DbObserverPrx prx;
    boost::mutex guard_;
    if (!prx) {
        DbObserverI *obs = new DbObserverI();

        boost::mutex::scoped_lock lock(guard_);
        if (!prx) {
            static Ice::ObjectAdapterPtr _adapter;
            _adapter = g_commuptr->createObjectAdapterWithEndpoints("test client", 
                                                            std::string("tcp -h ") + xce::Network::local_ip());
            prx = DbObserverPrx::uncheckedCast(_adapter->addWithUUID( obs));
            _adapter->activate();
        }
    }
    return prx;
}



XCE_STRESS_SUITE(dbdesc)

#if 1
XCE_STRESS_CASE(case1) {
    stress_target_type dbdesc_client = StressTarget();
    //static xce::test::Sample::uid_loop u = xce::test::Sample::RealUserId();
    dbdesc_client->reload();

    //struct a;
    // a.m1 = xce::test::xxxType();
    //shop_client->find(xce::test::UserId());
}
#endif

#if 1
XCE_STRESS_CASE(case2) {
    stress_target_type dbdesc_client = StressTarget();
    dbdesc_client->subscribe("common", get_prx());
    //dbdesc_client->reload();
    dbdesc_client->unsubscribe("common", get_prx());
}
#endif

#if 1
XCE_STRESS_CASE(case3) {
    stress_target_type dbdesc_client = StressTarget();
    DbObserverMap dmap = dbdesc_client->query("common");
    
    dbdesc_client->unsubscribe("common", get_prx());
}
#endif

XCE_STRESS_SUIT_END()

int main(int argc, char* argv[]) {
  if (!xce::test::stress_init(argc, argv))
    return 1;

  g_commuptr = Ice::initialize(argc, argv);
  stress_target_type dbdesc_client = StressTarget();  // 免得双锁
  get_prx();

  xce::test::stress_run();
  return 0;
}
