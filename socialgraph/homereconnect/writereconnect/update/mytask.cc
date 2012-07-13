#include "socialgraph/homereconnect/writereconnect/update/mytask.h"

#include "socialgraph/socialgraphutil/calculator/calculator.h"
#include "socialgraph/socialgraphutil/calculator/component/readfriendlistfromdisk.h"

namespace xce {
namespace task {

void SetDistCacheByWriteReconnectTask::handle() {
	using namespace socialgraph;
	using namespace MyUtil;
	using namespace xce::relationhelper;

	Int2IntSeqMap block_friends_map;
	RelationBatchHelper::getBlockResult(userid_, block_friends_map);
	std::vector<int> result_list;
	for (std::vector<int>::iterator iter = write_reconnect_list_.begin();
			iter != write_reconnect_list_.end(); ++iter) {
		Int2IntSeqMap::iterator fit = block_friends_map.find(*iter);
		if (fit == block_friends_map.end()) {
			result_list.push_back(*iter);	
		}
	}

	Items items;
	for (std::vector<int>::iterator iter = result_list.begin();
			iter != result_list.end(); ++iter) {
		Item* item_write_reconnect = items.add_items();
		item_write_reconnect->set_id(*iter);
	}

	std::vector<unsigned char> binary_data;
	int byte_size = items.ByteSize();
	binary_data.resize(byte_size);
	items.SerializeToArray(binary_data.data(), byte_size);

	if (binary_data.empty()) {
		return;
	}

	std::ostringstream mckey;
	mckey << "FFWR_" << userid_;
	bool insert = false;
	int insert_count = 0;
	while (!insert && ++insert_count <= 3) {
		insert = MemcachedClient::GetInstance().SetByDistCache(mckey.str(), binary_data);
	}
	//判断是否成功
	if (insert) {
		std::vector<unsigned char> tmp_data;
		MemcachedClient::GetInstance().GetByDistCache(mckey.str(), tmp_data);
		Items tmp;
		tmp.ParseFromArray(tmp_data.data(), tmp_data.size());
		LOG(INFO) << "Succes Distcache key:" << mckey.str() << " binary_data:" << binary_data.size()
				<< "	tmp_data size:" << tmp_data.size() << " item size:" << tmp.items_size();
	} else {
		LOG(INFO) << "FailDistcache key:" << mckey.str() << "  binary_date:" << binary_data.size();
	}
}

//-----------------------------------------------------------------------------------------------------------

void CreateWriteReconnectTask::handle() {
	MyUtil::Int2IntMap friend_count_map;
	xce::socialgraph::ReadFriendListFromDisk* instance = static_cast<xce::socialgraph::ReadFriendListFromDisk*>(
																															xce::socialgraph::Calculator::Get(0));
	std::vector<int> flist;
	for (std::vector<int>::iterator iter = friend_list_.begin();
			iter != friend_list_.end(); ++iter) {
		instance->GetFriendList(*iter, flist);
		if (FRIENDS_NUM_THRESHOLD <= (int)flist.size()) {
			friend_count_map.insert(std::make_pair<int, int>(*iter, (int)flist.size()));
		}
		flist.clear();
	}	

	if (!friend_count_map.empty()) {
		g_writereconnect_ptr->InsertWriteReconnectMap(userid_, friend_count_map);
	}
}

WriteReconnectItem* CreateWriteReconnectTask::g_writereconnect_ptr = NULL;

}
}

