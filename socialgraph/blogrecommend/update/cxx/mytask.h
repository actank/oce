#ifndef MYTASK_H_
#define MYTASK_H_

#include "UtilCxx/src/TaskManager.h"
#include "socialgraph/blogrecommend/update/cxx/common.h"
#include "item.pb.h"

namespace xce {
namespace socialgraph {

const static int SET_CACHE_LEVEL = 5;
const static std::string BIZ = "BLOG";

//--------------------------------------------------

class SetCacheTask : public MyUtil::Task {
public:
	SetCacheTask(long blog_id, BlogData blog_data, int level = SET_CACHE_LEVEL) : 
		MyUtil::Task(level), blog_id_(blog_id), blog_data_(blog_data) {
	}

	virtual void handle();

private:
	void AddMessages(std::string& value, Item& item);

	long blog_id_;
	BlogData blog_data_;
};

}
}

#endif
