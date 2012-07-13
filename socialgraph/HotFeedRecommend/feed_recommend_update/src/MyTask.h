#ifndef MY_TASK_H_
#define MY_TASK_H_

#include <HotFeedRecommendCacheAdapter.h>

#include "TaskManager.h"
#include "UGCItem.h"

#include "HotFeedUpdateDBHelper.h"
#include "recommend_interest_model.h"
#include "ugc_model.h"

extern RawUgcManager* g_ugc_manager;

namespace xce {
namespace task {

const int TASK_LEVEL_SET_CACHE = 1;

//---------------------------------------------------------

struct UgcComp {
  bool operator() (const item::UGCItem& u1, const item::UGCItem& u2) {
    return u1.getUGCWeight() > u2.getUGCWeight();
  }
};        

//-----------------------------------------------------------------------------------------------------------

class LoadHotFeedRecommendTask : public MyUtil::Task {
public:
	LoadHotFeedRecommendTask(const RecommendInterestModel& user_model, ::Ice::Int level = TASK_LEVEL_SET_CACHE):
		MyUtil::Task(TASK_LEVEL_SET_CACHE), user_model_(user_model) { }

	virtual void handle();

private:
	item::UGCItem GetMostWeigheUGC(int hostid, int friendid);
	const RecommendInterestModel user_model_;
};

}
}

#endif
