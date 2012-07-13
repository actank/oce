#include "VipMemberDataReaderAdapter.h"

namespace xce
{
namespace adapter
{
namespace vipmemberdata
{

VipMemberDataReaderAdapter::VipMemberDataReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::VipMemberDataReaderPrx>
														 ("ControllerVipInfoReader",120,300,new MyUtil::OceChannel,true,"VM")
{
}

xce::vipinfo::VipMemberDataReaderPrx VipMemberDataReaderAdapter::getVipMemberReader(int id)
{
	return getProxy(id);
}

void VipMemberDataReaderAdapter::setVipMemberData(int id, const MyUtil::Str2StrMap& props)
{
	if(id <= 0) {
		return;
	}
	std::vector<xce::vipinfo::VipMemberDataReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<xce::vipinfo::VipMemberDataReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setVipMemberData(id, props);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("VipMemberDataReaderAdapter::setVipMemberData Exception id = " << id << " " << e.what());
		}
	}
}

void VipMemberDataReaderAdapter::setData(int id, const Ice::ObjectPtr& data)
{
	if(!data) {
		return;
	}

	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));

	setData(res);
}

void VipMemberDataReaderAdapter::setData(const MyUtil::ObjectResultPtr& data)
{
	if(data->data.empty())
	{
		return;
	}

	std::vector<xce::vipinfo::VipMemberDataReaderPrx> allProxies = getAllProxySeq(0);
	for(std::vector<xce::vipinfo::VipMemberDataReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(data);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("VipMemberDataReaderAdapter::setData Exception id size = " << data->data.size() << " " << e.what());
		}
	}
}

}
}
}
