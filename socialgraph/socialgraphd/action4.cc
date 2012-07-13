#include "socialgraph/socialgraphd/action4.h"

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

bool ConvertPukToItems(RawData* raw_data, Items* items) {
  PukFriendRecommend pfr;
  pfr.ParseFromArray(raw_data->data(), raw_data->size());
  if (!pfr.commonfriendseq_size ()) {
    items->ParseFromArray(raw_data->data(), raw_data->size());
    return false;
  }

  if (pfr.has_timestamp()) {
    items->set_timestamp(pfr.timestamp());
  } 
 
  BOOST_FOREACH(const PukFriendRecommend_CommonFriend& pfrc, pfr.commonfriendseq()) {
    Item* item = items->add_items();
    item->set_id(pfrc.userid());
    item->set_field(pfrc.sharecount());
    BOOST_FOREACH(int i, pfrc.shares()) {
      item->add_fields(i);
    }
  }
  return true;
}

std::string ItemLayout::DoLayout(UserCacheMap* usercaches) {
  std::string out;
  if (!usercaches)
    return out;
  
  UserCacheMap::const_iterator iter = usercaches->find(item_data_.id());
  if (iter == usercaches->end())
    return out;

  std::string tpl_name("tpl/");
  tpl_name += rt_.desc_;
  tpl_name += ".tpl";
  std::transform(tpl_name.begin(), tpl_name.end(), tpl_name.begin(), ::tolower);

	ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tpl_name, ctemplate::STRIP_WHITESPACE);
  if (!tpl) {
    LOG(LERROR) << "can not load tpl:" << tpl_name;
    return out;
  }

  ctemplate::TemplateDictionary dict(rt_.desc_);
	dict.SetValue("TYPE_VALUE", rt_.desc_);
	dict.SetIntValue("ID", iter->second->id());
	dict.SetValue("NAME", iter->second->name());
	dict.SetValue("TINY_URL", xce::HeadUrl::FromPart(iter->second->tinyurl()));
	if (item_data_.has_field())
		dict.SetIntValue("FIELD", item_data_.field());
	if (item_data_.has_message())
		dict.SetValue("MESSAGE", item_data_.message());
  BOOST_FOREACH(int i, item_data_.fields()) {
    UserCacheMap::const_iterator citer = usercaches->find(i);
    if (citer == usercaches->end())
      continue;
    ctemplate::TemplateDictionary* cdict = dict.AddSectionDictionary("FIELDS");
    cdict->ShowSection("FIELDS");
    cdict->SetIntValue("FIELD_ID", citer->second->id());
    cdict->SetValue("FIELD_NAME", citer->second->name());
  }
  tpl->Expand(&out, &dict);

  return out;
}

std::string VideoLayout::DoLayout(UserCacheMap* map) {
  std::string out;
  std::string tpl_name("tpl/rvideo.tpl");
	ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tpl_name, ctemplate::STRIP_WHITESPACE);
  if (!tpl) {
    LOG(LERROR) << "can not load tpl:" << tpl_name;
    return out;
  }
  ctemplate::TemplateDictionary dict("video");
  dict.SetValue(kr_THUMB_URL, thrum_url_);
  dict.SetValue(kr_SHARE_URL, GetShareUrl());
  dict.SetValue(kr_TITLE, title_);
  tpl->Expand(&out, &dict);
  return out;
}

//----------------------------------------------------------------------------------------------------------- 
std::string BlogLayout::DoLayout(UserCacheMap* usercaches) {
	std::string out;
	std::string tpl_name("tpl/rblog.tpl");

	ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tpl_name, ctemplate::DO_NOT_STRIP);
	if (!tpl) {
		LOG(LERROR) << "BlogLayout can not load tpl " << tpl_name;
		return out;
	}
	if (3 > blog_recommend_.messages_size() ||			//分享时间time, title, body, (weight)列表
			2 != blog_recommend_.fields_size()) {		//分享人的id、count，share_user_id、count
		return "";
	}

	std::string title = blog_recommend_.messages(1);		//日志标题
	std::string desc = blog_recommend_.messages(2);			//日志概要
	int first_share_user = blog_recommend_.fields(0);		//第一分享人的id

	std::string message = blog_recommend_.message();		//多个属性拼接成的字符串 share_id:blog_id:(weight)
	std::vector<std::string> longList;
	boost::split(longList, message, boost::is_any_of(":"));
	if (2 > longList.size()) {
		return "";
	}

	long share_id = 0l;
	try {
		share_id = boost::lexical_cast<long>(longList.at(0));
	} catch (boost::bad_lexical_cast& e) {
		LOG(LERROR) << "[BlogLayout] DoLayout bad_lexical_cast" << e.what();
		return "";
	} catch (...) {
		LOG(LERROR) << "[BlogLayout] DoLayout unknown exception";
		return "";
	}

	//for release
	ctemplate::TemplateDictionary dict("blog");
	dict.SetValue(kr_BLOG_TITLE, title);
	dict.SetValue(kr_BLOG_URL, GetBlogUrl(first_share_user, share_id));
	dict.SetValue(kr_BLOG_DESC, desc);
	dict.SetValue(kr_BLOG_TYPE, type_);
	
	tpl->Expand(&out, &dict);	
	
	return out;
}

