#ifndef __VIP_ICON_DATA_READER_ADAPTER_H__
#define __VIP_ICON_DATA_READER_ADAPTER_H__

#include "VipInfo.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <boost/lexical_cast.hpp>

namespace xce {
namespace adapter {
namespace vipicondata{

enum VIP_TYPE
{
	VIP = 0,
	DISNEY,
	GIFT,
	WIRELESS
};

class VipIconDataReaderAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::vipinfo::VipIconDataReaderPrx>,
								  public MyUtil::Singleton<VipIconDataReaderAdapter>
{

public:
	
	VipIconDataReaderAdapter();

	/*
	 * ids : 用户id
	 * types : 需要取的用户vip类型，见enum VIP_TYPE
	 * 
	 * return : 返回用户的信息，map的key为用户id，value为要取的类型对应的数据的sequence
	 */
	std::map<int, xce::vipinfo::VipIconDataSeq> getVipIconDataSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types);

        /*
         * ids : 用户id
         * types : 需要取的用户vip类型，见enum VIP_TYPE
         * 
         * return : 返回用户的信息，map的key为用户id，value为要取的类型对应的数据的sequence
         */
	std::map<int, xce::vipinfo::VipFeedDataSeq> getVipFeedDataSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types);

private:

	xce::vipinfo::VipIconDataReaderPrx getVipIconReader(int id);
};

}
}
}

#endif

