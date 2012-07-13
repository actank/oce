#include "Icon2UrlDataReaderAdapter.h"

namespace xce
{
namespace adapter
{
namespace icon2urldata
{

Icon2UrlDataReaderAdapter::Icon2UrlDataReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::Icon2UrlDataReaderPrx>
														 ("ControllerVipInfoReader",120,300,new MyUtil::OceChannel,true,"IU")
{
}

xce::vipinfo::Icon2UrlDataReaderPrx Icon2UrlDataReaderAdapter::getIcon2UrlReader(int id)
{
	return getProxy(id);
}

void Icon2UrlDataReaderAdapter::setIcon2UrlData(int id, const MyUtil::Str2StrMap& props)
{
	if(id < 0) {
		return;
	}
	std::vector<xce::vipinfo::Icon2UrlDataReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<xce::vipinfo::Icon2UrlDataReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setIcon2UrlData(id, props);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("Icon2UrlDataReaderAdapter::setIcon2UrlData Exception id = " << id << " " << e.what());
		}
	}
}

void Icon2UrlDataReaderAdapter::setData(int id, const Ice::ObjectPtr& data)
{
	if(!data) {
		return;
	}

	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));

	setData(res);
}

void Icon2UrlDataReaderAdapter::setData(const MyUtil::ObjectResultPtr& data)
{
	if(data->data.empty())
	{
		return;
	}

	std::vector<xce::vipinfo::Icon2UrlDataReaderPrx> allProxies = getAllProxySeq(0);
	for(std::vector<xce::vipinfo::Icon2UrlDataReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(data);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("Icon2UrlDataReaderAdapter::setData Exception id size = " << data->data.size() << " " << e.what());
		}
	}
}

}
}
}
