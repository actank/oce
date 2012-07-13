#ifndef BUDDYCOREADAPTER_H_
#define BUDDYCOREADAPTER_H_

#include "BuddyCore.h"
#include <vector>
#include "Singleton.h"
#include "AdapterI.h"

namespace com {
namespace xiaonei {
namespace service {
namespace buddy {

class BuddyCoreLogicAdapter :
	public MyUtil::ReplicatedClusterAdapterI<BuddyCoreLogicPrx>, public MyUtil::Singleton<BuddyCoreLogicAdapter>{
public:
	void addApply(const Applyship& apply, const Ice::Context& ctx);
	void acceptApply(const Applyship& apply, const Ice::Context& ctx);
	void denyApply(const Applyship& apply, const Ice::Context& ctx);
	void removeFriend(const Friendship& buddy, const Ice::Context& ctx);
	void addBlock(const Blockship& block, const Ice::Context& ctx);
	void removeBlock(const Blockship& block, const Ice::Context& ctx);
	void addFriend(const Ice::Int, const Ice::Int, const Ice::Context& ctx);
	void reload(const Ice::Int);

	void registerSnapshot(const BuddyCoreSnapshotPrx& proxy, const Ice::Context& ctx = Ice::Context());
	BuddyCoreSnapshotPrxSeq getSnapshots(const Ice::Context& ctx = Ice::Context());

protected:
	virtual void initialize();
	BuddyCoreLogicAdapter():
		MyUtil::ReplicatedClusterAdapterI <BuddyCoreLogicPrx>("ControllerBuddyCoreLogic", 120, 300){
	}
	friend class MyUtil::Singleton<BuddyCoreLogicAdapter>;
};

}
}
}
}

#endif /*BUDDYCOREADAPTER_H_*/
