#include "socialgraph/socialgraphd/layout.h"

#include <algorithm>
#include <set>
#include <list>

#include <boost/foreach.hpp>
#include <ctemplate/template.h>

#include "base/ptime.h"
#include "base/logging.h"
#include "base/asyncall.h"
#include "base/stringdigest.h"

#include "socialgraph/socialgraphd/headurl.h"
#include "socialgraph/socialgraphutil/algostuff.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"
#include "socialgraph/socialgraphd/tpl/rvideo.tpl.varnames.h"
#include "socialgraph/socialgraphd/tpl/rblog.tpl.varnames.h"

namespace xce {
namespace socialgraph {

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

//-----------------------------------------------------------------------------------------------------------

std::string PageLayout::DoLayout(UserCacheMap* usercaches) {
  std::string out;
  if (!usercaches)
    return out;

  if (host_ == item_data_.id()) 		//fliter host
    return out;
  
  UserCacheMap::const_iterator iter = usercaches->find(item_data_.id());
  if (iter == usercaches->end())
    return out;

  const std::string page_type = item_data_.message();
  std::string tpl_name("tpl/");
  tpl_name += page_type;
  tpl_name += ".tpl";
  std::transform(tpl_name.begin(), tpl_name.end(), tpl_name.begin(), ::tolower);

	ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tpl_name, ctemplate::STRIP_WHITESPACE);
  if (!tpl) {
    LOG(LERROR) << "can not load tpl:" << tpl_name;
    return out;
  }

  ctemplate::TemplateDictionary dict(page_type);
	dict.SetValue("TYPE_VALUE", page_type);	//type
	dict.SetIntValue("ID", iter->second->id());		//pageid
	dict.SetValue("NAME", iter->second->name());	//pagename
	dict.SetValue("TINY_URL", xce::HeadUrl::FromPart(iter->second->tinyurl()));		//pagehead

	if (item_data_.has_field())
		dict.SetIntValue("FANS", item_data_.field());		//concern

	if (0 != item_data_.messages_size())
		dict.SetValue("DESCRIPTION", item_data_.messages(0));

	dict.SetValue("HIDECOMMONS", "style=\"display:none;\"");			// whether show common friends, not show as default

	if (0 != item_data_.fields_size()) {
		int common_size = item_data_.fields(0);		//common size
		if (common_size != 0) {
			dict.SetIntValue("COMMONS", common_size);		
			dict.SetValue("HIDECOMMONS", "");			// show common friends

			for (int i = 1; i <= common_size && i <= 2 && i < item_data_.fields_size(); ++i) {		//first show 2 common friends
				UserCacheMap::const_iterator citer = usercaches->find(item_data_.fields(i));
				if (citer == usercaches->end())
					continue;

				ctemplate::TemplateDictionary* cdict = dict.AddSectionDictionary("FIRST_FIELDS");
    		cdict->ShowSection("FIRST_FIELDS");
    		cdict->SetIntValue("FIRST_FIELD_ID", citer->second->id());
				std::string name = citer->second->name();
				int name_length = name.length() > 9 ? 9 : name.length();		//restrict name length in three characters
				name.resize(name_length);
    		cdict->SetValue("FIRST_FIELD_NAME", name);
			}

			for (int i = 1; i <= common_size && i < item_data_.fields_size(); ++i) {				//common friends detail
				UserCacheMap::const_iterator citer = usercaches->find(item_data_.fields(i));
				if (citer == usercaches->end())
					continue;

				ctemplate::TemplateDictionary* cdict = dict.AddSectionDictionary("FIELDS");
    		cdict->ShowSection("FIELDS");
    		cdict->SetIntValue("FIELD_ID", citer->second->id());
    		cdict->SetValue("FIELD_NAME", citer->second->name());
			}
		}
	}
	tpl->Expand(&out, &dict);

	return out;
}

//-----------------------------------------------------------------------------------------------------------

std::string RcdBlogLayout::DoLayout(UserCacheMap* usercaches) {
  std::string out;

  const std::string blog_type = item_data_.message();
  std::string tpl_name("tpl/");
  tpl_name += blog_type;
  tpl_name += ".tpl";
  std::transform(tpl_name.begin(), tpl_name.end(), tpl_name.begin(), ::tolower);
	ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tpl_name, ctemplate::STRIP_WHITESPACE);

	if (!tpl) {
		LOG(LERROR) << "RcdBlogLayout can not load tpl " << tpl_name;
		return out;
	}

	if (item_data_.messages_size() != 3) {		//messages at 0 position stored with shareBlogId
		return "";															//messages at 1 position stored with title
	}																					//messages at 2 position stored with shareUserId 

	std::string title = item_data_.messages(1);		//title of blog
	int blog_id = item_data_.id();			//blog id
	long share_blog	= 0l;
	int share_user = 0;			//share userid
	try {
		share_blog = boost::lexical_cast<long>(item_data_.messages(0));		//messages at 0 position stored with shareBlogId
		share_user = boost::lexical_cast<int>(item_data_.messages(2));		//messages at 2 position stored with shareUserId 
	} catch (boost::bad_lexical_cast& e) {	
		LOG(LERROR) << "[RcdBlogLayout] DoLayout bad_lexical_cast" << e.what();
		return "";
	} catch (...) {
		LOG(LERROR) << "[RcdBlogLayout] DoLayout unknown exception";
		return "";
	}

