#include "socialgraph/OfferFriends/sameareapeer/loadareapeer.h"

#include <vector>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "DbCxxPool/src/QueryRunner.h"
#include "DbCxxPool/src/ConnectionPoolManager.h"
#include "socialgraph/OfferFriends/sameareapeer/loadpeerthread.h"

namespace xce {
namespace socialgraph {

void LoadAreaPeer::Detail() {
	LOG(INFO) << "[LoadAreaPeer] Detail";
	Initialize();

	std::vector<IceUtil::ThreadControl> thread_ctrl;
	//加载高中数据 区县 ＋ 入学年
	IceUtil::Handle<LoadHighSchoolPeerThread> highschool_thread_ptr = new LoadHighSchoolPeerThread();
	thread_ctrl.push_back(highschool_thread_ptr->start());

	//加载初中数据 区县 ＋ 入学年
	IceUtil::Handle<LoadJuniorHighSchoolPeerThread> juniorhighschool_thread_ptr = new LoadJuniorHighSchoolPeerThread();
	thread_ctrl.push_back(juniorhighschool_thread_ptr->start());

	//加载中专数据 区县 ＋ 入学年
	IceUtil::Handle<LoadSpecialSchoolPeerThread> specialschool_thread_ptr = new LoadSpecialSchoolPeerThread();
	thread_ctrl.push_back(specialschool_thread_ptr->start());

	for (std::vector<IceUtil::ThreadControl>::iterator iter = thread_ctrl.begin();
			iter != thread_ctrl.end(); ++iter) {
		iter->join();
	}
}

void LoadAreaPeer::Initialize() {
	using namespace com::xiaonei::xce;
	ConnectionPoolManager::instance().initialize();
}

}
}
