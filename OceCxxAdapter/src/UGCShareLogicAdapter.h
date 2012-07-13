#ifndef __UGCSHARELOGIC_ADAPTER_H__
#define __UGCSHARELOGIC_ADAPTER_H__

#include "AdapterI.h"
#include "UgcShare.h"

namespace com {
namespace ugcshare{
	using namespace MyUtil;
	using namespace com::xiaonei::wService::slice;
	class UGCShareLogicAdapter : public AdapterI,public AdapterISingleton<MyUtil::ImUgcChannel, UGCShareLogicAdapter> {
	public:
		void createShareToShare(Ice::Long id,int ownerid,int hostid, const string& ip, const string& comment);
                void createInternalShare(Ice::Long id, int ownerid, int type, int hostid, const string& ip, const string& comment);
                void createExternalShare(int hostid, const string& link, const string& ip, const string& comment);
		int getCluster() {
			return _cluster;
		};
	protected:
		virtual string name() {
			//return "UGCCommentLogic";
			return "ShareLogicN";
		}
		virtual string endpoints() {
			return "@ShareLogicN";
			//return "@UGCCommentLogic";
		}
		virtual size_t cluster() {
			return 1;
			//return 5;
		}
		UGCShareLogicPrx getManager(int id = 0);
		vector<UGCShareLogicPrx>  _managers;
	};

};
};

#endif
