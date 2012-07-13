#include "socialgraph/socialgraphd/action.h"

#include <algorithm>
#include <set>
#include <list>

#include <boost/foreach.hpp>
#include <ctemplate/template.h>

#include "base/ptime.h"
#include "base/logging.h"
#include "base/asyncall.h"
#include "base/stringdigest.h"

#include "OceCxxAdapter/src/ContentRcdAdapter.h"
#include "OceCxxAdapter/src/ActiveTrackAdapter.h"
#include "OceCxxAdapter/src/SocialGraphRcdMessageServiceAdapter.h"
#include "socialgraph/socialgraphd/socialgraphlogic_adapter.h"
#include "socialgraph/socialgraphd/headurl.h"
#include "socialgraph/socialgraphutil/algostuff.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"
#include "socialgraph/socialgraphd/tpl/rvideo.tpl.varnames.h"
#include "socialgraph/socialgraphd/tpl/rblog.tpl.varnames.h"

namespace xce {
namespace socialgraph {

cwf::HttpStatusCode BlogAction::DoPost(cwf::Request* request, cwf::Response* response, cwf::User* host) {
  std::string blogid_str = request->form("blog_id");	
	int userid = host->id();		//用户id

	long blogid = 0l;		//当前日志id
	try {
		blogid = boost::lexical_cast<long>(blogid_str);		//当前日志id
	} catch (boost::bad_lexical_cast& e) {
		LOG(LERROR) << "[BlogAction] DoPost bad_lexical_cast" << e.what();
  	return cwf::HC_OK;
	} catch (...) {
		LOG(LERROR) << "[BlogAction] DoPost unknown exception";
  	return cwf::HC_OK;
	}

  Items type_items = GetBlogDataByBlogTag(blogid);
  Items action_items = GetBlogDataByUseraction(userid);

  int type_items_size = type_items.items_size();
  int action_items_size = action_items.items_size();

	LOG(INFO)	<< "[BlogAction] DoPost blogid:" << blogid << " userid:" << userid 
		<< " type_items_size:" << type_items_size << " action_items_size:" << action_items_size;
  
  std::vector<LayoutPtr> layout_list; 
  std::set<long> fliter_blogid;			//blogid过滤器
  for (int i = 0; i < type_items_size; ++i) {			//优先将按相关度推荐的日志放入cache中
		std::string message = type_items.items(i).message();			//按相关度推荐日志:message  share_id:blog_id:weight
		std::vector<std::string> long_list;
		boost::split(long_list, message, boost::is_any_of(":"));
		if (3 != (int)long_list.size()) {
			continue;
		}

		long rec_blogid = 0l;
		try {
			rec_blogid = boost::lexical_cast<long>(long_list.at(1));
		} catch (boost::bad_lexical_cast& e) {
			LOG(LERROR) << "[BlogAction] DoPost BLOGTAG bad_lexical_cast" << e.what();
			continue;
		} catch (...) {
			LOG(LERROR) << "[BlogAction] DoPost BLOGTAG unknown exception";
			continue;
		}

    LayoutPtr layout(new BlogLayout(type_items.items(i), "BLOGTAG"));
    layout_list.push_back(layout);
    fliter_blogid.insert(rec_blogid);		//插入过滤器中
  }

  for (int i = 0; i < action_items_size; ++i) {		//需要过滤两种数据源数据
		std::string message = action_items.items(i).message();		//按用户行为推荐日志:message  share_id:blog_id
		std::vector<std::string> long_list;
		boost::split(long_list, message, boost::is_any_of(":"));
		if (2 != (int)long_list.size()) {
			continue;
		}

		long rec_blogid = 0l;
		try {
			rec_blogid = boost::lexical_cast<long>(long_list.at(1));
		} catch (boost::bad_lexical_cast& e) {
			LOG(LERROR) << "[BlogAction] DoPost BLOGACTION bad_lexical_cast" << e.what();
			continue;
		} catch (...) {
			LOG(LERROR) << "[BlogAction] DoPost BLOGACTION unknown exception";
			continue;
		}

		if (fliter_blogid.find(rec_blogid) != fliter_blogid.end()) {		//过滤重复的blogid
			continue;
		}
		LayoutPtr layout(new BlogLayout(action_items.items(i), "BLOGACTION"));
		layout_list.push_back(layout);
  }

  std::random_shuffle(layout_list.begin(), layout_list.end());
  if ((int)layout_list.size() > 2) {    //添加layout_list的size判断
    layout_list.resize(2);      //前端每次只展示2个
  }

  BOOST_FOREACH (LayoutPtr ptr, layout_list) {
    std::string out = ptr->DoLayout(NULL);
    response->WriteRaw(out);
  }
  return cwf::HC_OK;
}

xce::socialgraph::Items BlogAction::GetBlogDataByBlogTag(long blogid) {
	std::string blog_key = xce::Comb2("BLOG", blogid);  //拼接出获取blog信息的key
	RawData binary_data;
  MemcachedAdapter::GetInstance().Get2(blog_key, binary_data);  //获取该blog对应的type信息
  if (binary_data.empty()) {
    return Items();
  }

  Items blog_item;
  blog_item.ParseFromArray(binary_data.data(), binary_data.size());
  Item blog = blog_item.items(0);     //取权值type近似度最大的

  std::string type_key = xce::Comb2("BLOGTAG", blog.id());
  RawData type_data;
  MemcachedAdapter::GetInstance().Get(type_key, type_data);   //取该类tags的推荐列表
  if (type_data.empty()) {
    return Items();
  }

  Items type_item;
  type_item.ParseFromArray(type_data.data(), type_data.size());
  return type_item;
}

xce::socialgraph::Items BlogAction::GetBlogDataByUseraction(int userid) {
  std::string key = xce::Comb2("BLOGACTION", userid);  		//拼接出获取blog信息的key
  RawData action_data;
  MemcachedAdapter::GetInstance().Get(key, action_data);  	//获取该key对应的推荐数据信息
	if (action_data.empty()) {
		return Items();
	}

  Items action_item;
  action_item.ParseFromArray(action_data.data(), action_data.size());

  return action_item;
}

//-----------------------------------------------------------------------------------------------------------

cwf::HttpStatusCode VideoAction::DoPost(cwf::Request* request, cwf::Response* response, cwf::User* host) {
	int size_limit = 2;		//limit of content is 2
	std::ostringstream oss;
	oss << "VideoAction::DoPost() host:" << host->id();
	PTIME(pt, oss.str(), true, 0);
  std::string sv_url = request->form("sv_url");
  if (sv_url.empty()) {
    sv_url = request->query("sv_url");
    if (sv_url.empty()) {
      LOG(INFO) << "sv_url is empty";
      response->WriteRaw("");
      return cwf::HC_OK;
    }
  }
  const std::string md5 = xce::MD5(sv_url);
  LOG(INFO) << " sv_url:" << sv_url << "\tmd5:" << md5;
  ContentRcdSeq rcd_seq;
  try {
    ContentRcd cr;
    cr.md5 = xce::MD5(sv_url);
    cr.lisc = 2;
    rcd_seq = ContentRcdAdapter::instance().GetVideoRcd(host->id(), cr);
  } catch (Ice::Exception& e) {
    LOG(LERROR) << e.what();
  } catch (...) {
    LOG(LERROR) << "ContentRcdAdapter::instance().GetVideoRcd() ... exception" << " sv_url:" << sv_url;
  }

  if (rcd_seq.empty()) {
    LOG(INFO) << "rcd_seq is empty() host:" << host->id() << " sv_url:" << sv_url;
  }

	if ((int)rcd_seq.size() > size_limit) {		//resize if error in recommendation of 
		rcd_seq.resize(size_limit);
	}

  std::vector<LayoutPtr> layouts;
  BOOST_FOREACH(ContentRcd& rcd, rcd_seq) {
    LayoutPtr layout(new VideoLayout(rcd.thumburl, rcd.title, rcd.sid, rcd.uid));
    layouts.push_back(layout);    
  }
  BOOST_FOREACH(LayoutPtr layout, layouts) {
    std::string out = layout->DoLayout(NULL);
    response->WriteRaw(out);
  }
  return cwf::HC_OK;
}

//-----------------------------------------------------------------------------------------------------------

cwf::HttpStatusCode GetAction5::DoPost(cwf::Request* request, cwf::Response* response, cwf::User* host) {
	std::ostringstream oss;
	oss << "GetAction5::DoPost() host:" << host->id();
	PTIME(pt, oss.str(), true, 0);
  std::vector<LayoutPtr> layouts = PrepareLayout(host->id());
  std::vector<int> user_id_seq = GetUserIdSeq(&layouts);
  UserCacheMap users;
  UserCacheMap usercaches_map;
  try {
		usercaches_map = xce::usercache::UserCacheAdapter::instance().GetUserCache(user_id_seq);
    FilterUserCache(usercaches_map, users); 
  } catch (...) {
    LOG(LERROR) << "xce::usercache::UserCacheAdapter::instance().GetUserCache() exception...  host:" << host->id();
  }

  std::random_shuffle(layouts.begin(), layouts.end());

  BOOST_FOREACH(LayoutPtr layout, layouts) {
    std::string out = layout->DoLayout(&users);
    response->WriteRaw(out);
  }
	return cwf::HC_OK;
}

std::vector<LayoutPtr> GetAction5::PrepareLayout(int uid) {
  std::vector<LayoutPtr> layout_seq;
  const static std::string kRcdBiz = "RCD";
  RawData bin_data;
  bool fetch = BusinessCacheAdapter::GetInstance().Get(kRcdBiz, uid, bin_data);
  if (!fetch) {
    LOG(INFO) << "GetAction5::PrepareLayout() user_id:" << uid << " NO_RCD_DATA";
  } else {
    Items items;
    items.ParseFromArray(bin_data.data(), bin_data.size());

    BOOST_FOREACH(const Item& item, items.items()) {
			layout_seq.push_back(LayoutFactory::Create(uid, item));		//create different layout by type
    }
    LOG(INFO) << "GetAction5::PrepareLayout() user_id:" << uid << " items.size():" << items.items_size() << " data.size():" << items.ByteSize();
  }
  
  return layout_seq;
}

std::vector<int> GetAction5::GetUserIdSeq(std::vector<LayoutPtr>* layouts) {
  std::vector<int> id_seq;
  std::set<int> id_set;
  BOOST_FOREACH(LayoutPtr layout, *layouts) {
    std::set<int> t_set = layout->UserIdSet();
    id_set.insert(t_set.begin(), t_set.end());
  }
  id_seq.insert(id_seq.end(), id_set.begin(), id_set.end());
  return id_seq;
}

void GetAction5::FilterUserCache(const UserCacheMap& map1, UserCacheMap& map2) {
  UserCacheMap::const_iterator it = map1.begin();
  for (; it != map1.end(); ++it) {
    int status = it->second->status();   
    if (status != 6 && status != 7 && status != 8) {
      map2.insert(std::make_pair(it->first, it->second));
    }
  }
}

//-----------------------------------------------------------------------------------------------------------

cwf::HttpStatusCode RemoveAction5::DoPost(cwf::Request* request, cwf::Response* response, cwf::User* host) {
	std::ostringstream oss;
	oss << "RemoveAction5::DoPost() host:" << host->id();
	PTIME(pt, oss.str(), true, 0);

  const std::string biz_name = request->get("type");
  if (biz_name.empty()) {
    return cwf::HC_OK;
  }
  long guest = request->get("guest", -1);
  if (guest == -1) {
    return cwf::HC_OK;
  }

  try {
    RcdMessageServiceAdapter::instance().AddSgBlockAndRemoveRcdCache(host->id(), guest, biz_name);
  } catch (Ice::Exception& e) {
    LOG(LERROR) << e.what();
  } catch (...) {
    LOG(LERROR) << "RemoveAction5()::DoPost() .. exception host:" << host->id() << " guest:" << guest << " biz_name:" << biz_name;
  }

  return cwf::HC_OK;
}

//-----------------------------------------------------------------------------------------------------------

StatAction::StatAction() {
  using namespace com::xiaonei::service;
  log_paths_.push_back("socialgraph");
  log_paths_.push_back("reconnect");
  log_paths_.push_back("homeReconnect");
  remote_logger_ = XceLoggerAdapter::instance().getLogger();
  remote_logger_->initialize();

  xce::log::ActiveTrack::instance().initialize();
}

cwf::HttpStatusCode StatAction::DoPost(cwf::Request* request, cwf::Response* response, cwf::User* host) {
	std::ostringstream oss;
	oss << "StatAction::DoPost() host:" << host->id();
	PTIME(pt, oss.str(), true, 0);
  std::string param = request->get("param");
  if (!param.empty()) {
    remote_logger_->addLog(log_paths_, param);
    static const std::string kDesc = "no_desc";
    static const std::string kNoneIp = "0.0.0.0";
    std::ostringstream otem;
    otem << "http://" << request->host() << request->url();
    xce::log::ActiveTrack::instance().access(host->id(), kNoneIp, otem.str(), kDesc);
  }
  return cwf::HC_OK;
}

//-----------------------------------------------------------------------------------------------------------

cwf::HttpStatusCode GetOtherAction5::DoPost(cwf::Request* request, cwf::Response* response, cwf::User* host) {
	std::ostringstream oss;
	oss << "GetOtherAction5::DoPost() host:" << host->id();
	PTIME(pt, oss.str(), true, 0);
  std::vector<LayoutPtr> layouts = PrepareLayout(host->id());
  std::vector<int> user_id_seq = GetUserIdSeq(&layouts);
  UserCacheMap users;
  UserCacheMap usercaches_map;
  try {
		usercaches_map = xce::usercache::UserCacheAdapter::instance().GetUserCache(user_id_seq);
    FilterUserCache(usercaches_map, users); 
  } catch (...) {
    LOG(LERROR) << "xce::usercache::UserCacheAdapter::instance().GetUserCache() exception...  host:" << host->id();
  }

  std::random_shuffle(layouts.begin(), layouts.end());

  BOOST_FOREACH(LayoutPtr layout, layouts) {
    std::string out = layout->DoLayout(&users);
    response->WriteRaw(out);
  }
	return cwf::HC_OK;
}

std::vector<LayoutPtr> GetOtherAction5::PrepareLayout(int uid) {
  std::vector<LayoutPtr> layout_seq;
  const static std::string kRcdBiz = "RCD";
  RawData bin_data;
  bool fetch = BusinessCacheAdapter::GetInstance().Get(kRcdBiz, uid, bin_data);
  if (!fetch) {
    LOG(INFO) << "GetOtherAction5::PrepareLayout() user_id:" << uid << " NO_RCD_DATA";
  } else {
    Items items;
    items.ParseFromArray(bin_data.data(), bin_data.size());

    BOOST_FOREACH(const Item& item, items.items()) {
			if (item.message() == COMMON_FRIENDS ||				//only show common friends, cluster friends and page in other entry
					item.message() == FREINDS_CLUSTER ||
					item.message() == PAGE) {
				layout_seq.push_back(LayoutFactory::Create(uid, item));		//create different layout by type
			}
    }
    LOG(INFO) << "GetOtherAction5::PrepareLayout() user_id:" << uid << " items.size():" << items.items_size() << " data.size():" << items.ByteSize();
  }
  
  return layout_seq;
}

std::vector<int> GetOtherAction5::GetUserIdSeq(std::vector<LayoutPtr>* layouts) {
  std::vector<int> id_seq;
  std::set<int> id_set;
  BOOST_FOREACH(LayoutPtr layout, *layouts) {
    std::set<int> t_set = layout->UserIdSet();
    id_set.insert(t_set.begin(), t_set.end());
  }
  id_seq.insert(id_seq.end(), id_set.begin(), id_set.end());
  return id_seq;
}

void GetOtherAction5::FilterUserCache(const UserCacheMap& map1, UserCacheMap& map2) {
  UserCacheMap::const_iterator it = map1.begin();
  for (; it != map1.end(); ++it) {
    int status = it->second->status();   
    if (status != 6 && status != 7 && status != 8) {
      map2.insert(std::make_pair(it->first, it->second));
    }
  }
}

//-----------------------------------------------------------------------------------------------------------

cwf::HttpStatusCode RemoveOtherAction5::DoPost(cwf::Request* request, cwf::Response* response, cwf::User* host) {
	std::ostringstream oss;
	oss << "RemoveOtherAction5::DoPost() host:" << host->id();
	PTIME(pt, oss.str(), true, 0);

  const std::string biz_name = request->get("type");
  if (biz_name.empty()) {
    return cwf::HC_OK;
  }
  long guest = request->get("guest", -1);
  if (guest == -1) {
    return cwf::HC_OK;
  }

  try {
    RcdMessageServiceAdapter::instance().AddSgBlockAndRemoveRcdCache(host->id(), guest, biz_name);
  } catch (Ice::Exception& e) {
    LOG(LERROR) << e.what();
  } catch (...) {
    LOG(LERROR) << "RemoveOtherAction5()::DoPost() .. exception host:" << host->id() << " guest:" << guest << " biz_name:" << biz_name;
  }

  return cwf::HC_OK;
}

}} // end xce::socialgraph


