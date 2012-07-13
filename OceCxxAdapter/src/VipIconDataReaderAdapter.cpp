#include "VipIconDataReaderAdapter.h"

namespace xce
{
namespace adapter
{
namespace vipicondata
{

VipIconDataReaderAdapter::VipIconDataReaderAdapter() : MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::VipIconDataReaderPrx>
														 ("ControllerVipInfoReader",120,300,new MyUtil::OceChannel,true,"VI")
{
}

xce::vipinfo::VipIconDataReaderPrx VipIconDataReaderAdapter::getVipIconReader(int id)
{
	return getProxy(id);
}

std::map<int, xce::vipinfo::VipIconDataSeq> VipIconDataReaderAdapter::getVipIconDataSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types)
{
	if(ids.empty())
	{
		return std::map<int, xce::vipinfo::VipIconDataSeq>();
	}
	
	xce::vipinfo::VipIconDataReaderPrx prx = getVipIconReader(0);
	if( prx )
	{
		try
		{
			return prx->getVipIconDataSeqMap(ids, types);
		}catch(...)
		{
			MCE_WARN("VipIconDataReaderAdapter::getVipIconDataSeqMap Exception!");
		}
	}
	
	return std::map<int, xce::vipinfo::VipIconDataSeq>();
}

std::map<int, xce::vipinfo::VipFeedDataSeq> VipIconDataReaderAdapter::getVipFeedDataSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types)
{
        if(ids.empty())
        {
                return std::map<int, xce::vipinfo::VipFeedDataSeq>();
        }

        xce::vipinfo::VipIconDataReaderPrx prx = getVipIconReader(0);
        if( prx )
        {
                try
                {
                        return prx->getVipFeedDataSeqMap(ids, types);
                }catch(...)
                {
                        MCE_WARN("VipIconDataReaderAdapter::getVipFeedDataSeqMap Exception!");
                }
        }

        return std::map<int, xce::vipinfo::VipFeedDataSeq>();
}


}
}
}
