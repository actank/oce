#ifndef __VIP_MEMBERDATA_WRITER_ADAPTER_H__
#define __VIP_MEMBERDATA_WRITER_ADAPTER_H__

#include <ext/hash_set>
#include "VipInfo.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <IceUtil/IceUtil.h>
#include <boost/lexical_cast.hpp>

namespace xce {
namespace adapter {
namespace vipmemberdata{

const int MAX_RELOAD_ID_SIZE = 1000;

class VipMemberDataWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::VipMemberDataWriterPrx>,
	public MyUtil::Singleton<VipMemberDataWriterAdapter>, IceUtil::Thread
{
public:
	VipMemberDataWriterAdapter();

	void reload(int id);
	
	void reload(const MyUtil::LongSeq& ids);

protected:

	void run();

private:

	xce::vipinfo::VipMemberDataWriterPrx getVipMemberDataWriter(int id);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	__gnu_cxx::hash_set<int> _idSet;
};

}
}
}

#endif

