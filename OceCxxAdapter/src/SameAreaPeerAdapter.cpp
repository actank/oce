#include "OceCxxAdapter/src/SameAreaPeerAdapter.h"

#include "item.pb.h"

namespace xce {
namespace socialgraph {

std::vector<PItem> SameAreaPeerAdapter::getAreaPeer(long key, int limit) {
	std::vector<PItem> recommend_list;
	std::vector<unsigned char> value;
	GetDistCacheData2(BIZ, key, value);

	if (!value.empty()) {
		Items items;
		items.ParseFromArray(value.data(), value.size());
		for (::google::protobuf::RepeatedPtrField< ::xce::socialgraph::Item >::const_iterator iter = items.items().begin();
				iter != items.items().end(); ++iter) {
			PItem pe;
			pe.uid_ = iter->id();
			pe.gender_ = iter->field();

			recommend_list.push_back(pe);
		}
	}

	if (limit > 0 && (int)recommend_list.size() > limit) {
		recommend_list.resize(limit);
	}

	return recommend_list;
}

}
}