//-----------------------------------------------------------------------------------------------------------

cwf::HttpStatusCode GetAction4::DoPost(cwf::Request* request, cwf::Response* response, cwf::User* host) {
	std::ostringstream oss;
	oss << "GetAction4::DoPost() host:" << host->id();
	PTIME(pt, oss.str(), true, 0);
  ReconnectList current_rts = CurrentReconnects();
  RawDataMap raw_data_map;
  GetData(host->id(), &current_rts, &raw_data_map);
  std::vector<LayoutPtr> layouts = PrepareLayout(host->id(), &current_rts, &raw_data_map);
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

	return cwf::HC_OK; }

//-----------------------------------------------------------------------------------------------------------

void GetAction4::FilterUserCache(const UserCacheMap& map1, UserCacheMap& map2) {
  UserCacheMap::const_iterator it = map1.begin();
  for (; it != map1.end(); ++it) {
    int status = it->second->status();   
    if (status != 6 && status != 7 && status != 8) {
      map2.insert(std::make_pair(it->first, it->second));
    }
  }
}


std::vector<int> GetAction4::GetUserIdSeq(std::vector<LayoutPtr>* layouts) {
  std::vector<int> id_seq;
  std::set<int> id_set;
  BOOST_FOREACH(LayoutPtr layout, *layouts) {
    std::set<int> t_set = layout->UserIdSet();
    id_set.insert(t_set.begin(), t_set.end());
  }
  id_seq.insert(id_seq.end(), id_set.begin(), id_set.end());
  return id_seq;
}


std::vector<LayoutPtr> GetAction4::PrepareLayout(int uid, ReconnectList* rts, RawDataMap* raw_data_m) {
  using namespace xce::socialgraph;
  std::vector<LayoutPtr> layout_seq;
  BOOST_FOREACH(Reconnect& rt, *rts) {
    RawDataMap::const_iterator iter = raw_data_m->find(rt.desc_);
    if (iter == raw_data_m->end())
      continue;    
    if (rt.desc_ == "FFW") { //这个得先过滤到FFUCR推荐出来的 damn it!!!
      std::set<int> blocks;
      RawDataMap::const_iterator it = raw_data_m->find("FFUCR");
      if (it != raw_data_m->end()) {
        Items b_items; 
        b_items.ParseFromArray(it->second.data(), it->second.size());
        for (int i = 0; i < b_items.items_size(); ++i) {
          blocks.insert(b_items.items(i).id());
        }     
      }

      Items items;
      items.ParseFromArray(iter->second.data(), iter->second.size());
	    LOG(INFO) << "mckey:" << xce::Comb2(rt.desc_, uid) 
                << " items_size():"  << items.items_size() 
                << " data.size():" << iter->second.size();
      int size = std::min(items.items_size(), rt.fetch_count_);
      for (int i = 0; i < size; ++i) {
        const Item& item = items.items(i);
        if (blocks.find(item.id()) == blocks.end()) {
          LayoutPtr layout(new ItemLayout(uid, rt, item));
          layout_seq.push_back(layout);
        }
      }
    }
    else {
      Items items;
      items.ParseFromArray(iter->second.data(), iter->second.size());
	    LOG(INFO) << "mckey:" << xce::Comb2(rt.desc_, uid) 
                << " items_size():"  << items.items_size() 
                << " data.size():" << iter->second.size();
      int size = std::min(items.items_size(), rt.fetch_count_);
      for (int i = 0; i < size; ++i) {
        const Item& item = items.items(i);
        LayoutPtr layout(new ItemLayout(uid, rt, item));
        layout_seq.push_back(layout);
      }
    }
  }

  return layout_seq;
}

