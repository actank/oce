#include "OceCxxAdapter/src/TipsAdapter.h"
#include "ServiceI.h"

using xce::tps::TipsManagerPrx;
using xce::tps::TipsAdapter;
using xce::tps::TipsPtr;
using xce::tps::TipsSeq;

TipsPtr TipsAdapter::show(int userid, int website) {

	TipsPtr result = NULL;
	TipsManagerPrx prx = _clientCS.getProxy(userid);
	if ( prx ) {
		try {
			result = prx->show(userid,website);
		} catch (Ice::Exception& e ) {
			MCE_WARN("[TipsAdapter::show] Ice::Exception : " << e.what());
		} catch (std::exception& e ) {
			MCE_WARN("[TipsAdapter::show] std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[TipsAdapter::show] exception");
		}
	} else {
		MCE_WARN("[TipsAdapter::show] no proxy for " << userid);
	}

	return result;
}

TipsPtr TipsAdapter::get(int tipsid) {

	TipsPtr result = NULL;
	TipsManagerPrx prx = _clientCS.getProxy(tipsid); // 如果服务有多份，取哪一份都是一样的，用tipsid选只是为了把压力散开
	if ( prx ) {
		try {
			result = prx->get(tipsid);
		} catch (Ice::Exception& e ) {
			MCE_WARN("[TipsAdapter::get] Ice::Exception : " << e.what());
		} catch (std::exception& e ) {
			MCE_WARN("[TipsAdapter::get] std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[TipsAdapter::get] exception");
		}
	} else {
		MCE_WARN("[TipsAdapter::get] no proxy for " << tipsid);
	}

	return result;
}

TipsSeq TipsAdapter::list() {

	TipsSeq results;
	TipsManagerPrx prx = _clientCS.getProxy(0); // 如果服务有多份, 取哪一份都是一样的, 这个请求只有后台用,数量很少
	if ( prx ) {
		try {
			results = prx->list();
		} catch (Ice::Exception& e ) {
			MCE_WARN("[TipsAdapter::list] Ice::Exception : " << e.what());
		} catch (std::exception& e ) {
			MCE_WARN("[TipsAdapter::list] std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[TipsAdapter::list] exception");
		}
	} else {
		MCE_WARN("[TipsAdapter::list] no proxy for 0");
	}

	return results;
}

void TipsAdapter::renew(bool rebuildIndex) {
	map<int, vector<TipsManagerPrx> > prxs = _clientCS.getAvailableProxies();
	for ( map<int, vector<TipsManagerPrx> >::iterator it0 = prxs.begin();
			it0 != prxs.end(); ++it0 ) {
		for (vector<TipsManagerPrx>::iterator it1 = (*it0).second.begin();
				it1 != (*it0).second.end(); ++it1) {
			try {
				(*it1)->renew(rebuildIndex);
			} catch(Ice::Exception& e) {
				MCE_WARN("[TipsAdapter::renew] renew for " 
						<< (*it1)->ice_toString() << " Ice::Exception " << e.what());
			} catch(std::exception& e) {
				MCE_WARN("[TipsAdapter::renew] renew for " 
						<< (*it1)->ice_toString() << " std::exception " << e.what());
			} catch(...) {
				MCE_WARN("[TipsAdapter::renew] renew for " 
						<< (*it1)->ice_toString() << " exception");
			}
			MCE_INFO("[TipsAdapter::renew] renew for " << (*it1)->ice_toString());
		}
	}
}

void TipsAdapter::reloadTipsInterest(int tpsId) {
	TipsManagerPrx prx = _clientCS.getProxy(0);
	if ( prx ) {
		try {
			prx->reloadTipsInterest(tpsId, true);
		} catch (Ice::Exception& e ) {
			MCE_WARN("[TipsAdapter::reloadTipsInterest] Ice::Exception : " << e.what());
		} catch (std::exception& e ) {
			MCE_WARN("[TipsAdapter::reloadTipsInterest] std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[TipsAdapter::reloadTipsInterest] exception");
		}
	} else {
		MCE_WARN("[TipsAdapter::reloadTipsInterest] can't get TipsManagerPtr ");
	}
}

void TipsAdapter::relocateUserCache(int userid) {

	TipsManagerPrx prx = _clientCS.getProxy(userid);
	if ( prx ) {
		try {
			prx->relocateUserCache(userid);
		} catch (Ice::Exception& e ) {
			MCE_WARN("[TipsAdapter::relocateUserCache] Ice::Exception : " << e.what());
		} catch (std::exception& e ) {
			MCE_WARN("[TipsAdapter::relocateUserCache] std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[TipsAdapter::relocateUserCache] exception");
		}
	} else {
		MCE_WARN("[TipsAdapter::relocateUserCache] no proxy for " << userid);
	}
}

void TipsAdapter::click(int userid, int tipsid, bool beClose) {

	TipsManagerPrx prx = _clientCS.getProxy(userid);
	if ( prx ) {
		try {
			prx->click(userid, tipsid, beClose);
		} catch (Ice::Exception& e ) {
			MCE_WARN("[TipsAdapter::click] Ice::Exception : " << e.what());
		} catch (std::exception& e ) {
			MCE_WARN("[TipsAdapter::click] std::exception : " << e.what());
		} catch (...) {
			MCE_WARN("[TipsAdapter::click] exception");
		}
	} else {
		MCE_WARN("[TipsAdapter::click] no proxy for " << userid);
	}
}
