#ifndef WRITERECONNECTWORKERADAPTER_H_
#define WRITERECONNECTWORKERADAPTER_H_

#include "Singleton.h"
#include "socialgraph/socialgraphutil/socialgraphbase.h"

namespace xce {
namespace socialgraph {

const static std::string BIZ = "FFWR";

class WriteReconnectWorkerAdapter : public SocialGraphBase,
	public MyUtil::Singleton<WriteReconnectWorkerAdapter> {
public:
	WriteReconnectWorkerAdapter() {}

	MyUtil::IntSeq getWriteRecommendRandom(int userid, int limit);
	void removeRecommendFriends(int userid, int friendid, const MyUtil::IntSeq& ids);
};

}
}

#endif
