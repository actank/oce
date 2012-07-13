/*
 * FeedNewsAdapter.h
 *
 *  Created on: Oct 15, 2009
 *      Author: antonio
 */

#ifndef FEEDNEWSREPLICAADAPTER_H_
#define FEEDNEWSREPLICAADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedFocusInvert.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedFocusInvertReplicaAdapter : public MyUtil::ReplicatedClusterAdapterI<FeedFocusInvertPrx>,
  public MyUtil::Singleton<FeedFocusInvertReplicaAdapter>{

public:
	FeedFocusInvertReplicaAdapter() : MyUtil::ReplicatedClusterAdapterI<FeedFocusInvertPrx> ("ControllerFeedFocusInvertR",120,300,new XceFeedChannel){
	}
	IntSeq GetSources(int target){
		if(target < 0){
			MCE_WARN("FeedFocusInvertAdapter::GetSources. wrong param.  target:" << target);
			return IntSeq();
		}
		return getProxy(target)->GetSources(target);
	}
	void AddFocus(int source,int target){
		if(source < 0 || target < 0){
			MCE_WARN("FeedFocusInvertAdapter::AddFocus. wrong param. source:" << source << " target:" << target);
			return;
		}
		getProxy(target)->AddFocus(source,target);
	}
	void DelFocus(int source,int target){
		if(source < 0 || target < 0){
			MCE_WARN("FeedFocusInvertAdapter::DelFocus. wrong param. source:" << source << " target:" << target);
			return;
		}
		getProxy(target)->DelFocus(source,target);
	}


};

}
;
}
;
#endif /* FEEDITEMADAPTER_H_ */


