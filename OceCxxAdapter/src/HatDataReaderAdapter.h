#ifndef __HATDATA_READER_ADAPTER_H__
#define __HATDATA_READER_ADAPTER_H__

#include "VipInfo.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <boost/lexical_cast.hpp>

namespace xce {
namespace adapter {
namespace hatdata{

class HatDataReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::HatDataReaderPrx>,
								  public MyUtil::Singleton<HatDataReaderAdapter>
{

public:
	
	HatDataReaderAdapter();
	
	void setHatData(int id, const MyUtil::Str2StrMap& props);
	void setData(int id, const Ice::ObjectPtr& data);
	void setData(const MyUtil::ObjectResultPtr& data);

private:

	xce::vipinfo::HatDataReaderPrx getHatReader(int id);
};

}
}
}

#endif

