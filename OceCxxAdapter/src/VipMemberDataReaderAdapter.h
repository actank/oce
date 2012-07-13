#ifndef __VIP_MEMBERDATA_READER_ADAPTER_H__
#define __VIP_MEMBERDATA_READER_ADAPTER_H__

#include "VipInfo.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <boost/lexical_cast.hpp>

namespace xce {
namespace adapter {
namespace vipmemberdata{

class VipMemberDataReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::VipMemberDataReaderPrx>,
								  public MyUtil::Singleton<VipMemberDataReaderAdapter>
{

public:
	
	VipMemberDataReaderAdapter();
	
	void setVipMemberData(int id, const MyUtil::Str2StrMap& props);
	void setData(int id, const Ice::ObjectPtr& data);
	void setData(const MyUtil::ObjectResultPtr& data);

private:

	xce::vipinfo::VipMemberDataReaderPrx getVipMemberReader(int id);
};

}
}
}

#endif

