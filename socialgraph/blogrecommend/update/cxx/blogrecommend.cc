#include "socialgraph/blogrecommend/update/cxx/blogrecommend.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sstream>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "socialgraph/socialgraphutil/algostuff.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"
#include "socialgraph/blogrecommend/update/cxx/mytask.h"
#include "socialgraph/blogrecommend/update/cxx/sort_strategy.h"
#include "socialgraph/blogrecommend/update/cxx/strategy/sort_by_tagweight.h"

namespace xce {
namespace socialgraph {

void BlogRecommend::Detail() {
	SGTIME(t, "");

	//----------------Load Blog data from file-----------------//
	LoadBlogData("/data/xce/Project/blogrecommend/source");
	SGTIME_CHECK(t, "LoadBlogData");
	////for (std::map<long, BlogData>::iterator iter = blog_map_.begin();
	////		iter != blog_map_.end(); ++iter) {
	////	LOG(INFO) << iter->first << ":";
	////	LOG(INFO) << iter->second.share_id_ << ":" << iter->second.blog_id_ << ":" << iter->second.host_id_
	////		<< ":" << iter->second.share_u_id_ << ":" << iter->second.time_ << ":" << iter->second.score_;
	////	for (std::vector< std::pair<int, int> >::iterator it = iter->second.tag_list_.begin();
	////			it != iter->second.tag_list_.end(); ++it) {
	////		LOG(INFO) << it->first << " : " << it->second;
	////	}
	////}

	//----------------Create tag map-----------------//
	CreateTagMap("/data/xce/Project/blogrecommend/source");
	SGTIME_CHECK(t, "CreateTagMap");
	LOG(INFO) << "t_map_ size : " << t_map_.size();
	PrintTagMap(t_map_);

	//----------------Sort tag map-----------------//
	SortTagMap();
	PrintTagMap(t_map_);
	////----------------Set Blog Data-----------------//
	//SetBlogData();
	//SGTIME_CHECK(t, "Set Blog Data");

	////----------------Set Blog Tag-----------------//
	//SetBlogTag();
	//SGTIME_CHECK(t, "Set Blog Tag");
}

void BlogRecommend::LoadBlogData(const std::string& file) {
	int item_count = 9;
	std::string line = "";
	std::ifstream infile(file.c_str(), std::ifstream::in); 

	while (getline(infile, line)) {
		try {
			std::vector<std::string> content_list;
			boost::algorithm::split(content_list, line, boost::algorithm::is_any_of("	"));
			LOG(INFO) << "content_list size : " << content_list.size();
			if ((int)content_list.size() < item_count) {
				continue;
			}
			//拼接出Blog数据
			//long share_id = boost::lexical_cast<long>(content_list.at(0));		//分享id
			long blog_id = boost::lexical_cast<long>(content_list.at(1));			//blog id
			//int host_id = boost::lexical_cast<int>(content_list.at(2)); 		//该blog的拥有者id
			std::string share_time = content_list.at(3);				//分享时间
			//int share_count = boost::lexical_cast<int>(content_list.at(4));				//被分享的次数
			//int first_share_id = boost::lexical_cast<int>(content_list.at(5));			//第一个分享该日志的用户
			std::string title = content_list.at(6);						//日志的title
			std::string content = content_list.at(7);				//日志内容
			std::string detail = content_list.at(8);			//日志的type列表

			::xce::socialgraph::Items blog_data;
			std::vector<std::string> type_list;
			boost::algorithm::split(type_list, detail, boost::algorithm::is_any_of(";"));
			for (std::vector<std::string>::iterator iter = type_list.begin();
					iter != type_list.end(); ++iter) {
				std::vector<std::string> weight_list;
				boost::algorithm::split(weight_list, *iter, boost::algorithm::is_any_of(","));
				if (weight_list.size() != 2) {
					continue;
				}
				int type = boost::lexical_cast<int>(weight_list.at(0));
				std::string weight = weight_list.at(1);

				::xce::socialgraph::Item* blog_item = blog_data.add_items();
				blog_item->set_id(type);    
				blog_item->set_message(weight);
			}
			std::vector<unsigned char> binary_data;
			int byte_size = blog_data.ByteSize();
			binary_data.resize(byte_size);	
			blog_data.SerializeToArray(binary_data.data(), byte_size);

			if (!binary_data.empty()) {
				std::string key = xce::Comb2(BLOG, blog_id);	
				LOG(INFO) << "key:" << key << " blog_id:" << blog_id << " detail:" << detail;
				Print(blog_data);
				//MemcachedAdapter::GetInstance().Set2(key, binary_data);
			}
		} catch (boost::bad_lexical_cast& e) {
			LOG(LERROR) << "boost bad_lexical_cast : " << e.what();
		} catch (...) {
			LOG(LERROR) << "unknow error ";
		}
	}
}

void BlogRecommend::CreateTagMap(const std::string& file) {
	int item_count = 9;
	std::string line = "";
	std::ifstream infile(file.c_str(), std::ifstream::in); 

	while (getline(infile, line)) {
		try {
			std::vector<std::string> content_list;
			boost::algorithm::split(content_list, line, boost::algorithm::is_any_of("	"));
			LOG(INFO) << "content_list size : " << content_list.size();
			if ((int)content_list.size() < item_count) {
				continue;
			}
			//拼接出Blog数据
			long share_id = boost::lexical_cast<long>(content_list.at(0));		//分享id
			long blog_id = boost::lexical_cast<long>(content_list.at(1));			//blog id
			int host_id = boost::lexical_cast<int>(content_list.at(2)); 		//该blog的拥有者id
			std::string share_time = content_list.at(3);				//分享时间
			int share_count = boost::lexical_cast<int>(content_list.at(4));				//被分享的次数
			int first_share_id = boost::lexical_cast<int>(content_list.at(5));			//第一个分享该日志的用户
			std::string title = content_list.at(6);						//日志的title
			std::string content = content_list.at(7);				//日志内容
			std::string detail = content_list.at(8);			//日志的type列表
			
			Blog blog;
			blog.share_id_ = share_id;
			blog.blog_id_ = blog_id;
			blog.host_id_ = host_id;
			blog.count_ = share_count;
			blog.first_share_id_ = first_share_id;
			blog.title_ = title;
			blog.content_ = content;
			blog.weight_str_ = detail;

			std::vector<std::string> time_list;
			boost::algorithm::split(time_list, share_time, boost::algorithm::is_any_of(" "));
			if (time_list.size() != 2) {
				continue;
			}
			blog.time_ = FormatTime(time_list.at(0), time_list.at(1));

			std::vector<std::string> type_list;
			boost::algorithm::split(type_list, detail, boost::algorithm::is_any_of(";"));
			for (std::vector<std::string>::iterator iter = type_list.begin();
					iter != type_list.end(); ++iter) {
				std::vector<std::string> weight_list;
				boost::algorithm::split(weight_list, *iter, boost::algorithm::is_any_of(","));
				if (weight_list.size() != 2) {
					continue;
				}
				int type = boost::lexical_cast<int>(weight_list.at(0));
				std::string weight = weight_list.at(1);
				blog.weight_ = boost::lexical_cast<float>(weight);
				t_map_[type].push_back(blog);
			}
		} catch (boost::bad_lexical_cast& e) {
			LOG(LERROR) << "boost bad_lexical_cast : " << e.what();
		} catch (...) {
			LOG(LERROR) << "unknow error ";
		}
	}
}

long BlogRecommend::FormatTime(const std::string& date, const std::string& clock) {
	//LOG(INFO) << date << " : " << clock;
	struct tm local_time;
	std::vector<std::string> date_vec; //处理年月日，2010-10-22
	boost::algorithm::split(date_vec, date, boost::algorithm::is_any_of("-"));
	if ((int)date_vec.size() != 3)
		return 0;
	local_time.tm_year = boost::lexical_cast<int>(date_vec[0].c_str()) - 1900;
	local_time.tm_mon = boost::lexical_cast<int>(date_vec[1].c_str()) - 1;
	local_time.tm_mday = boost::lexical_cast<int>(date_vec[2].c_str());
	
	std::vector<std::string> clock_vec; //处理时分秒, 13:55:00
	boost::algorithm::split(clock_vec, clock, boost::algorithm::is_any_of(":"));
	if ((int)clock_vec.size() != 3) 
		return 0;
	local_time.tm_hour = boost::lexical_cast<int>(clock_vec[0].c_str());
	local_time.tm_min = boost::lexical_cast<int>(clock_vec[1].c_str());
	local_time.tm_sec = boost::lexical_cast<int>(clock_vec[2].c_str());

	//LOG(INFO) << local_time.tm_year << ":" << local_time.tm_mon << ":" << local_time.tm_mday
	//			<< ":" << local_time.tm_hour << ":" << local_time.tm_min << ":" << local_time.tm_sec;
	return (long)mktime(&local_time);
}

void BlogRecommend::SortTagMap() {
	SortStrategy* obj = new SortByTagweight();
	obj->NewSort(t_map_);	

	delete obj;
}

void BlogRecommend::SetBlogData() {
	using namespace MyUtil;
	TaskManager::instance().config(SET_CACHE_LEVEL, ThreadPoolConfig(0, 100));
	int count = 0;
	int wait = 0;
	for (std::map<long, BlogData>::iterator iter = blog_map_.begin();
			iter != blog_map_.end(); ++iter) {
		if (++count % 1000 == 0) {
			LOG(INFO) << "count : " << count;
		}
		TaskManager::instance().execute(new SetCacheTask(iter->first, iter->second));
		while (TaskManager::instance().size(SET_CACHE_LEVEL) > 10000) {
			wait += 2;
			sleep(2);
		}
	}

	while (TaskManager::instance().size(SET_CACHE_LEVEL) > 0) {
		sleep(2);
	}
}

void BlogRecommend::SetBlogTag() {
	for (std::map<int, std::vector<long> >::iterator iter = tag_map_.begin();
			iter != tag_map_.end(); ++iter) {
		Item item;
		item.set_id(iter->first);
		for (std::vector<long>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			std::string recommend_blog = boost::lexical_cast<std::string>(*it);
			std::string* message = item.add_messages();
			message = &recommend_blog;
		}
		std::string key = xce::Comb2(TAG, iter->first);
		
		int byte_size = item.ByteSize();
		std::vector<unsigned char> binary_data;
		binary_data.resize(byte_size);
		item.SerializeToArray(binary_data.data(), byte_size);

		//MemcachedAdapter::GetInstance().Set2(key, binary_data);
	}	
}

void BlogRecommend::Print(const ::xce::socialgraph::Items& target) {
	for (::google::protobuf::RepeatedPtrField< ::xce::socialgraph::Item >::const_iterator iter = target.items().begin();
			iter != target.items().end(); ++iter) {
		LOG(INFO) << "id : " << iter->id() << " message :" << iter->message();
	}
	LOG(INFO) << "";
}

void BlogRecommend::PrintTagMap(const std::map<int, std::vector<Blog> >& t_map) {
	for (std::map<int, std::vector<Blog> >::const_iterator iter = t_map.begin();
			iter != t_map.end(); ++iter) {
		LOG(INFO) << "key:" << iter->first;
		for (std::vector<Blog>::const_iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			LOG(INFO) << it->share_id_ << ":" << it->blog_id_ << ":" << it->host_id_ << ":"
								<< it->time_ << ":" << it->count_ << ":" << it->first_share_id_ << ":"
								<< it->weight_ << ":" << it->title_ << ":" << it->content_ << ":"
								<< it->weight_str_;
		}
		LOG(INFO) << "";
	}
}

}
}
