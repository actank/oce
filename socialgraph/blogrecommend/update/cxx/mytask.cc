#include "socialgraph/blogrecommend/update/cxx/mytask.h"

#include "boost/lexical_cast.hpp"

#include "socialgraph/socialgraphutil/mcclient.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"
#include "socialgraph/socialgraphutil/algostuff.h"

namespace xce {
namespace socialgraph {

void SetCacheTask::handle() {
	std::string key = xce::Comb2(BIZ, blog_id_);
	Item item;
	item.set_id(blog_data_.host_id_);  //存储host_id
	item.set_field(blog_data_.score_); //存储score值

	std::ostringstream message; 
	for (std::vector< std::pair<int, int> >::iterator iter = blog_data_.tag_list_.begin();
			iter != blog_data_.tag_list_.end(); ++iter) {
		message << iter->first << ":" << iter->second << "#";
	}
	item.set_message(message.str()); //存储message信息

	std::string share_id_s = boost::lexical_cast<std::string>(blog_data_.share_id_);
	AddMessages(share_id_s, item);  //存储share_id , 1

	std::string blog_id_s = boost::lexical_cast<std::string>(blog_data_.blog_id_);
	AddMessages(blog_id_s, item); 	//存储blog_id , 2

	std::string share_u_id_s = boost::lexical_cast<std::string>(blog_data_.share_u_id_);
	AddMessages(share_u_id_s, item); 	//存储第一分享人的id , 3

	std::string time_s = boost::lexical_cast<std::string>(blog_data_.time_);
	AddMessages(time_s, item);		//存储分享时间 , 4

	int byte_size = item.ByteSize();
	std::vector<unsigned char> binary_data;
	binary_data.resize(byte_size);
	item.SerializeToArray(binary_data.data(), byte_size);

	//MemcachedAdapter::GetInstance().Set2(key, binary_data);
}

void SetCacheTask::AddMessages(std::string& value, Item& item) {
	std::string* message = item.add_messages();
	message = &value;
}

}
}
