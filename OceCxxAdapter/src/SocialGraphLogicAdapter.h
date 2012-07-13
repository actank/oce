#ifndef _SOCIALGRAPHLOGIC_ADAPTER_H_
#define _SOCIALGRAPHLOGIC_ADAPTER_H_

#include "SocialGraphLogic.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace socialgraph {

class SocialGraphLogicAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SocialGraphLogicPrx>,
	public MyUtil::Singleton<SocialGraphLogicAdapter> {
public:
	SocialGraphLogicAdapter():
		MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SocialGraphLogicPrx>("ControllerSocialGraphLogic", 120, 1000) {
	}
	void Report(int hostId, Message msg);
};

}
}

#endif