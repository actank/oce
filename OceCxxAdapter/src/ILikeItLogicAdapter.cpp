/*
 * ILikeItLogicAdapter.cpp
 *
 *  Created on: 2009-07-06
 *      Author: zhanghan
 */

#include "OceCxxAdapter/src/ILikeItLogicAdapter.h"
#include "app/ILikeIt/share/ILikeItUtil.h"

using namespace xce::ilikeit;
using namespace xce::clusterstate;
using namespace MyUtil;

// ========= ILikeItLogicAdapter =================================
ILikeItLogicAdapter::ILikeItLogicAdapter() :
	_clientCS("ControllerILikeItLogic", 120, 300) {
	_clientCS.intialize();
}

void ILikeItLogicAdapter::addLike(const std::string& gidStr, int uid) {

	LikeshipPtr ship = new Likeship;
	ship->gid = GlobalIdTrans::Str2GlobalId(gidStr);
	ship->uid = uid;

	try {
		_clientCS.getProxy(0)->addLike(ship);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItLogicAdapter::addLike] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItLogicAdapter::addLike] std::excetpion : " << e.what()); 
	} catch (...) {
		MCE_WARN("[ILikeItLogicAdapter::addLike] exception");
	}
}

void ILikeItLogicAdapter::addLikeBatch(const std::vector<pair<std::string, int> >& shipStrSeq) {
	
	LikeshipSeq shipSeq;
	for ( size_t i = 0; i < shipStrSeq.size(); i++ ) {
		LikeshipPtr ship = new Likeship;
		ship->gid = GlobalIdTrans::Str2GlobalId(shipStrSeq[i].first);
		ship->uid = shipStrSeq[i].second;
		shipSeq.push_back(ship);
	}

	try {
		_clientCS.getProxy(0)->addLikeBatch(shipSeq);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItLogicAdapter::addLikeBatch] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItLogicAdapter::addLikeBatch] std::excetpion : " << e.what()); 
	} catch (...) {
		MCE_WARN("[ILikeItLogicAdapter::addLikeBatch] exception");
	}
}

void ILikeItLogicAdapter::removeLike(const std::string& gidStr, int uid) {
	
	LikeshipPtr ship = new Likeship;
	ship->gid = GlobalIdTrans::Str2GlobalId(gidStr);
	ship->uid = uid;
	try {
		_clientCS.getProxy(0)->removeLike(ship);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItLogicAdapter::removeLike] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItLogicAdapter::removeLike] std::excetpion : " << e.what()); 
	} catch (...) {
		MCE_WARN("[ILikeItLogicAdapter::removeLike] exception");
	}
}

void ILikeItLogicAdapter::removeLikeBatch(const std::vector<pair<std::string, int> >& shipStrSeq) {

	LikeshipSeq shipSeq;
	for ( size_t i = 0; i < shipStrSeq.size(); i++ ) {
		LikeshipPtr ship = new Likeship;
		ship->gid = GlobalIdTrans::Str2GlobalId(shipStrSeq[i].first);
		ship->uid = shipStrSeq[i].second;
		shipSeq.push_back(ship);
	}

	try {
		_clientCS.getProxy(0)->removeLikeBatch(shipSeq);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItLogicAdapter::removeLikeBatch] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItLogicAdapter::removeLikeBatch] std::excetpion : " << e.what()); 
	} catch (...) {
		MCE_WARN("[ILikeItLogicAdapter::removeLikeBatch] exception");
	}
}

LikeInfoPtr ILikeItLogicAdapter::addLikeAndGet(const std::string& gidStr, int uid) {

	LikeshipPtr ship = new Likeship;
	ship->gid = GlobalIdTrans::Str2GlobalId(gidStr);
	ship->uid = uid;
	LikeInfoPtr result = ILikeItUtil::emptyLikeInfo();
	
	MCE_DEBUG("[ILikeItLogicAdapter::addLikeAndGet] Add gid : " << gidStr 
			<< " : " << ship->gid->type << " : " << ship->gid->id);
	try {
		result = _clientCS.getProxy(0)->addLikeAndGet(ship);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItLogicAdapter::addLikeAndGet] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItLogicAdapter::addLikeAndGet] std::excetpion : " << e.what()); 
	} catch (...) {
		MCE_WARN("[ILikeItLogicAdapter::addLikeAndGet] exception");
	}
	return result;
}
LikeInfo2Ptr ILikeItLogicAdapter::addLikeAndGet2(const std::string& gidStr, int uid) {

        LikeshipPtr ship = new Likeship;
        ship->gid = GlobalIdTrans::Str2GlobalId(gidStr);
        ship->uid = uid;
        LikeInfo2Ptr result = ILikeItUtil::emptyLikeInfo2();

        MCE_DEBUG("[ILikeItLogicAdapter::addLikeAndGet] Add gid : " << gidStr
                        << " : " << ship->gid->type << " : " << ship->gid->id);
        try {
                result = _clientCS.getProxy(0)->addLikeAndGet2(ship);
        } catch (Ice::Exception& e) {
                MCE_WARN("[ILikeItLogicAdapter::addLikeAndGet] Ice::Exception : " << e.what());
        } catch (std::exception& e) {
                MCE_WARN("[ILikeItLogicAdapter::addLikeAndGet] std::excetpion : " << e.what());
        } catch (...) {
                MCE_WARN("[ILikeItLogicAdapter::addLikeAndGet] exception");
        }
        return result;
}

LikeInfoPtr ILikeItLogicAdapter::removeLikeAndGet(const std::string& gidStr, int uid) {

	LikeshipPtr ship = new Likeship;
	ship->gid = GlobalIdTrans::Str2GlobalId(gidStr);
	ship->uid = uid;
	LikeInfoPtr result = ILikeItUtil::emptyLikeInfo();
	try {
		result = _clientCS.getProxy(0)->removeLikeAndGet(ship);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ILikeItLogicAdapter::addLikeAndGet] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[ILikeItLogicAdapter::addLikeAndGet] std::excetpion : " << e.what()); 
	} catch (...) {
		MCE_WARN("[ILikeItLogicAdapter::addLikeAndGet] exception");
	}
	return result;
}
LikeInfo2Ptr ILikeItLogicAdapter::removeLikeAndGet2(const std::string& gidStr, int uid) {

        LikeshipPtr ship = new Likeship;
        ship->gid = GlobalIdTrans::Str2GlobalId(gidStr);
        ship->uid = uid;
        LikeInfo2Ptr result = ILikeItUtil::emptyLikeInfo2();
        try {
                result = _clientCS.getProxy(0)->removeLikeAndGet2(ship);
        } catch (Ice::Exception& e) {
                MCE_WARN("[ILikeItLogicAdapter::addLikeAndGet] Ice::Exception : " << e.what());
        } catch (std::exception& e) {
                MCE_WARN("[ILikeItLogicAdapter::addLikeAndGet] std::excetpion : " << e.what());
        } catch (...) {
                MCE_WARN("[ILikeItLogicAdapter::addLikeAndGet] exception");
        }
        return result;
}

