#include "HatDataReaderAdapter.h"

namespace xce
{
namespace adapter
{
namespace hatdata
{

HatDataReaderAdapter::HatDataReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::HatDataReaderPrx>
														 ("ControllerVipInfoReader",120,300,new MyUtil::OceChannel,true,"IH")
{
}

xce::vipinfo::HatDataReaderPrx HatDataReaderAdapter::getHatReader(int id)
{
	return getProxy(id);
}

void HatDataReaderAdapter::setHatData(int id, const MyUtil::Str2StrMap& props)
{
	if(id < 0) {
		return;
	}
	std::vector<xce::vipinfo::HatDataReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<xce::vipinfo::HatDataReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setHatData(id, props);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("HatDataReaderAdapter::setHatData Exception id = " << id << " " << e.what());
		}
	}
}

void HatDataReaderAdapter::setData(int id, const Ice::ObjectPtr& data)
{
	if(!data) {
		return;
	}

	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));

	setData(res);
}

void HatDataReaderAdapter::setData(const MyUtil::ObjectResultPtr& data)
{
	if(data->data.empty())
	{
		return;
	}

	std::vector<xce::vipinfo::HatDataReaderPrx> allProxies = getAllProxySeq(0);
	for(std::vector<xce::vipinfo::HatDataReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(data);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("HatDataReaderAdapter::setData Exception id size = " << data->data.size() << " " << e.what());
		}
	}
}

}
}
}