	//for release
	ctemplate::TemplateDictionary dict("rcd_blog");
	dict.SetValue("TYPE_VALUE", blog_type);			//type
	dict.SetIntValue("RCD_BLOG_SHARE_USER", share_user);			//share_user
	dict.SetFormattedValue("RCD_BLOG_SHARE_ID", "%ld", share_blog);			//share_blog
	dict.SetIntValue("RCD_BLOG_ID", blog_id);		//blog_id
	dict.SetValue("RCD_BLOG_URL", GetBlogUrl(share_user, share_blog));		//share blog url
	dict.SetValue("RCD_BLOG_TITLE", title);			//title

	tpl->Expand(&out, &dict);	
	return out;
}

//-----------------------------------------------------------------------------------------------------------

std::string RcdVideoLayout::DoLayout(UserCacheMap* usercaches) {
  std::string out;

  const std::string video_type = item_data_.message();
  std::string tpl_name("tpl/");
  tpl_name += video_type;
  tpl_name += ".tpl";
  std::transform(tpl_name.begin(), tpl_name.end(), tpl_name.begin(), ::tolower);
	ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tpl_name, ctemplate::STRIP_WHITESPACE);

	if (!tpl) {
		LOG(LERROR) << "RcdVideoLayout can not load tpl " << tpl_name;
		return out;
	}

	if (item_data_.messages_size() != 3) {		//messages at 0 position stored with shareVideoId
		return "";															//messages at 1 position stored with title
	}																					//messages at 2 position stored wiht shareUserId

	std::string title = item_data_.messages(1);		//title of blog
	int video_id = item_data_.id();		//share userid
	long share_video	= 0l;
	int share_user = 0;

	try {
		share_video = boost::lexical_cast<long>(item_data_.messages(0));		//messages at 0 position stored with shareVideoId
		share_user = boost::lexical_cast<int>(item_data_.messages(2));			//messages at 2 position stored with shareUserId 
	} catch (boost::bad_lexical_cast& e) {
		LOG(LERROR) << "[RcdVideoLayout] DoLayout bad_lexical_cast" << e.what();
		return "";
	} catch (...) {
		LOG(LERROR) << "[RcdVideoLayout] DoLayout unknown exception";
		return "";
	}

	//for release
	ctemplate::TemplateDictionary dict("rcd_video");
	dict.SetValue("TYPE_VALUE", video_type);			//type
	dict.SetIntValue("RCD_VIDEO_SHARE_USER", share_user);			//share_user
	dict.SetFormattedValue("RCD_VIDEO_SHARE_ID", "%ld", share_video);			//share_video
	dict.SetIntValue("RCD_VIDEO_ID", video_id);			//video_id
	dict.SetValue("RCD_VIDEO_URL", GetShareUrl(share_user, share_video));		//share video url
	dict.SetValue("RCD_VIDEO_TITLE", title);			//title
	
	tpl->Expand(&out, &dict);	
	return out;
}

//-----------------------------------------------------------------------------------------------------------

std::string RcdSiteLayout::DoLayout(UserCacheMap* usercaches) {
  std::string out;

  UserCacheMap::const_iterator iter = usercaches->find(item_data_.id());
  if (iter == usercaches->end())	{
  	return out;
	}

	const std::string site_type = item_data_.message();			//get type of data
	std::string tpl_name("tpl/");
	tpl_name += site_type;
  tpl_name += ".tpl";
	boost::to_lower(tpl_name);
	ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tpl_name, ctemplate::STRIP_WHITESPACE);

	if (!tpl) {
		LOG(LERROR) << "RcdVideoLayout can not load tpl " << tpl_name;
		return out;
	}

	if (item_data_.messages_size() != 1) {
		LOG(LERROR) << "RcdVideoLayout can not find messages of item_data!";
		return out;
	}

	ctemplate::TemplateDictionary dict(site_type);
	dict.SetValue("URL", item_data_.messages(0));			//position 0 of messages stored with url of minisite
	dict.SetValue("TYPE_VALUE", site_type);
	dict.SetIntValue("ID", iter->second->id());
	dict.SetValue("NAME", iter->second->name());
	dict.SetValue("TINY_URL", xce::HeadUrl::FromPart(iter->second->tinyurl()));
	dict.SetIntValue("CONCERN", item_data_.field());			//concern of minisite

	tpl->Expand(&out, &dict);	
	return out;
}

//-----------------------------------------------------------------------------------------------------------

LayoutPtr LayoutFactory::Create(int uid, const Item& item_data) {
	if (COMMON_FRIENDS == item_data.message()) {			//common_friends use RcdLayout 
		return LayoutPtr(new RcdLayout(uid, item_data));
	}	else if (FREINDS_CLUSTER == item_data.message()) {		//friend_cluster use RcdLayout
		return LayoutPtr(new RcdLayout(uid, item_data));
	} else if (PAGE == item_data.message()) {						//page recommend use PageLayout
		return LayoutPtr(new PageLayout(uid, item_data));
	} else if (RBLOG == item_data.message()) {				//blog recommend use RcdBlogLayout
		return LayoutPtr(new RcdBlogLayout(uid, item_data));		
	} else if (RVIDEO == item_data.message()) {			//video recommend use RcdVideoLayout
		return LayoutPtr(new RcdVideoLayout(uid, item_data));		
	} else if (RSITE == item_data.message()) {		//site recommend use RcdSiteLayout
		return LayoutPtr(new RcdSiteLayout(uid, item_data));
	}

	return LayoutPtr(new RcdLayout(uid, item_data));		//if not found, return RcdLayout as default, such as recommend by user's info
}

}} // end xce::socialgraph
