#ifndef _SOCIALGRAPHBASE_H_
#define _SOCIALGRAPHBASE_H_

#include <string>
#include <map>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "SocialGraphLogic.h"
#include "Channel.h"
#include "AdapterI.h"

namespace xce {
namespace socialgraph {

class SocialGraphBase : public MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SocialGraphLogicPrx> {
public:
	SocialGraphBase():
		MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SocialGraphLogicPrx>("ControllerSocialGraphLogic", 120, 1000) {
	}	

	void Report(int userid, Message msg);
	bool GetDistCacheData(const std::string& biz, int userid, std::vector<unsigned char>& value);
	bool SetDistCacheData(const std::string& biz, int userid, const std::vector<unsigned char>& value);

	bool GetDistCacheData2(const std::string& biz, long key, std::vector<unsigned char>& value);
	bool SetDistCacheData2(const std::string& biz, long key, const std::vector<unsigned char>& value);
};

}
}

#endif
