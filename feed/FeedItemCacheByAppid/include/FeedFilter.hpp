/*
 * =====================================================================================
 *
 *       Filename:  FeedFilter.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月23日 10时07分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __FEED_FILTER_HPP__
#define __FEED_FILTER_HPP__

#include "util/cpp/String.h"
#include "FeedMetaData.hpp"
#include "TypeParser.hpp"
#include "LogWrapper.h"

namespace xce {
namespace feed {

struct FriendFilter {
	std::set<int> friends;
	FriendFilter(const std::vector<int>& _friends) : friends(_friends.begin(), _friends.end()) {};

	inline bool operator()(const FeedMetaData& data){
		return friends.empty() || friends.find(data.userid) != friends.end();
	}
};

struct PassFilter {
	std::set<int> types;
	PassFilter(const std::vector<int>& _types) : types(_types.begin(), _types.end()) {};

	inline bool operator()(const FeedMetaData& data){
		MCE_DEBUG("type: " << data.type << " stype: " << WeiredType(data.type).type << " btype: " << int(WeiredType(data.type).type/100)*100);
		return types.empty() || 
			types.find(data.type) != types.end() || 
			types.find(WeiredType(data.type).type) != types.end() ||
			types.find(int(WeiredType(data.type).type/100)*100) != types.end();
	}
};

struct PreventFilter {
	std::map<int, std::set<long> > ids;
	PreventFilter(const std::vector<std::string>& _ids) {
		for (std::vector<std::string>::const_iterator it = _ids.begin();
				it != _ids.end(); ++it) {
			std::vector<string> tmp = MyUtil::StrUtil::split((*it), ":");
			if (tmp.size() == 2) {
				int type_id = boost::lexical_cast<int>(tmp[0]);
				int filter_id = boost::lexical_cast<long>(tmp[1]);
				std::map<int, std::set<long> >::iterator it = ids.find(type_id);
				if (it == ids.end()) {
					std::set<long> filter;
					ids[type_id] = filter;
				}
				ids[type_id].insert(filter_id);
			}
		}
	};

	inline bool operator()(const FeedMetaData& data){
		if (ids.empty()) return false;
		bool result = false;
		for (std::map<int, std::set<long> >::iterator it = ids.begin();
				it != ids.end(); ++it) {
				std::set<long> feedids = ids[FeedidPrefix];
				std::set<long> userids = ids[UseridPrefix];
				std::set<long> types = ids[TypePrefix];
			switch(it->first) {
				case ::xce::feed::FeedidPrefix:
					result = result || feedids.find(data.feedid) != feedids.end();
					break;
				case ::xce::feed::UseridPrefix:
					result = result || userids.find(data.userid) != userids.end();
					break;
				case ::xce::feed::TypePrefix:
					result = result || types.find(data.type) != types.end() || 
						types.find(WeiredType(data.type).type) != types.end() || 
						types.find(int(WeiredType(data.type).type/100)*100) != types.end() ||
						types.find(int(WeiredType(data.type).type/100)) != types.end();
					break;
				default:
					result = result || false;
			}
		}
	return result;
	}
};

}
}

#endif
