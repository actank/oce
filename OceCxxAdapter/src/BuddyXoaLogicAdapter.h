/*
 * BuddyXoaLogicAdapter.h
 *
 *  Created on: 2011-7-25
 *      Author: xiaofeng.liang@renren-inc.com
 */

#ifndef _BUDDY_XOA_LOGIC_ADAPTER_H_
#define _BUDDY_XOA_LOGIC_ADAPTER_H_

#include "AdapterI.h"
#include "BuddyXoaLogic.h"

namespace xce {
namespace buddy{
    using namespace MyUtil;

	class BuddyXoaLogicAdapter: public AdapterI, public AdapterISingleton<MyUtil::BuddyChannel, BuddyXoaLogicAdapter>  {
	public:
		void addApply(int applicant, int apply, const Ice::Context ctx);
		void acceptApply(int applicant, int apply, const Ice::Context ctx);
		void denyApply(int applicant, int apply, const Ice::Context ctx);

		void addFriend(int from, int to, const Ice::Context ctx);
		void removeFriend(int from, int to, const Ice::Context ctx);

		void addBlock(int block, int blocked, const Ice::Context ctx);
		void removeBlock(int block, int blocked, const Ice::Context ctx);

    void reload(int userId, const Ice::Context ctx);

	protected:
		virtual string name() {
			return "BuddyXoaLogic";
		}
		virtual string endpoints() {
			return "@BuddyXoaLogic";
		}
		virtual size_t cluster() {
			return 1;
		}
		BuddyXoaLogicPrx getManagerOneway(int id);
		BuddyXoaLogicPrx getManager(int id);

	private:
		vector<BuddyXoaLogicPrx> _managersOneway;
		vector<BuddyXoaLogicPrx> _managers;
	};
}
}
#endif /* BUDDYXOALOGICADAPTER_H_ */
