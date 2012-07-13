#include "socialgraph/socialgraphutil/socialgraphbase.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "distcache/DistCacheAdapter.h"
#include "socialgraph/socialgraphutil/algostuff.h"
#include "socialgraph/socialgraphutil/mcclient.h"

namespace xce {
namespace socialgraph {

void SocialGraphBase::Report(int userid, Message msg) {
	getProxy(userid)->Report(msg);
}

bool SocialGraphBase::GetDistCacheData(const std::string& biz, int userid, std::vector<unsigned char>& value) {
	std::string key = xce::Comb2(biz, userid);
	return xce::MemcachedClient::GetInstance().GetByDistCache(key, value);
}

bool SocialGraphBase::SetDistCacheData(const std::string& biz, int userid, const std::vector<unsigned char>& value) {
	std::string key = xce::Comb2(biz, userid);
	return xce::MemcachedClient::GetInstance().SetByDistCache(key, value);
}

bool SocialGraphBase::GetDistCacheData2(const std::string& biz, long key, std::vector<unsigned char>& value) {
	std::string s_key = xce::Comb2(biz, key);
	return xce::MemcachedClient::GetInstance().GetByDistCache2(s_key, value);
}

bool SocialGraphBase::SetDistCacheData2(const std::string& biz, long key, const std::vector<unsigned char>& value) {
	std::string s_key = xce::Comb2(biz, key);
	return xce::MemcachedClient::GetInstance().SetByDistCache2(s_key, value);
}

}
}
