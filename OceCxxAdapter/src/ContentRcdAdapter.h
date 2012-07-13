#ifndef _SOCIALGRAPHCONTENTRCD_ADAPTER_H_
#define _SOCIALGRAPHCONTENTRCD_ADAPTER_H_

#include "ContentRcd.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace socialgraph {

class ContentRcdAdapter : public MyUtil::ReplicatedClusterAdapterI<ContentRcdWorkerPrx>,
	public MyUtil::Singleton<ContentRcdAdapter> {
public:
	ContentRcdAdapter():
		MyUtil::ReplicatedClusterAdapterI<ContentRcdWorkerPrx>("ControllerContentRcdWorker", 120, 500, new MyUtil::SocialGraphChannel()) {
	}
  ContentRcdSeq GetVideoRcd(int host, ContentRcd rcd);
};

}
}

#endif
