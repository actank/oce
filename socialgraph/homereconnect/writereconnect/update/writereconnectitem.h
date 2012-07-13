#include <iostream>
#include <map>
#include <vector>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

class WriteReconnectItem {
public:
	WriteReconnectItem() {}
	void InsertWriteReconnectMap(int userid, const std::map<int, int>& friend_count_map) {
		IceUtil::RWRecMutex::WLock lock(init_flag_mutex_);		
		for (std::map<int, int>::const_iterator iter = friend_count_map.begin();
				iter != friend_count_map.end(); ++iter) {
			write_reconnect_map_[iter->first].push_back(userid);
		}
	}

	void GetWriteReconnectMap(std::map<int, std::vector<int> >& write_reconnect_map) {
		IceUtil::RWRecMutex::WLock lock(init_flag_mutex_);		
		write_reconnect_map.swap(write_reconnect_map_);
	}

private:
	std::map<int, std::vector<int> > write_reconnect_map_;
	IceUtil::RWRecMutex init_flag_mutex_;
};