void GetAction4::GetData(int uid, ReconnectList* rts, RawDataMap* raw_data_m) {
  using namespace xce::socialgraph;
  BOOST_FOREACH(Reconnect& rt, *rts) {
    const std::string mckey = xce::Comb2(rt.desc_, uid);
    RawData data;
    MemcachedAdapter::GetInstance().Get(mckey, data);
    if (rt.desc_ == "FFW") {
      Items items;
      if (ConvertPukToItems(&data, &items)) {
        std::string s;
        items.SerializeToString(&s);
        data.clear();
        data.reserve(s.size());
        data.assign(s.data(), s.data() + s.size());
      }
    }
    raw_data_m->insert(std::make_pair(rt.desc_, data));
    //异步通知
    xce::Post(boost::bind(&GetAction4::SendItemMessage, this, uid, mckey, rt, data));
  }
}

void GetAction4::SendItemMessage(int host, const std::string& mckey, const Reconnect& rt, const RawData& data) {
	xce::socialgraph::Message message;
	xce::socialgraph::Info info;
	info.hostId = host;
	info.type = rt.desc_;
	info.key = mckey;
	info.operation = xce::socialgraph::UNDO;
  info.data.insert(info.data.end(), data.begin(), data.end());
	message.content.insert(std::make_pair(info.type, info));
	try {
  	xce::socialgraph::SocialGraphLogicPrx prx = xce::CachedSocialGraphLogicAdapter(host);
		prx->Report(message);
	} catch (Ice::Exception& e) {
		LOG(LERROR) << e.what();
	} catch (...) {
		LOG(LERROR) << "GetAction4::SendItemMessage() ... exception";
	}
}

cwf::HttpStatusCode RemoveAction4::DoPost(cwf::Request* request, cwf::Response* response, cwf::User* host) {
	std::ostringstream oss;
	oss << "RemoveAction4::DoPost() host:" << host->id();
	PTIME(pt, oss.str(), true, 0);
  
  const std::string biz_name = request->get("type");
  if (biz_name.empty()) {
    return cwf::HC_OK;
  }
  int guest = request->get("guest", -1);
  if (guest == -1) {
    return cwf::HC_OK;
  }
  xce::Post(boost::bind(&RemoveAction4::SendMessage, this, host->id(), guest, biz_name));
  return cwf::HC_OK;
}

void RemoveAction4::SendMessage(int host, int guest, const std::string& biz) {
	std::ostringstream oss;
	oss << "RemoveAction4::SendMessage() host:" << host << " guest:" << guest << " biz:" << biz;
	PTIME(pt, oss.str(), true, 0);
  const std::string mckey = xce::Comb2(biz, host);
  if (biz != "FFW") {
    xce::socialgraph::Message message;
    xce::socialgraph::Info info;
    info.hostId = host; 
    info.type = biz;
    info.key = mckey;
    info.operation = xce::socialgraph::DELETE;
    info.friendIds.push_back(guest);
    message.content.insert(std::make_pair(info.type, info));
    try {
	    xce::socialgraph::SocialGraphLogicPrx prx = xce::CachedSocialGraphLogicAdapter(host);
	    prx->Report(message);
    } catch (Ice::Exception& e) {
	    LOG(LERROR) << e.what();
    } catch (...) {
      LOG(LERROR) << "RemoveAction2::SendMessage() ... exception";
    }
  }
  else {  // FFW需要在web马上做remove,那就直接从cache中删除吧
   
    RawData old_raw_data;
    MemcachedAdapter::GetInstance().Get(mckey, old_raw_data);
    Items old_data;
    ConvertPukToItems(&old_raw_data, &old_data);
    Items new_data;
    BOOST_FOREACH(const Item& item, old_data.items()) {
      if (guest == item.id()) {
        continue;
      }
      Item* it = new_data.add_items();
      *it = item; 
    }
    int size = new_data.ByteSize();
    std::string s;
    new_data.SerializeToString(&s);
    RawData new_raw_data;
    new_raw_data.reserve(s.size());
    new_raw_data.assign(s.data(), s.data() + s.size());
    MemcachedAdapter::GetInstance().Set(mckey, new_raw_data);
    
    //通知
    xce::socialgraph::Message message;
    xce::socialgraph::Info info;
    info.hostId = host;
    info.type = biz;
    info.key = mckey;
    info.operation = xce::socialgraph::DELETE;
    info.friendIds.push_back(guest);
    message.content.insert(std::make_pair(info.type, info));
    try {
      xce::socialgraph::SocialGraphLogicPrx prx = xce::CachedSocialGraphLogicAdapter(host);
      prx->Report(message);
    } catch (Ice::Exception& e) {
      LOG(LERROR) << e.what();
    } catch (...) {
      LOG(LERROR) << "RemoveAction4::SendMessage() ... exception";
    }
  }
}

