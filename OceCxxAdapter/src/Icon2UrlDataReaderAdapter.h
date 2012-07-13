#ifndef __ICON2URLDATA_READER_ADAPTER_H__
#define __ICON2URLDATA_READER_ADAPTER_H__

#include "VipInfo.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <boost/lexical_cast.hpp>

namespace xce {
namespace adapter {
namespace icon2urldata{

class Icon2UrlDataReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::Icon2UrlDataReaderPrx>,
								  public MyUtil::Singleton<Icon2UrlDataReaderAdapter>
{

public:
	
	Icon2UrlDataReaderAdapter();
	
	void setIcon2UrlData(int id, const MyUtil::Str2StrMap& props);
	void setData(int id, const Ice::ObjectPtr& data);
	void setData(const MyUtil::ObjectResultPtr& data);

private:

	xce::vipinfo::Icon2UrlDataReaderPrx getIcon2UrlReader(int id);
};

}
}
}

#endif

