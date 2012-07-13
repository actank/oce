/*
 * BulletinAdapter.cpp
 *
 *  Created on: 2009-11-02
 *      Author: zhanghan
 */

#include "OceCxxAdapter/src/BulletinAdapter.h"
//#include "ServiceI.h"

using xce::blt::BulletinManagerPrx;
using xce::blt::BulletinAdapter;
using xce::blt::BulletinPtr;
using xce::blt::BulletinSeq;

BulletinPtr BulletinAdapter::show(int userid, int website) {

	BulletinPtr result = NULL;
	BulletinManagerPrx prx = _clientCS.getProxy(userid);
	if ( prx ) {
		try {
			result = prx->show(userid, website);
		} catch (Ice::Exception& e ) {
			MCE_WARN("[BulletinAdapter::show] Ice::Exception : " << e.what());
		} catch (std::exception& e ) {
			MCE_WARN("[BulletinAdapter::show] std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[BulletinAdapter::show] exception");
		}
	} else {
		MCE_WARN("[BulletinAdapter::show] no proxy for " << userid);
	}

	return result;
}

BulletinPtr BulletinAdapter::get(int bulletinid) {

	BulletinPtr result = NULL;
	BulletinManagerPrx prx = _clientCS.getProxy(bulletinid); // 如果服务有多份，取哪一份都是一样的，用bulletinid选只是为了把压力散开
	if ( prx ) {
		try {
			result = prx->get(bulletinid);
		} catch (Ice::Exception& e ) {
			MCE_WARN("[BulletinAdapter::get] Ice::Exception : " << e.what());
		} catch (std::exception& e ) {
			MCE_WARN("[BulletinAdapter::get] std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[BulletinAdapter::get] exception");
		}
	} else {
		MCE_WARN("[BulletinAdapter::get] no proxy for " << bulletinid);
	}

	return result;
}

BulletinSeq BulletinAdapter::list() {

	BulletinSeq results;
	BulletinManagerPrx prx = _clientCS.getProxy(0); // 如果服务有多份, 取哪一份都是一样的, 这个请求只有后台用,数量很少
	if ( prx ) {
		try {
			results = prx->list();
		} catch (Ice::Exception& e ) {
			MCE_WARN("[BulletinAdapter::list] Ice::Exception : " << e.what());
		} catch (std::exception& e ) {
			MCE_WARN("[BulletinAdapter::list] std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[BulletinAdapter::list] exception");
		}
	} else {
		MCE_WARN("[BulletinAdapter::list] no proxy for 0");
	}

	return results;
}

void BulletinAdapter::renew(bool rebuildIndex) {
	map<int, vector<BulletinManagerPrx> > prxs = _clientCS.getAvailableProxies();
	for ( map<int, vector<BulletinManagerPrx> >::iterator it0 = prxs.begin();
			it0 != prxs.end(); ++it0 ) {
		for (vector<BulletinManagerPrx>::iterator it1 = (*it0).second.begin();
				it1 != (*it0).second.end(); ++it1) {
			try {
				(*it1)->renew(rebuildIndex);
			} catch(Ice::Exception& e) {
				MCE_WARN("[BulletinAdapter::renew] renew for " 
						<< (*it1)->ice_toString() << " Ice::Exception " << e.what());
			} catch(std::exception& e) {
				MCE_WARN("[BulletinAdapter::renew] renew for " 
						<< (*it1)->ice_toString() << " std::exception " << e.what());
			} catch(...) {
				MCE_WARN("[BulletinAdapter::renew] renew for " 
						<< (*it1)->ice_toString() << " exception");
			}
			MCE_INFO("[BulletinAdapter::renew] renew for " << (*it1)->ice_toString());
		}
	}
}

void BulletinAdapter::reloadBulletinInterest(int bltId) {
	BulletinManagerPrx prx = _clientCS.getProxy(0);
	if ( prx ) {
		try {
			prx->reloadBulletinInterest(bltId, true);
		} catch (Ice::Exception& e ) {
			MCE_WARN("[BulletinAdapter::reloadBulletinInterest] Ice::Exception : " << e.what());
		} catch (std::exception& e ) {
			MCE_WARN("[BulletinAdapter::reloadBulletinInterest] std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[BulletinAdapter::reloadBulletinInterest] exception");
		}
	} else {
		MCE_WARN("[BulletinAdapter::reloadBulletinInterest] can't get BulletinManagerPtr ");
	}
}

void BulletinAdapter::relocateUserCache(int userid) {

	BulletinManagerPrx prx = _clientCS.getProxy(userid);
	if ( prx ) {
		try {
			prx->relocateUserCache(userid);
		} catch (Ice::Exception& e ) {
			MCE_WARN("[BulletinAdapter::relocateUserCache] Ice::Exception : " << e.what());
		} catch (std::exception& e ) {
			MCE_WARN("[BulletinAdapter::relocateUserCache] std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[BulletinAdapter::relocateUserCache] exception");
		}
	} else {
		MCE_WARN("[BulletinAdapter::relocateUserCache] no proxy for " << userid);
	}
}

void BulletinAdapter::click(int userid, int bulletinid, bool beClose) {

	BulletinManagerPrx prx = _clientCS.getProxy(userid);
	if ( prx ) {
		try {
			prx->click(userid, bulletinid, beClose);
		} catch (Ice::Exception& e ) {
			MCE_WARN("[BulletinAdapter::click] Ice::Exception : " << e.what());
		} catch (std::exception& e ) {
			MCE_WARN("[BulletinAdapter::click] std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[BulletinAdapter::click] exception");
		}
	} else {
		MCE_WARN("[BulletinAdapter::click] no proxy for " << userid);
	}
}