//-----------------------------------------------------------------------------------------------------------

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

cwf::HttpStatusCode TestAction::DoPost(cwf::Request* req, cwf::Response* res, cwf::User* host)  {
	std::ostringstream oss;
	oss << "TestAction::DoPost() host:" << host->id();
	PTIME(pt, oss.str(), true, 0);
	return cwf::HC_OK;
}

cwf::HttpStatusCode VideoAction::DoPost(cwf::Request* request, cwf::Response* response, cwf::User* host) {
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

//---------------------------
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
  }
  else {
    Items items;
    items.ParseFromArray(bin_data.data(), bin_data.size());
    BOOST_FOREACH(const Item& item, items.items()) {
      LayoutPtr layout(new RcdLayout(uid, item));
      layout_seq.push_back(layout);
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

std::string RcdLayout::DoLayout(UserCacheMap* usercaches) {
  std::string out;
  if (!usercaches)
    return out;

  if (host_ == item_data_.id()) {
    return out;
  }
  
  UserCacheMap::const_iterator iter = usercaches->find(item_data_.id());
  if (iter == usercaches->end())
    return out;

  const std::string rcd_type = item_data_.message();
  std::string tpl_name("tpl/");
  tpl_name += rcd_type;
  tpl_name += ".tpl";
  std::transform(tpl_name.begin(), tpl_name.end(), tpl_name.begin(), ::tolower);

	ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tpl_name, ctemplate::STRIP_WHITESPACE);
  if (!tpl) {
    LOG(LERROR) << "can not load tpl:" << tpl_name;
    return out;
  }

  ctemplate::TemplateDictionary dict(rcd_type);
	dict.SetValue("TYPE_VALUE", rcd_type);
	dict.SetIntValue("ID", iter->second->id());
	dict.SetValue("NAME", iter->second->name());
	dict.SetValue("TINY_URL", xce::HeadUrl::FromPart(iter->second->tinyurl()));
	if (item_data_.has_field())
		dict.SetIntValue("FIELD", item_data_.field());
	if (item_data_.has_message())
		dict.SetValue("MESSAGE", item_data_.message());
  BOOST_FOREACH(int i, item_data_.fields()) {
    UserCacheMap::const_iterator citer = usercaches->find(i);
    if (citer == usercaches->end())
      continue;
    ctemplate::TemplateDictionary* cdict = dict.AddSectionDictionary("FIELDS");
    cdict->ShowSection("FIELDS");
    cdict->SetIntValue("FIELD_ID", citer->second->id());
    cdict->SetValue("FIELD_NAME", citer->second->name());
  }
  tpl->Expand(&out, &dict);

  return out;
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


cwf::HttpStatusCode RemoveAction5::DoPost(cwf::Request* request, cwf::Response* response, cwf::User* host) {
	std::ostringstream oss;
	oss << "RemoveAction5::DoPost() host:" << host->id();
	PTIME(pt, oss.str(), true, 0);

  const std::string biz_name = request->get("type");
  if (biz_name.empty()) {
    return cwf::HC_OK;
  }
  int guest = request->get("guest", -1);
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


//-----------

StatAction::StatAction() {
  using namespace com::xiaonei::service;
  log_paths_.push_back("socialgraph");
  log_paths_.push_back("reconnect");
  log_paths_.push_back("homeReconnect");
  remote_logger_ = XceLoggerAdapter::instance().getLogger();
  remote_logger_->initialize();

  // 
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

}} // end xce::socialgraph


