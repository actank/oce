#include "OceCxxAdapter/src/WriteReconnectWorkerAdapter.h"

#include "item.pb.h"

namespace xce {
namespace socialgraph {

MyUtil::IntSeq WriteReconnectWorkerAdapter::getWriteRecommendRandom(int userid, int limit) {
	MyUtil::IntSeq friend_list;
	std::vector<unsigned char> value;
	GetDistCacheData(BIZ, userid, value);
	
	if (!value.empty()) {
		Items items;
		items.ParseFromArray(value.data(), value.size());
		for (::google::protobuf::RepeatedPtrField< ::xce::socialgraph::Item >::const_iterator iter = items.items().begin();
				iter != items.items().end(); ++iter) {
			friend_list.push_back(iter->id());
		}
	}

	if (limit > 0 && (int)friend_list.size() > limit) {
		friend_list.resize(limit);
	}

	return friend_list;
}

void WriteReconnectWorkerAdapter::removeRecommendFriends(int userid, int friendid, const MyUtil::IntSeq& ids) {
	Info info;
	info.hostId = userid;
	info.type = "FFWR";
	info.operation = DELETE;
	info.friendIds.push_back(friendid);

	Message msg;
	msg.content.insert(std::make_pair<std::string, Info>(BIZ, info));
	Report(userid, msg);
}

}
}
