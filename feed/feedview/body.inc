#define MAKE_VALUE(f, count) sec->SetValue( f, fx->find( f, count))

// 默认的 fill 动作, 如果table.inc 中没有对应的函数, 则使用该函数
bool fill_default(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_del_no_assign_typeText
  //MCE_INFO(WeiredType(feed->type).type);
  if (dict_delete_) 
    dict_delete_->ShowSection("typeText_NOT");
  return true; 
}

bool fill_group_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  
  const std::vector<std::string> & entry_id_v = fx->find_v("f_entry_id");

  if (entry_id_v.size() > 1) {
    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("single_blog_NOT");
    int count = -1;
    const std::vector<std::string> & url_v = fx->find_v("f_entry_url");
    const std::vector<std::string> & title_v = fx->find_v("f_entry_title");
    const std::vector<std::string> & owner_name_v = fx->find_v("f_entry_owner_name");
    if (url_v.size() != entry_id_v.size() || title_v.size() != entry_id_v.size() || owner_name_v.size() != entry_id_v.size()) {
      MCE_WARN("body.inc::fill_group_blog_publish --> vector error!");
      return false;
    }


    int blog_no = entry_id_v.size();
    sec->SetIntValue("blog_no", blog_no);
    
    BOOST_FOREACH(const std::string& r, url_v) {
      if (++count == 5)
        break;

      ctemplate::TemplateDictionary* blog_sec = sec->AddSectionDictionary("blog_list");
      blog_sec->SetValue("blog_url", r);
      //MAKE_VALUE("blog_title", count);
      blog_sec->SetValue("blog_title", title_v[count]);
      blog_sec->SetValue("f_entry_owner_name", owner_name_v[count]);
      
    }
    

  } else {

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("single_blog");

    int count = -1;
    const std::vector<std::string> & tag_title_v = fx->find_v("f_entry_tags_tag_title");
    const std::vector<std::string> & tag_url_v = fx->find_v("f_entry_tags_tag_url");

    if (tag_title_v.size() != tag_url_v.size()) {
      MCE_WARN("body.inc::fill_group_blog_publish --> tag size error!");
      return false;
    }


    BOOST_FOREACH(const std::string& r, tag_title_v) {
      if (++count == 4)
        break;

      ctemplate::TemplateDictionary* tag_sec = sec->AddSectionDictionary("tags");
      tag_sec->SetValue("f_entry_tags_tag_title", r);
      MAKE_VALUE("f_entry_tags_tag_url", count);

    }

    IfElse(fx, sec, "f_from_url",NULL);
    IfElse(fx, sec, "f_from_anonymous",NULL);
    IfElse(fx, sec, "f_from_certify_icon",NULL);
    IfElse(fx, sec, "f_entry_prefixUrl",NULL);
    IfElse(fx, sec, "f_entry_owner_name",NULL);
    IfElse(fx, sec, "f_entry_owner2_name",NULL);
    IfElse(fx, sec, "f_entry_url",NULL);
    IfElse(fx, sec, "f_entry_complementUrl",NULL);
    IfElse(fx, sec, "f_entry_bodyUrl",NULL);
    IfElse(fx, sec, "f_entry_attachments_attachment_src",NULL);
    IfElse(fx, sec, "f_entry_comment",NULL);
    IfElse(fx, sec, "f_entry_actions_action_title",NULL);

    // write_reply_loop
    count = -1;
    const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
    BOOST_FOREACH(const std::string& r, reply_v) {
      if (++count == 2)
        break;

      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
      sec->SetValue("f_reply_id", r);
      MAKE_VALUE("f_reply_from_tinyimg", count);
      MAKE_VALUE("f_reply_body", count);
      MAKE_VALUE("f_reply_from_name", count);
      MAKE_VALUE("f_reply_time", count);
      MAKE_VALUE("f_reply_type", count);
      MAKE_VALUE("f_reply_from_id", count);

    }
  }

  if (dict_delete_) 
    dict_delete_->ShowSection("typeText_NOT");
  return true; 
	
}
bool fill_general(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  //MCE_INFO( feed->xml);
  
  int count = -1;
  const std::vector<std::string> & tag_title_v = fx->find_v("f_entry_tags_tag_title");
  const std::vector<std::string> & tag_url_v = fx->find_v("f_entry_tags_tag_url");

  if (tag_title_v.size() != tag_url_v.size()) {
    MCE_WARN("body.inc::fill_general --> tag size error!");
    return false;
  }


  BOOST_FOREACH(const std::string& r, tag_title_v) {
    if (++count == 4)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("tags");
    sec->SetValue("f_entry_tags_tag_title", r);
    MAKE_VALUE("f_entry_tags_tag_url", count);
    
  }

	IfElse(fx, dict, "f_from_url",NULL);
	IfElse(fx, dict, "f_from_anonymous",NULL);
	IfElse(fx, dict, "f_from_certify_icon",NULL);
	IfElse(fx, dict, "f_entry_prefixUrl",NULL);
	IfElse(fx, dict, "f_entry_owner_name",NULL);
	IfElse(fx, dict, "f_entry_owner2_name",NULL);
	IfElse(fx, dict, "f_entry_url",NULL);
	IfElse(fx, dict, "f_entry_complementUrl",NULL);
	IfElse(fx, dict, "f_entry_bodyUrl",NULL);
	IfElse(fx, dict, "f_entry_attachments_attachment_src",NULL);
	IfElse(fx, dict, "f_entry_comment",NULL);
	IfElse(fx, dict, "f_entry_actions_action_title",NULL);

  if (dict_delete_) 
    dict_delete_->ShowSection("typeText_NOT");
  return true; 
	
}
bool fill_share_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_user_image","f_share_user_image");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_mini_site_blog(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  
  If(fx, dict, "f_share_comment","f_share_comment");
  If(fx, dict, "f_from_certify_icon",NULL);
 // IfElse(fx, dict, "selfzancount",NULL);
  
  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"个日志分享");
  return true;
}
bool fill_share_blog(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  
  If(fx, dict, "f_share_comment","f_share_comment");
  If(fx, dict, "f_from_certify_icon",NULL);
  IfElse(fx, dict, "selfzancount",NULL);

  if(!((fx->find("userVipIconId")).empty())) {
    dict->ShowSection("userVipIconId");
  } 
 
  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"个日志分享");
  return true;
}
bool fill_page_share_blog(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);

  // write_assign_share_blog_url
  std::string ss = fx->find("f_share_blog_url");
  if (ss.find('?') != std::string::npos)
    dict->SetValue("symbol", "&");
  else
    dict->SetValue("symbol", "?");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_mini_site_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  //MCE_INFO("site photo:" << feed->xml);
  If(fx, dict, "f_share_album_photo_image","f_share_album_photo_image");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  If(fx, dict, "f_from_certify_icon",NULL);
  If(fx, dict, "f_share_album_photo_digest","f_share_album_photo_digest");
  IfElse(fx, dict, "f_share_album_origin_url","f_share_album_origin_url");
  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);
  

  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"个照片分享");
  return true;
}
bool fill_share_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_album_photo_image","f_share_album_photo_image");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  If(fx, dict, "f_from_certify_icon",NULL);
  If(fx, dict, "f_share_album_photo_digest","f_share_album_photo_digest");
  IfElse(fx, dict, "f_share_album_origin_url","f_share_album_origin_url");
  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);
  
  if(!((fx->find("userVipIconId")).empty())) {
    dict->ShowSection("userVipIconId");  
  }

  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"个照片分享");
  return true;
}
bool fill_page_share_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_album_photo_image","f_share_album_photo_image");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_album(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_album_image","f_share_album_image");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_share_album_title","f_share_album_title");
  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"个相册分享");
  return true;
}
bool fill_share_thread(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_theme(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_mini_site_share_link(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  int cnt = -1;
  const std::vector<std::string> & tags_v = fx->find_v("f_site_tag_url");
  BOOST_FOREACH(const std::string& r, tags_v) {
    if (++cnt == 3)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("tags");
    sec->SetValue("f_site_tag_url", r);
    MAKE_VALUE("f_site_tag_name", cnt);

  }

  IfElse(fx, dict, "f_share_shared_link_title","f_share_shared_link_title");
  If(fx, dict, "f_share_shared_link_image","f_share_shared_link_image");
  If(fx, dict, "f_share_shared_link_body","f_share_shared_link_body");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"个链接分享");
  return true;
}
bool fill_share_mini_site_link(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  IfElse(fx, dict, "f_share_shared_link_title","f_share_shared_link_title");
  If(fx, dict, "f_share_shared_link_image","f_share_shared_link_image");
  If(fx, dict, "f_share_shared_link_body","f_share_shared_link_body");
  IfElse(fx, dict, "selfzancount",NULL);
  

  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"个链接分享");
  return true;
}
bool fill_share_link(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  IfElse(fx, dict, "f_share_shared_link_title","f_share_shared_link_title");
  If(fx, dict, "f_share_shared_link_image","f_share_shared_link_image");
  If(fx, dict, "f_share_shared_link_body","f_share_shared_link_body");
  IfElse(fx, dict, "selfzancount",NULL);
  

  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"个链接分享");
  return true;
}
bool fill_page_share_link(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  IfElse(fx, dict, "f_share_shared_link_title","f_share_shared_link_title");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_market(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_market_image","f_share_market_image");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_movie(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_movie_image","f_share_movie_image");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_mini_site_share_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  int cnt = -1;
  const std::vector<std::string> & tags_v = fx->find_v("f_site_tag_url");
  BOOST_FOREACH(const std::string& r, tags_v) {
    if (++cnt == 3)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("tags");
    sec->SetValue("f_site_tag_url", r);
    MAKE_VALUE("f_site_tag_name", cnt);

  }

  std::vector<std::string> titlev = fx->find_v("f_share_id");
  int video_count = titlev.size();
  if (1 == video_count) {
    ctemplate::TemplateDictionary* dict2 = dict->AddSectionDictionary("multi_video_NOT");
    int count = 0;
    const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
    BOOST_FOREACH(const std::string& r, reply_v) {
      ctemplate::TemplateDictionary* sec = dict2->AddSectionDictionary("f_reply_id");
      sec->SetValue("f_reply_id", r);
      MAKE_VALUE("f_reply_from_tinyimg", count);
      MAKE_VALUE("f_reply_body", count);
      MAKE_VALUE("f_reply_from_name", count);
      MAKE_VALUE("f_reply_time", count);
      MAKE_VALUE("f_reply_type", count);
      MAKE_VALUE("f_reply_from_id", count);

      if (++count >= 2) {
        break;
      }
    }
	If(fx, dict, "f_from_certify_icon",NULL);
    IfElse(fx, dict2, "selfzancount",NULL);
    // write_assign_key
    if (dict_delete_)
      dict_delete_->SetValue("typeText" ,"个视频分享");
    return true;
  } 


 
  ctemplate::TemplateDictionary* dict2 = dict->AddSectionDictionary("multi_video"); 
  int video_limit = 0;
  BOOST_FOREACH(const std::string& share_id, titlev) {
    ctemplate::TemplateDictionary* sec = dict2->AddSectionDictionary("video_num");

    MAKE_VALUE("f_share_video_title", video_limit);
    MAKE_VALUE("f_share_id", video_limit);
    MAKE_VALUE("f_share_video_image", video_limit);
    MAKE_VALUE("f_share_video_short_title", video_limit);
    
    
    if (video_count > 3 && 2 == video_limit) {
      sec->ShowSection("more_video_button");
    }

    if (++video_limit >= 6) {
      break;
    } 
  }
 
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");  
  return true;
}

bool fill_share_mini_site_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  std::vector<std::string> titlev = fx->find_v("f_share_id");
  int video_count = titlev.size();


  if (1 == video_count) {
    ctemplate::TemplateDictionary* dict2 = dict->AddSectionDictionary("multi_video_NOT");
    int count = 0;
    const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
    BOOST_FOREACH(const std::string& r, reply_v) {
      ctemplate::TemplateDictionary* sec = dict2->AddSectionDictionary("f_reply_id");
      sec->SetValue("f_reply_id", r);
      MAKE_VALUE("f_reply_from_tinyimg", count);
      MAKE_VALUE("f_reply_body", count);
      MAKE_VALUE("f_reply_from_name", count);
      MAKE_VALUE("f_reply_time", count);
      MAKE_VALUE("f_reply_type", count);
      MAKE_VALUE("f_reply_from_id", count);

      if (++count >= 2) {
        break;
      }
    }
	  If(fx, dict, "f_from_certify_icon",NULL);
    IfElse(fx, dict2, "selfzancount",NULL);

    
    // write_assign_key
    if (dict_delete_)
      dict_delete_->SetValue("typeText" ,"个视频分享");
    return true;
  } 

  ctemplate::TemplateDictionary* dict2 = dict->AddSectionDictionary("multi_video"); 
  int video_limit = 0;
  BOOST_FOREACH(const std::string& share_id, titlev) {
    ctemplate::TemplateDictionary* sec = dict2->AddSectionDictionary("video_num");

    MAKE_VALUE("f_share_video_title", video_limit);
    MAKE_VALUE("f_share_id", video_limit);
    MAKE_VALUE("f_share_video_image", video_limit);
    MAKE_VALUE("f_share_video_short_title", video_limit);
    
    
    if (video_count > 3 && 2 == video_limit) {
      sec->ShowSection("more_video_button");
    }

    if (++video_limit >= 6) {
      break;
    } 
  }
 
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");  
  return true;
}


bool fill_share_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  std::vector<std::string> titlev = fx->find_v("f_share_id");
  int video_count = titlev.size();


  if (1 == video_count) {
    ctemplate::TemplateDictionary* dict2 = dict->AddSectionDictionary("multi_video_NOT");
    int count = 0;
    const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
    BOOST_FOREACH(const std::string& r, reply_v) {
      ctemplate::TemplateDictionary* sec = dict2->AddSectionDictionary("f_reply_id");
      sec->SetValue("f_reply_id", r);
      MAKE_VALUE("f_reply_from_tinyimg", count);
      MAKE_VALUE("f_reply_body", count);
      MAKE_VALUE("f_reply_from_name", count);
      MAKE_VALUE("f_reply_time", count);
      MAKE_VALUE("f_reply_type", count);
      MAKE_VALUE("f_reply_from_id", count);

      if (++count >= 2) {
        break;
      }
    }
	  If(fx, dict, "f_from_certify_icon",NULL);
    IfElse(fx, dict2, "selfzancount",NULL);

    ctemplate::TemplateDictionary* on_sec = dict2;
    std::string on_value = fx->find("f_share_owner_name");
    if (!on_value.empty()) {
      on_sec = dict2->AddSectionDictionary("f_share_owner_name");
    }
    // write_assign_key
    if (dict_delete_)
      dict_delete_->SetValue("typeText" ,"个视频新鲜事");
    return true;
  } 

  ctemplate::TemplateDictionary* dict2 = dict->AddSectionDictionary("multi_video"); 
  int video_limit = 0;
  BOOST_FOREACH(const std::string& share_id, titlev) {
    ctemplate::TemplateDictionary* sec = dict2->AddSectionDictionary("video_num");

    MAKE_VALUE("f_share_video_title", video_limit);
    MAKE_VALUE("f_share_id", video_limit);
    MAKE_VALUE("f_share_video_image", video_limit);
    MAKE_VALUE("f_share_video_short_title", video_limit);
    
    
    if (video_count > 3 && 2 == video_limit) {
      sec->ShowSection("more_video_button");
    }

    if (++video_limit >= 6) {
      break;
    } 
  }
 
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");  
  return true;
}


bool fill_page_share_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_mini_site_share_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

 int cnt = -1;
  const std::vector<std::string> & tags_v = fx->find_v("f_site_tag_url");
  BOOST_FOREACH(const std::string& r, tags_v) {
    if (++cnt == 3)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("tags");
    sec->SetValue("f_site_tag_url", r);
    MAKE_VALUE("f_site_tag_name", cnt);

  }

  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_mini_site_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  //IfElse(fx, dict, "selfzancount",NULL);
  

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  IfElse(fx, dict, "selfzancount",NULL);
  

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_page_share_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_activity(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_activity_image","f_share_activity_image");

  {
  // write_replace ,请手工消除重复代码
  std::string k = fx->find("f_share_activity_url");
  boost::replace_all(k, "xiaonei.com", "renren.com");
  dict->SetValue("f_share_activity_url", k);
  }

  {
  // write_replace ,请手工消除重复代码
  std::string k = fx->find("f_share_activity_url");
  boost::replace_all(k, "xiaonei.com", "renren.com");
  dict->SetValue("f_share_activity_url", k);
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_share_activity_comment","f_share_activity_comment");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_app(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_app_image","f_share_app_image");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_qa(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_connect_graph(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_video_b(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_video_comment(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_share_video_like(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_edm_edm_text(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_edm_logo","f_edm_logo");
  IfElse(fx, dict, "f_edm_page_action_url", NULL);
  If(fx, dict, "f_stat_id", NULL);
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_edm_ad","f_edm_ad");
  IfElse(fx, dict, "f_edm_resource",NULL);
  // write_IfWithValue
  {
  std::string value = fx->find("f_edm_interactive");
  if (!value.empty() && value == "0")
    dict->ShowSection("f_edm_interactive0");
  else
    dict->ShowSection("f_edm_interactive0_NOT");
  }
  IfElse(fx, dict, "selfzancount",NULL);

  // write_edm_interactive_url
  if (fx->find("f_edm_action_name") != "" && fx->find("f_edm_action_url") != "")
    dict->ShowSection("EDM_NAME_URL");
  IfElse(fx, dict, "f_edm_user_id","f_edm_user_id");
  IfElse(fx, dict, "totalzancount","totalzancount");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_edm_edm_game(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_edm_logo","f_edm_logo");

  int cnt = -1;
  const std::vector<std::string> & friend_id_v = fx->find_v("f_fl_friend_id");
  const std::vector<std::string> & friend_name_v = fx->find_v("f_fl_friend_name");
  const std::vector<std::string> & friend_src_v = fx->find_v("f_fl_friend_src");
  std::vector<std::string>::size_type id_vec_size = friend_id_v.size();
  std::vector<std::string>::size_type name_vec_size = friend_name_v.size();
  std::vector<std::string>::size_type src_vec_size = friend_src_v.size();


  if (id_vec_size != name_vec_size) {
    MCE_WARN( "body.inc --> fill_edm_edm_game --> id_vec  and  name_vec size error!");
    return false;
  }
  
  if (!src_vec_size && id_vec_size != src_vec_size) {
    MCE_WARN( "body.inc --> fill_edm_edm_game --> src_vec size error!");
    return false;
  }

  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  BOOST_FOREACH(const std::string& i, friend_id_v) {
    if (++cnt == 4)
      break;
    
    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("friends_list");
    sec->SetValue("f_fl_friend_id", i);
    MAKE_VALUE("f_fl_friend_name", cnt);

    if(!strVipIconId.empty() && i == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
    }
  } 

  dict->SetIntValue("fl_count", friend_id_v.size());

  cnt = -1;
  BOOST_FOREACH(const std::string& s, friend_src_v) {
    if (++cnt == 7)
      break;
    
    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("friends_pic");
    sec->SetValue("f_fl_friend_src", s);
    MAKE_VALUE("f_fl_friend_id", cnt);
    MAKE_VALUE("f_fl_friend_name", cnt);
  } 

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_edm_ad","f_edm_ad");
  IfElse(fx, dict, "f_edm_resource",NULL);
  // write_IfWithValue
  {
  std::string value = fx->find("f_edm_interactive");
  if (!value.empty() && value == "0")
    dict->ShowSection("f_edm_interactive0");
  else
    dict->ShowSection("f_edm_interactive0_NOT");
  }
  IfElse(fx, dict, "selfzancount",NULL);

  // write_edm_interactive_url
  if (fx->find("f_edm_action_name") != "" && fx->find("f_edm_action_url") != "")
    dict->ShowSection("EDM_NAME_URL");
  IfElse(fx, dict, "f_edm_user_id","f_edm_user_id");
  IfElse(fx, dict, "totalzancount","totalzancount");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_edm_edm_pic(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_edm_logo","f_edm_logo");
  If(fx, dict, "f_stat_id", NULL);



  IfElse(fx, dict, "f_edm_page_action_url", NULL);
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_edm_ad","f_edm_ad");
  IfElse(fx, dict, "f_edm_resource",NULL);
  // write_IfWithValue
  {
  std::string value = fx->find("f_edm_interactive");
  if (!value.empty() && value == "0")
    dict->ShowSection("f_edm_interactive0");
  else
    dict->ShowSection("f_edm_interactive0_NOT");
  }
  IfElse(fx, dict, "selfzancount",NULL);

  // write_edm_interactive_url
  if (fx->find("f_edm_action_name") != "" && fx->find("f_edm_action_url") != "")
    dict->ShowSection("EDM_NAME_URL");
  IfElse(fx, dict, "f_edm_user_id","f_edm_user_id");
  IfElse(fx, dict, "totalzancount","totalzancount");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_edm_edm_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_edm_logo","f_edm_logo");
  If(fx, dict, "f_stat_id", NULL);
  
  IfElse(fx, dict, "f_edm_page_action_url", NULL);
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_edm_ad","f_edm_ad");
  IfElse(fx, dict, "f_edm_resource",NULL);
  // write_IfWithValue
  {
  std::string value = fx->find("f_edm_interactive");
  if (!value.empty() && value == "0")
    dict->ShowSection("f_edm_interactive0");
  else
    dict->ShowSection("f_edm_interactive0_NOT");
  }
  IfElse(fx, dict, "selfzancount",NULL);

  // write_edm_interactive_url
  if (fx->find("f_edm_action_name") != "" && fx->find("f_edm_action_url") != "")
    dict->ShowSection("EDM_NAME_URL");
  IfElse(fx, dict, "f_edm_user_id","f_edm_user_id");
  IfElse(fx, dict, "totalzancount","totalzancount");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_event_share_event(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_album_photo_image","f_share_album_photo_image");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_event_share_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_album_photo_image","f_share_album_photo_image");
  If(fx, dict, "f_share_album_photo_digest","f_share_album_photo_digest");
  IfElse(fx, dict, "f_share_album_origin_url",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
/*
bool fill_group_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
*/
bool fill_group_publish_thread(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_group_forum_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_tieba_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_tieba_create(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_tieba_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_act_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    
      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }      
    }
    idcount++;    
  }
  IfElse(fx, dict, "f_commerce_user_url","f_commerce_user_url");
  If(fx, dict, "f_commerce_image","f_commerce_image");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_head_upload(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_userinfo_image","f_userinfo_image");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_mini_site_status_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  If(fx,dict, "f_status_re_title", NULL);

  std::string lbs_id = fx->find("f_lbs_id");
  if (!lbs_id.empty()){
     dict->ShowSection("lbs");
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  int cnt = -1;
  const std::vector<std::string> & tags_v = fx->find_v("f_site_tag_url");
  BOOST_FOREACH(const std::string& r, tags_v) {
    if (++cnt == 3)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("tags");
    sec->SetValue("f_site_tag_url", r);
    MAKE_VALUE("f_site_tag_name", cnt);
    
  }

  // write_status_publish_elseif
  std::string newfeed = fx->find("isNewsFeed");
  std::string userID = fx->find("userID");
  std::string interviewee = fx->find("interviewee"); 
  if (!newfeed.empty())
    dict->ShowSection("isNewsFeed");
  else if (userID != interviewee)
    dict->ShowSection("isNewsFeedELIF");
  IfElse(fx, dict, "f_reply_owner",NULL);
  If(fx, dict, "isNewsFeed",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"条状态");
  return true;
}
bool fill_status_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  If(fx,dict, "f_status_re_title", NULL);

  std::string lbs_id = fx->find("f_lbs_id");
  if (!lbs_id.empty()){
     dict->ShowSection("lbs");
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  // write_status_publish_elseif
  std::string newfeed = fx->find("isNewsFeed");
  std::string userID = fx->find("userID");
  std::string interviewee = fx->find("interviewee"); 
  if (!newfeed.empty())
    dict->ShowSection("isNewsFeed");
  else if (userID != interviewee)
    dict->ShowSection("isNewsFeedELIF");
  IfElse(fx, dict, "f_reply_owner",NULL);
  If(fx, dict, "isNewsFeed",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
  
  #if 1 //for site, added by qun.liu 20110412
  std::string siteID = fx->find("f_site_id");
  if(!siteID.empty()) {
    std::string siteName = fx->find("f_site_name");
    if(siteName.empty()) {
      MCE_INFO("fill_status_publish in body.inc: need f_site_name!");
      return false;
    }
    std::string siteUrl = fx->find("f_site_url");
    if(siteUrl.empty()) {
      MCE_INFO("fill_status_publish in body.inc: need f_site_url!");
      return false;
    }
    
    dict->SetValue("f_site_id", siteID);
    dict->SetValue("f_site_name", siteName);
    dict->SetValue("f_site_url", siteUrl);
    
    dict->ShowSection("f_site_name");
  }
  else {
    dict->ShowSection("f_site_name_NOT");
  }
  #endif //for site, added by qun.liu 20110412
 
  std::string vipsign_signUrl = fx->find("f_vipsign_signUrl");
  if(!vipsign_signUrl.empty()) {
    std::string vipsign_iconUrl = fx->find("f_vipsign_iconUrl");
    std::string vipsign_iconName = fx->find("f_vipsign_iconName");
    dict->SetValue("f_vipsign_signUrl", vipsign_signUrl);
    dict->SetValue("f_vipsign_iconUrl", vipsign_iconUrl);
    dict->SetValue("f_vipsign_iconName", vipsign_iconName);
    dict->ShowSection("f_vipsign_signUrl");
  }
 
  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"条状态");
  return true;
}

bool fill_v_status_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  If(fx,dict, "f_status_re_title", NULL);
  If(fx,dict, "f_status_re_src", NULL);
  IfElse(fx,dict, "f_from_certify_icon", NULL);
  If(fx,dict, "f_status_src", NULL);
  If(fx,dict, "f_status_from_certify_icon", NULL);

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }


  IfElse(fx, dict, "f_reply_owner",NULL);

  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"条状态");
  return true;

}

bool fill_mini_group_share_link(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo*      likeinfo, const std::string & template_file, int index, bool isAdmin) {
  //MCE_INFO( feed->xml);
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "isNewsFeed",NULL);
  IfElse(fx, dict, "f_share_shared_link_title","f_share_shared_link_title");
  If(fx, dict, "f_share_shared_link_image","f_share_shared_link_image");
  If(fx, dict, "f_share_shared_link_body","f_share_shared_link_body");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"个链接分享");
  return true;
}

bool fill_mini_group_share_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo*      likeinfo, const std::string & template_file, int index, bool isAdmin) {
  //MCE_INFO( feed->xml);
  If(fx, dict, "isNewsFeed",NULL);
  std::vector<std::string> titlev = fx->find_v("f_share_id");
  int video_count = titlev.size();
  if (1 == video_count) {
    ctemplate::TemplateDictionary* dict2 = dict->AddSectionDictionary("multi_video_NOT");
    int count = 0;
    const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
    BOOST_FOREACH(const std::string& r, reply_v) {
      ctemplate::TemplateDictionary* sec = dict2->AddSectionDictionary("f_reply_id");
      sec->SetValue("f_reply_id", r);
      MAKE_VALUE("f_reply_from_tinyimg", count);
      MAKE_VALUE("f_reply_body", count);
      MAKE_VALUE("f_reply_from_name", count);
      MAKE_VALUE("f_reply_time", count);
      MAKE_VALUE("f_reply_type", count);
      MAKE_VALUE("f_reply_from_id", count);

      if (++count >= 2) {
        break;
      }
    }

    IfElse(fx, dict2, "selfzancount",NULL);
    // write_assign_key
    if (dict_delete_)
      dict_delete_->SetValue("typeText" ,"个视频分享");
    return true;
  } 

  ctemplate::TemplateDictionary* dict2 = dict->AddSectionDictionary("multi_video"); 
  int video_limit = 0;
  BOOST_FOREACH(const std::string& share_id, titlev) {
    ctemplate::TemplateDictionary* sec = dict2->AddSectionDictionary("video_num");
    sec->SetValue("f_share_id", share_id);
    MAKE_VALUE("f_share_video_title", video_limit);
    MAKE_VALUE("f_share_video_image", video_limit);
    MAKE_VALUE("f_share_video_short_title", video_limit);
    
    if (video_count > 3 && 2 == video_limit) {
      sec->ShowSection("more_video_button");
    }

    if (++video_limit >= 6) {
      break;
    } 
  }
 
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");  
  return true;
}

bool fill_mini_group_share_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo*      likeinfo, const std::string & template_file, int index, bool isAdmin) {
  //MCE_INFO( feed->xml);
  If(fx, dict, "isNewsFeed",NULL);
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;

}
bool fill_mini_group_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo*      likeinfo, const std::string & template_file, int index, bool isAdmin) {
  //MCE_INFO( feed->xml);
  If(fx, dict, "isNewsFeed",NULL);
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  // write_IfWithValue
  {
  std::string value = fx->find("f_blog_type");
  if (!value.empty() && value == "1")
    dict->ShowSection("f_blog_type1");
  }
  If(fx, dict, "f_blog_video_image","f_blog_video_image");
  If(fx, dict, "f_blog_photo_image","f_blog_photo_image");
  If(fx, dict, "f_blog_music",NULL);
  IfElse(fx, dict, "selfzancount",NULL);

  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"篇日志");
  return true;
}
bool fill_mini_group_status_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo*      likeinfo, const std::string & template_file, int index, bool isAdmin) {
  //MCE_INFO( feed->xml);
  If(fx, dict, "isNewsFeed",NULL);
  If(fx,dict, "f_status_re_title", NULL);

  std::string lbs_id = fx->find("f_lbs_id");
  if (!lbs_id.empty()){
     dict->ShowSection("lbs");
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }


  // write_status_publish_elseif
  std::string userID = fx->find("userID");
  std::string interviewee = fx->find("interviewee"); 
  IfElse(fx, dict, "f_reply_owner",NULL);
  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"条状态");
  return true;

}
bool fill_mini_group_photo_publish_one(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo*      likeinfo, const std::string & template_file, int index, bool isAdmin) {
  //MCE_INFO( feed->xml);
  If(fx, dict, "isNewsFeed",NULL);
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }


  // write_photo_one_elseif
  //std::string source = fx->find("f_album_photo_source");
  //std::string digest = fx->find("f_album_photo_digest");
  IfElse(fx, dict, "f_album_photo_digest", NULL);
  IfElse(fx, dict, "selfzancount",NULL);

  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"张照片");
  return true;
}
bool fill_mini_group_invite_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo*      likeinfo, const std::string & template_file, int index, bool isAdmin) {
  //MCE_INFO( feed->xml);
  // write_album_loop
  std::vector<std::string> namev = fx->find_v("f_entry_attachments_attachment_desc");
  std::vector<std::string> urlv = fx->find_v("f_entry_attachments_attachment_url");
  std::vector<std::string> srcv = fx->find_v("f_entry_attachments_attachment_src");



  int namev_size = namev.size();
  int urlv_size = urlv.size(); 
  int srcv_size = srcv.size(); 
  //int desc_full_v_size = desc_full_v.size();
/*
  if (desc_full_v_size != descv_size) {
     MCE_WARN( "body.inc::fill_photo_publish_more --> desc_full_v_size != desc_full_v .   desc_full_v_size:" 
         << desc_full_v_size << " desc_size:" << descv_size;
     return false;
  }
*/
  if (namev_size != urlv_size || namev_size != srcv_size) {
    MCE_WARN( "body.inc::fill_mini_group_invite_friend --> namev_size, urlv_size, srcv_size error!");
    return false;
  }

  int photo_limit = -1;
  BOOST_FOREACH(const std::string& name, namev) {
    if (++photo_limit < 30) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("friends_list");
      sec->SetValue("f_entry_attachments_attachment_desc", name);
      MAKE_VALUE("f_entry_attachments_attachment_url", photo_limit);
      MAKE_VALUE("f_entry_attachments_attachment_src", photo_limit);
    }
  }

  ctemplate::TemplateDictionary* inc = dict->AddIncludeDictionary("TOOLBAR_MINIGROUP_INVITE_AND_CREATE");
  inc->SetFilename("./mini_group/toolbar_minigroup_invite_and_create.ftl.1.tpl2");
 
  unsigned int flag = (unsigned int)source;
  if (!(flag & WS_MINIGROUP)) {
    inc->ShowSection("isNewsFeed");
  } 

}

bool fill_mini_group_creat_group(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo*      likeinfo, const std::string & template_file, int index, bool isAdmin) {
  
  //MCE_INFO( feed->xml);
  ctemplate::TemplateDictionary* inc = dict->AddIncludeDictionary("TOOLBAR_MINIGROUP_INVITE_AND_CREATE");
  inc->SetFilename("./mini_group/toolbar_minigroup_invite_and_create.ftl.1.tpl2");
 
  unsigned int flag = (unsigned int)source;
  if (!(flag & WS_MINIGROUP)) {
    inc->ShowSection("isNewsFeed");
  } 

}


bool fill_page_status_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
    If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
 
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);


  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }



  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_from_certify",NULL);
  // write_IfWithValue
  {
  std::string value = fx->find("visitType");
  if (!value.empty() && value == "newsfeed")
    dict->ShowSection("vTypeminifeed_");
  else
    dict->ShowSection("vTypeminifeed_NOT");
  }
  If(fx, dict, "isNewsFeed",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_mini_site_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  int cnt = -1;
  const std::vector<std::string> & tags_v = fx->find_v("f_site_tag_url");
  BOOST_FOREACH(const std::string& r, tags_v) {
    if (++cnt == 3)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("tags");
    sec->SetValue("f_site_tag_url", r);
    MAKE_VALUE("f_site_tag_name", cnt);
    
  }
  // write_IfWithValue
  {
  std::string value = fx->find("f_blog_type");
  if (!value.empty() && value == "1")
    dict->ShowSection("f_blog_type1");
  }
  If(fx, dict, "f_blog_video_image","f_blog_video_image");
  If(fx, dict, "f_blog_photo_image","f_blog_photo_image");
  If(fx, dict, "f_blog_music",NULL);
  If(fx, dict, "isNewsFeed",NULL);
  IfElse(fx, dict, "selfzancount",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"篇日志");
  return true;
}
bool fill_douding_share_doc(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  IfElse(fx, dict, "f_share_shared_link_title","f_share_shared_link_title");
  If(fx, dict, "f_share_shared_link_image","f_share_shared_link_image");
  If(fx, dict, "f_share_shared_link_body","f_share_shared_link_body");
  IfElse(fx, dict, "selfzancount",NULL);
  

  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"个文档分享");
  return true;

}
bool fill_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {


  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  // write_IfWithValue
  {
  std::string value = fx->find("f_blog_type");
  if (!value.empty() && value == "1")
    dict->ShowSection("f_blog_type1");
  }
  If(fx, dict, "f_blog_video_image","f_blog_video_image");
  If(fx, dict, "f_blog_photo_image","f_blog_photo_image");
  If(fx, dict, "f_blog_music",NULL);
  If(fx, dict, "isNewsFeed",NULL);
  IfElse(fx, dict, "selfzancount",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"篇日志");
  return true;
}
bool fill_page_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
  If(fx, dict, "f_stat_id","f_stat_id");
 
  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  If(fx, dict, "f_blog_video_image","f_blog_video_image");
  If(fx, dict, "f_blog_photo_image","f_blog_photo_image");
  If(fx, dict, "f_blog_music",NULL);

 
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
  }



  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_page_lover_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
 
  If(fx, dict, "f_blog_video_image","f_blog_video_image");
  If(fx, dict, "f_blog_photo_image","f_blog_photo_image");
  If(fx, dict, "f_blog_music",NULL);
  If(fx, dict, "f_stat_id","f_stat_id");


  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
  }



  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_mini_site_photo_publish_one(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
 int cnt = -1;
  const std::vector<std::string> & tags_v = fx->find_v("f_site_tag_url");
  BOOST_FOREACH(const std::string& r, tags_v) {
    if (++cnt == 3)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("tags");
    sec->SetValue("f_site_tag_url", r);
    MAKE_VALUE("f_site_tag_name", cnt);

  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  std::string lbs_id = fx->find("f_lbs_id");
  if (!lbs_id.empty()){
     dict->ShowSection("lbs");
  }
  
  // write_photo_one_elseif
  std::string source = fx->find("f_album_photo_source");
  std::string digest = fx->find("f_album_photo_digest");
  
  If(fx, dict, "f_album_photo_digest","f_album_photo_digest");
  If(fx, dict, "isNewsFeed",NULL);
  IfElse(fx, dict, "selfzancount",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"张照片");
  return true;
}

bool fill_photo_publish_one(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  std::string lbs_id = fx->find("f_lbs_id");
  if (!lbs_id.empty()){
     dict->ShowSection("lbs");
  }
  
  // write_photo_one_elseif
  std::string source = fx->find("f_album_photo_source");
  std::string digest = fx->find("f_album_photo_digest");
  if (!source.empty())
    dict->ShowSection("f_album_photo_source");
  else if (!digest.empty())
    dict->ShowSection("f_album_photo_sourceELIF");
  else
    dict->ShowSection("f_album_photo_sourceELSE");
  If(fx, dict, "isNewsFeed",NULL);
  IfElse(fx, dict, "selfzancount",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"张照片");
  return true;
}
bool fill_page_photo_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
  }

  // write_assign_num
  int inum = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      inum += atoi(a.c_str());
  }
  dict->SetIntValue("num", inum);

  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> iv = fx->find_v("f_album_photo_image");
  BOOST_FOREACH(const std::string& img, iv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", img);
	  MAKE_VALUE("f_album_photo_url", photo_limit);
      MAKE_VALUE("f_album_photo_id", photo_limit);
    }
  }
  If(fx, dict, "f_from_certify",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_lover_photo_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);


  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
  }

  // write_assign_num
  int inum = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      inum += atoi(a.c_str());
  }
  dict->SetIntValue("num", inum);

  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> iv = fx->find_v("f_album_photo_image");
  BOOST_FOREACH(const std::string& img, iv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", img);
	  MAKE_VALUE("f_album_photo_url", photo_limit);
      MAKE_VALUE("f_album_photo_id", photo_limit);
    }
  }
  If(fx, dict, "f_from_certify",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}




bool fill_photo_tag(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_album_photo_image","f_album_photo_image");
  If(fx, dict, "f_from_certify_icon",NULL);
  // write_assign_num
  int inum = 0;
  std::vector<std::string> av = fx->find_v("f_tag_count");
  BOOST_FOREACH(const std::string& a, av) {
      inum += atoi(a.c_str());
  }
  dict->SetIntValue("num", inum);
  If(fx, dict, "f_album_photo_digest","f_album_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_page_photo_tag(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_album_photo_image","f_album_photo_image");

  // write_assign_num
  int inum = 0;
  std::vector<std::string> av = fx->find_v("f_tag_count");
  BOOST_FOREACH(const std::string& a, av) {
      inum += atoi(a.c_str());
  }
  dict->SetIntValue("num", inum);
  If(fx, dict, "f_album_photo_digest","f_album_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_api_simple_feed(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;

}

/*
bool fill_photo_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_assign_num
  int inum = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      inum += atoi(a.c_str());
  }
  //dict->SetIntValue("num", inum);
  
  int version  = 0;
  {
    std::string xml_version = fx->find(kTemplateVersion);
    if (!xml_version.empty())
      version = atoi(xml_version.c_str());
  }
  int type = 701;
  if (inum > 1) {
    type = 709;
    fill_photo_publish_more(feed, fx, dict);
  } else {
    fill_photo_publish_one(feed, fx, dict);
  }
  
  std::string filename = GetTemplateFilename(type, WS_HOME, version);
  fx->template_file = filename;

  // write_del_no_assign_typeText
//   if (dict_delete_)
//     dict_delete_->ShowSection("typeText_NOT");
  return true;
}
*/
bool fill_photo_reply_one(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  IfElse(fx, dict, "f_album_photo_digest","f_album_photo_digest");
  If(fx, dict, "isNewsFeed",NULL);
  IfElse(fx, dict, "selfzancount",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"张照片");
  return true;
}
bool fill_photo_publish_more(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_assign_num
  int itotalCount = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      itotalCount += atoi(a.c_str());
  }
  dict->SetIntValue("totalCount", itotalCount);
  If(fx, dict, "f_from_certify_icon",NULL);
  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> imgv = fx->find_v("f_album_photo_image");
  std::vector<std::string> bigimgv = fx->find_v("f_album_photo_bigImage");
  std::vector<std::string> descv = fx->find_v("f_album_photo_desc");
  //std::vector<std::string> desc_full_v = fx->find_v("f_album_photo_descfull");



  int imgv_size = imgv.size();
  int descv_size = descv.size(); 
  //int desc_full_v_size = desc_full_v.size();
/*
  if (desc_full_v_size != descv_size) {
     MCE_WARN( "body.inc::fill_photo_publish_more --> desc_full_v_size != desc_full_v .   desc_full_v_size:" 
         << desc_full_v_size << " desc_size:" << descv_size;
     return false;
  }
*/

  if (imgv_size != descv_size) {
     //MCE_WARN( descv_size);
     std::vector<std::string> tmp_vec(imgv_size, "&nbsp");
     descv = tmp_vec;
  }
/*
  if (imgv_size >= 3) {
     if ( descv_size < 3) {
       MCE_WARN( descv_size);
       std::vector<std::string> tmp_vec(imgv_size, "&nbsp");
       descv = tmp_vec;
       for (std::vector<std::string>::iterator it = descv.begin(); it != descv.end(); it++) {
         MCE_WARN( *it); 
       }
     }
  } else {
    if (imgv_size != descv_size) {
      MCE_WARN( descv_size);
      std::vector<std::string> tmp_vec(imgv_size, "&nbsp");
      descv = tmp_vec;
      for (std::vector<std::string>::iterator it = descv.begin(); it != descv.end(); it++) {
        MCE_WARN( *it);
      }

    }

  }
*/

  //BOOST_FOREACH(const std::string& img, imgv) {
  for(int i = 0; i < imgv.size(); ++i) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", imgv[i]);
      if(i < bigimgv.size()) {
        sec->SetValue("f_album_photo_bigImage", bigimgv[i]);
      }
      MAKE_VALUE("f_album_photo_id", photo_limit);
      std::string desc = descv[photo_limit];
      //std::string desc_full = desc_full_v[photo_limit]; 
      if (desc != "&nbsp") {
        ctemplate::TemplateDictionary* sub_sec = sec->AddSectionDictionary("photo_desc");
        sub_sec->SetValue("f_album_photo_desc", desc);
        //sub_sec->SetValue("f_album_photo_descfull", desc_full); 
        
      }      
    }
  }

  // write_assign_photoIds
  std::string sphotoIDs;
  int idx = 0;
  std::vector<std::string> photoidv = fx->find_v("f_album_photo_id");
  BOOST_FOREACH(const std::string& imgid, photoidv) {
    if (idx > 19)
      break;
      sphotoIDs += imgid;
      sphotoIDs += ",";
      idx ++;
  }
  dict->SetValue("photoIDs", sphotoIDs);

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }


  // write_totalCount3
  if (itotalCount> 3 && sphotoIDs != "")
    dict->ShowSection("PHOTO_MORE");
    If(fx, dict, "isNewsFeed",NULL);
  IfElse(fx, dict, "selfzancount",NULL);

  // write_assign_key
  if (dict_delete_)
    dict_delete_->SetValue("typeText" ,"个相册");
  return true;
}
bool fill_event_photo_publish_one(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_photo_one_elseif
  std::string source = fx->find("f_album_photo_source");
  std::string digest = fx->find("f_album_photo_digest");
  if (!source.empty())
    dict->ShowSection("f_album_photo_source");
  else if (!digest.empty())
    dict->ShowSection("f_album_photo_sourceELIF");
  else
    dict->ShowSection("f_album_photo_sourceELSE");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_event_photo_publish_more(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_assign_num
  int itotalCount = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      itotalCount += atoi(a.c_str());
  }
  dict->SetIntValue("totalCount", itotalCount);

  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> imgv = fx->find_v("f_album_photo_image");
  BOOST_FOREACH(const std::string& img, imgv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", img);
      MAKE_VALUE("f_album_photo_id", photo_limit);
    }
  }

 // write_assign_photoIds
 std::string sphotoIDs;
 int idx = 0;
 std::vector<std::string> photoidv = fx->find_v("f_album_photo_id");
  BOOST_FOREACH(const std::string& imgid, photoidv) {
    if (idx > 19)
      break;
      sphotoIDs += imgid;
      sphotoIDs += ",";
      idx ++;
  }
  dict->SetValue("photoIDs", sphotoIDs);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_connect_like(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
 
  If(fx, dict, "f_entry_attachments_attachment_src",NULL);

 // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);

  }

 
 // write_del_no_assign_typeText
   if (dict_delete_)
     dict_delete_->ShowSection("typeText_NOT");
   return true;
}  

bool fill_receive_gift(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  std::string isAnonymous = fx->find("f_anonymous");
  std::string isPrivate = fx->find("f_privately");

  if(isAnonymous.empty() && isPrivate.empty()) {
    dict->ShowSection("open");
  } else if (isAnonymous.empty()) {
    dict->ShowSection("private");
  } else if (isPrivate.empty()) {
    dict->ShowSection("anonymous");
  }

  If(fx, dict, "f_gift_blessing", "f_gift_blessing");
  If(fx, dict, "f_gift_meaning", "f_gift_meaning");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_send_gift(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  std::string isAnonymous = fx->find("f_anonymous");
  std::string isPrivate = fx->find("f_privately");

  if(isAnonymous.empty() && isPrivate.empty()) {
    dict->ShowSection("open");
  } else if (isAnonymous.empty()) {
    dict->ShowSection("private");
  } else if (isPrivate.empty()) {
    dict->ShowSection("anonymous");
  }

  If(fx, dict, "f_gift_blessing", "f_gift_blessing");
  If(fx, dict, "f_gift_meaning", "f_gift_meaning");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_receive_vip(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  std::string isAnonymous = fx->find("f_anonymous");
  std::string isPrivate = fx->find("f_privately");

  if(isAnonymous.empty() && isPrivate.empty()) {
    dict->ShowSection("open");
  } else if (isAnonymous.empty()) {
    dict->ShowSection("private");
  } else if (isPrivate.empty()) {
    dict->ShowSection("anonymous");
  }

  If(fx, dict, "f_gift_blessing", "f_gift_blessing");
  If(fx, dict, "f_gift_meaning", "f_gift_meaning");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;

}
bool fill_present_vip(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  std::string isAnonymous = fx->find("f_anonymous");
  std::string isPrivate = fx->find("f_privately");

  if(isAnonymous.empty() && isPrivate.empty()) {
    dict->ShowSection("open");
  } else if (isAnonymous.empty()) {
    dict->ShowSection("private");
  } else if (isPrivate.empty()) {
    dict->ShowSection("anonymous");
  }

  If(fx, dict, "f_gift_blessing", "f_gift_blessing");
  If(fx, dict, "f_gift_meaning", "f_gift_meaning");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);

  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;

}
bool fill_buy_vip(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  std::string isAnonymous = fx->find("f_anonymous");
  std::string isPrivate = fx->find("f_privately");

  if(isAnonymous.empty() && isPrivate.empty()) {
    dict->ShowSection("open");
  } else if (isAnonymous.empty()) {
    dict->ShowSection("private");
  } else if (isPrivate.empty()) {
    dict->ShowSection("anonymous");
  }

  If(fx, dict, "f_gift_blessing", "f_gift_blessing");
  If(fx, dict, "f_gift_meaning", "f_gift_meaning");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);

  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;


}

bool fill_gift_farm(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  
  std::string f_entry_owner2_url = fx->find("f_entry_owner2_url");
  std::string f_entry_owner2_name = fx->find("f_entry_owner2_name"); 
  std::string f_entry_actions_action_title = fx->find("f_entry_actions_action_title");
  std::string f_entry_actions_action_url = fx->find("f_entry_actions_action_url ");

  if (f_entry_owner2_url.empty()) {
    f_entry_owner2_url = "http://apps.renren.com/rr_farm?source=present"; 
  }

  if (f_entry_owner2_name.empty()) {
    f_entry_owner2_name = "人人农场";
  }


  if (f_entry_actions_action_title.empty()) {
    f_entry_actions_action_title = "去人人农场，送更多精彩礼物";
  }


  if (f_entry_actions_action_url.empty()) {
    f_entry_actions_action_url = "http://apps.renren.com/rr_farm?source=present";
  }

  dict->SetValue("f_entry_owner2_url", f_entry_owner2_url);
  dict->SetValue("f_entry_owner2_name", f_entry_owner2_name);
  dict->SetValue("f_entry_actions_action_title", f_entry_actions_action_title);
  dict->SetValue("f_entry_actions_action_url", f_entry_actions_action_url);
  


  std::string anonymous = fx->find("f_from_anonymous");
  if (anonymous == "y") {
    dict->ShowSection("anonymous");
  } else {
    dict-> ShowSection("anonymous_NOT");
  } 
 
 // write_del_no_assign_typeText
   if (dict_delete_)
     dict_delete_->ShowSection("typeText_NOT");
   return true;
}


bool fill_page_gift_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  If(fx, dict, "f_from_certify_icon", NULL);
  If(fx, dict, "f_stat_id","f_stat_id");
  IfElse(fx, dict, "f_from_anonymous", NULL);
  If(fx, dict, "f_entry_comment", "f_entry_comment");
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}






bool fill_gift_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_gift_image","f_gift_image");

  // write_gift_elseif
  std::string value0 = fx->find("f_gift_sender_name");
  if (value0 == "一个TA")
    dict->ShowSection("f_gift_sender_nameTA");
  else if(value0 == "神秘的TA")
    dict->ShowSection("f_gift_sender_nameTAELIF");
  else
    dict->ShowSection("f_gift_sender_nameTAELSE");
  IfElse(fx, dict, "f_gift_body","f_gift_body");

  // write_gift_pair_var
  std::string k1 = fx->find("f_gift_sender_name");
  std::string k2 = fx->find("f_gift_digest");
  if (k1 != "一个TA" && k2 != "")
    dict->ShowSection("f_gift_sender_nameTA_f_gift_digest");
  If(fx, dict, "f_gift_ad_title","f_gift_ad_title");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_gift_publish_free(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_gift_image","f_gift_image");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
      
      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  // write_IfWithValue
  {
  std::string value = fx->find("f_gift_sender_name");
  if (!value.empty() && value == "一个TA")
    dict->ShowSection("f_gift_sender_nameTA");
  else
    dict->ShowSection("f_gift_sender_nameTA_NOT");
  }

  // write_gift_pair_var
  std::string k1 = fx->find("f_gift_sender_name");
  std::string k2 = fx->find("f_gift_digest");
  if (k1 != "一个TA" && k2 != "")
    dict->ShowSection("f_gift_sender_nameTA_f_gift_digest");
  If(fx, dict, "f_gift_ad_title","f_gift_ad_title");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_im_use(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");  

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
   
      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
     }
    idcount++;    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_mobile_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    
      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_friend_school_recommend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");  

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    
       if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_market_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_market_image","f_market_image");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_movie_comment(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_movie_image","f_movie_image");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_movie_mark(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_movie_image","f_movie_image");
  If(fx, dict, "f_movie_mark_title","f_movie_mark_title");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_vip_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
      
      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_vip_share(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_vip_apply_domain(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_vip_apply_template(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
     
      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_vip_apply_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_vip_photosticker(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_photo_image","f_photo_image");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_page_gossip_publish_page(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_owner_certify",NULL);
  If(fx, dict, "f_gossip_digest","f_gossip_digest");
  
  const long ti = 1306403560 * 1000;
  string ti_java_str = fx->find("f_time");
  long ti_java = 0;

  try {
    ti_java =  boost::lexical_cast<long>(ti_java_str);
  } catch (boost::bad_lexical_cast & e) {
    MCE_WARN( "fill_page_gossip_publish_page in body.inc, exception: " << e.what());
    return false;
  }
  //MCE_INFO("2001 ti_java:" << ti_java);

  if (ti < ti_java) { 
    If(fx, dict, "f_stat_id","f_stat_id");
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_pagetiny_gossip_publish_user(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_page_page_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_page_image","f_page_image");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        //MCE_INFO("in body.inc: fill_page_page_join find the uid: "<<id);
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "f_page_certify_icon",NULL);
  IfElse(fx, dict, "f_page_freestype_name","f_page_freestype_name");
  If(fx, dict, "f_page_digest","f_page_digest");
  IfElse(fx, dict, "f_page_freestype_action","f_page_freestype_action");

  const long ti = 1306403560 * 1000;
  string ti_java_str = fx->find("f_time");
  long ti_java = 0;

  try {
    ti_java =  boost::lexical_cast<long>(ti_java_str);
  } catch (boost::bad_lexical_cast & e) {
    MCE_WARN( "fill_page_page_join in body.inc, exception: " << e.what());
    return false;
  }
  //MCE_INFO("2002 ti_java:" << ti_java);

  if (ti < ti_java) { 
    If(fx, dict, "f_stat_id","f_stat_id");
  }



  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_pagetiny_page_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  IfElse(fx, dict, "f_page_freestype_name","f_page_freestype_name");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_page_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_photo_image","f_photo_image");

  // write_pair
  std::string k1 = fx->find("f_photo_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_photo_owner_id_f_from_id");
  else
    dict->ShowSection("f_photo_owner_id_f_from_id_NOT");
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }
  If(fx, dict, "f_photo_digest","f_photo_digest");


  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_pagetiny_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_page_blog_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");



  // write_pair
  std::string k1 = fx->find("f_blog_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_blog_owner_id_f_from_id");
  else
    dict->ShowSection("f_blog_owner_id_f_from_id_NOT");
  If(fx, dict, "f_blog_digest","f_blog_digest");

  const long ti = 1306403560 * 1000;
  string ti_java_str = fx->find("f_time");
  long ti_java = 0;

  try {
    ti_java =  boost::lexical_cast<long>(ti_java_str);
  } catch (boost::bad_lexical_cast & e) {
    MCE_WARN( "fill_page_blog_publish_common in body.inc, exception: " << e.what());
    return false;
  }
  //MCE_INFO("2012 ti_java:" << ti_java);

  if (ti < ti_java) { 
    If(fx, dict, "f_stat_id","f_stat_id");
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_pagetiny_blog_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_page_head_upload(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_userinfo_image","f_userinfo_image");
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

 

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_page_gossip_publish_admin(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_owner_certify",NULL);
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_page_share_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_user_image","f_share_user_image");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "f_share_user_certify",NULL);
  If(fx, dict, "f_share_user_digest","f_share_user_digest");
  If(fx, dict, "f_share_comment","f_share_comment");


  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_page_video_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_page_album_video_brief","f_page_album_video_brief");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_page_popularize(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_gossip_publish_page(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_club_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_page_image","f_page_image");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");  

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "f_page_digest","f_page_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_share_blog(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");
  
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_share_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_share_album_photo_image","f_share_album_photo_image");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    
      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_share_link(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  IfElse(fx, dict, "f_share_shared_link_title","f_share_shared_link_title");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_share_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_share_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_status_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }


  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_photo_image","f_photo_image");
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }
  If(fx, dict, "f_photo_digest","f_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_blog_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_blog_digest","f_blog_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_photo_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_assign_num
  int inum = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      inum += atoi(a.c_str());
  }
  dict->SetIntValue("num", inum);

  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> imgv = fx->find_v("f_album_photo_image");
  BOOST_FOREACH(const std::string& img, imgv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", img);
      MAKE_VALUE("f_album_photo_id", photo_limit);
    }
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_gossip_publish_admin(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_club_share_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_share_user_image","f_share_user_image");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");  

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "f_share_user_digest","f_share_user_digest");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_poll_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_poll_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
      
      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_api_install(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");  

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    
      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "f_app_digest","f_app_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_api_install_mobile(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_app_url","f_app_url");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    
      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_api_follow(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");  

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
     
      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "f_app_image","f_app_image");
  If(fx, dict, "f_app_digest","f_app_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_asp_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_asp_photo_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_assign_num
  int inum = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      inum += atoi(a.c_str());
  }
  dict->SetIntValue("num", inum);

  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> imgv = fx->find_v("f_album_photo_image");
  BOOST_FOREACH(const std::string& img, imgv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", img);
      MAKE_VALUE("f_album_photo_id", photo_limit);
    }
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_asp_school_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_page_image","f_page_image");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  IfElse(fx, dict, "f_page_freestype_name","f_page_freestype_name");
  If(fx, dict, "f_page_digest","f_page_digest");
  IfElse(fx, dict, "f_page_freestype_action","f_page_freestype_action");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_common_textandpic(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_info_image","f_info_image");

  // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");
  
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;    
  }
  If(fx, dict, "f_info_digest","f_info_digest");
  If(fx, dict, "f_info_action_url","f_info_action_url");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_connect_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  IfElse(fx, dict, "f_share_connect_id","f_share_connect_id");
  If(fx, dict, "f_share_comment","f_share_comment");
  IfElse(fx, dict, "selfzancount",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_rain(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_mobile_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_question_and_answer(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_qa_body","f_qa_body");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_edm_edm_flash(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_edm_logo","f_edm_logo");

  IfElse(fx, dict, "f_edm_page_action_url", NULL);
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  // write_IfWithValue
  {
  std::string value = fx->find("f_edm_interactive");
  if (!value.empty() && value == "0")
    dict->ShowSection("f_edm_interactive0");
  else
    dict->ShowSection("f_edm_interactive0_NOT");
  }
  IfElse(fx, dict, "selfzancount",NULL);
  IfElse(fx, dict, "totalzancount","totalzancount");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_edm_edm_flash_box(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  //std::string ll = feed->xml;
  If(fx, dict, "f_edm_logo","f_edm_logo");
  If(fx, dict, "f_edm_body","f_edm_body");

  IfElse(fx, dict, "f_edm_page_action_url", NULL);
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);

  }

  If(fx, dict, "f_edm_ad","f_edm_ad");
  IfElse(fx, dict, "f_edm_resource",NULL);
 // write_IfWithValue
  {
  std::string value = fx->find("f_edm_interactive");
  if (!value.empty() && value == "0")
    dict->ShowSection("f_edm_interactive0");
  else
    dict->ShowSection("f_edm_interactive0_NOT");
  }
  IfElse(fx, dict, "selfzancount",NULL);

  // write_edm_interactive_url
  if (fx->find("f_edm_action_name") != "" && fx->find("f_edm_action_url") != "")
    dict->ShowSection("EDM_NAME_URL");
  IfElse(fx, dict, "f_edm_user_id","f_edm_user_id");
  IfElse(fx, dict, "totalzancount","totalzancount");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}




bool fill_edm_edm_hotfeed(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_edm_edm_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_edm_edm_newbie(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_api_common_rich(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo* likeinfo, const std::string & template_file, int index, bool isAdmin) {

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_app_image","f_app_image");
  If(fx, dict, "f_app_describe","f_app_describe");
  If(fx, dict, "f_app_body","f_app_body");
  // write_IfWithValue
  {
  std::string value = fx->find("f_app_id");
  if (!value.empty() && value != "0")
    dict->ShowSection("f_app_id0_");
  }
  If(fx, dict, "f_app_button_url","f_app_button_url");


// for TOOLBAR_1 =  DELETE1 + DELETE2 + focus
  // TOOLBAR_NOFOCUS_1 =  DELETE1 + DELETE2 

    //ctemplate::TemplateDictionary* del_dict = 0;

    dict->SetFormattedValue("id", "%llu", feed->feed);
    dict->SetIntValue("f_type", WeiredType(feed->type).type);
    dict->SetValue("f_from_id", fx->find("f_from_id"));
    dict->SetValue("f_from_name", fx->find("f_from_name"));

 std::string merge_count = fx->find("merge_feed_count");
    std::vector<std::string> mergev = fx->find_v("merge_feed_fid");
    std::string fids;

    unsigned int flag = render_;
    if (flag & WS_MINIFEED) {
      dict->ShowSection("isNewsFeed_NOT");
    } else {
      dict->ShowSection("isNewsFeed");
      if (u && feed->actor != u->id()) {
        dict->ShowSection("SELF_NOT"); //只有toolbar需要show SELF_NOT
      }
    }

    if (u) {
      std::string uid = lexical_cast<std::string>(u->id());
      if ((uid == fx->find("f_from_id")) || isAdmin) {
        dict->ShowSection("SELF");

      }

      dict->SetIntValue("userID", u->id());
    }

    char first_id[40];
    sprintf(first_id,"%llu",feed->feed);

    fids = "," + fids;
    std::string id_list = first_id  + fids;
    dict->SetValue("id_list", id_list);  //在顶层模板填充id_list，目前只有share_video需要

    //dict_delete_ = del_dict;




  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_api_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_app_body","f_app_body");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_taobao_taobao_share(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  
  IfElse(fx, dict, "selfzancount",NULL);
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_taobao_taobao_comment(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_taobao_taobao_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_asp_question_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_asp_experience_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_page_job_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_stat_id","f_stat_id");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_edm_edm_focus(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  // write_reply_loop
  int count = 0;
  std::vector<std::string> fromv = fx->find_v("f_entry_from_id");
  BOOST_FOREACH(const std::string &u, fromv) {
    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_entry_from_id_L");
    sec->SetValue("f_entry_from_id", u);
    MAKE_VALUE("f_entry_from_name", count);
    count ++;
  }

  count = -1;
  std::vector<std::string> tov = fx->find_v("f_entry_friend_id");
  BOOST_FOREACH(const std::string &u, tov) {
    ctemplate::TemplateDictionary* sec;
    if (++count < 5) {
      sec = dict->AddSectionDictionary("f_entry_friend_id_L_lt_3");
      sec->SetValue("f_entry_friend_id", u);
      MAKE_VALUE("f_entry_friend_name", count);
      MAKE_VALUE("f_entry_friend_head", count);
    }

    sec = dict->AddSectionDictionary("f_entry_friend_id_L");
    sec->SetValue("f_entry_friend_id", u);
    MAKE_VALUE("f_entry_friend_name", count);
  }
  
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}





bool fill_edm_edm_add_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  // write_reply_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");  

  int count = 0;
  std::vector<std::string> fromv = fx->find_v("f_entry_from_id");
  BOOST_FOREACH(const std::string &u, fromv) {
    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_entry_from_id_L");
    sec->SetValue("f_entry_from_id", u);
    MAKE_VALUE("f_entry_from_name", count);

    if(!strVipIconId.empty() && u == lexical_cast<std::string>(feed->actor)) {
      sec->SetValue("userVipIconId", strVipIconId);
      sec->SetValue("userVipIconLevel", strVipIconLevel);
      sec->ShowSection("userVipIconId");
    } 

    count ++;
  }

  count = -1;
  std::vector<std::string> tov = fx->find_v("f_entry_friend_id");
  BOOST_FOREACH(const std::string &u, tov) {
    ctemplate::TemplateDictionary* sec;
    if (++count < 3) {
      sec = dict->AddSectionDictionary("f_entry_friend_id_L_lt_3");
      sec->SetValue("f_entry_friend_id", u);
      MAKE_VALUE("f_entry_friend_name", count);
      MAKE_VALUE("f_entry_friend_head", count);
    }

    sec = dict->AddSectionDictionary("f_entry_friend_id_L");
    sec->SetValue("f_entry_friend_id", u);
    MAKE_VALUE("f_entry_friend_name", count);
  }
  
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_asp_share_asp_text(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_asp_share_asp_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  IfElse(fx, dict, "f_entry_attachments_attachment_url",NULL);
  IfElse(fx, dict, "f_entry_attachments_attachment_src",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_asp_photo_publish_more(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_assign_num
  int itotalCount = 0;
  std::vector<std::string> av = fx->find_v("f_entry_count");
  BOOST_FOREACH(const std::string& a, av) {
      itotalCount += atoi(a.c_str());
  }
  dict->SetIntValue("totalCount", itotalCount);

  // write_attachments_loop
  int photo_limit = -1;
  std::vector<std::string> attv = fx->find_v("f_entry_attachments_attachment_src");

  BOOST_FOREACH(const std::string& a, attv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_entry_attachments_attachment_src_L_lt_3");
      sec->SetValue("f_entry_attachments_attachment_src", a);
      MAKE_VALUE("f_entry_attachments_attachment_url", photo_limit);
    }
  }
  
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_edm_birthday(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  IfElse(fx, dict, "f_entry_from",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}
bool fill_page_page_couples_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  IfElse(fx, dict, "f_from_tinyimg",NULL);
  If(fx, dict, "f_stat_id", "f_stat_id");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_comment_blog(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo* likeinfo, const std::string & template_file, int index, bool isAdmin) {


  std::string from_id = fx->find("f_from_id");
  if (from_id.empty()) {
    MCE_INFO( "body.inc::fill_comment_blog  --> fill_comment_blog --> no blog publish feed.");
    return false;   
  }

  // 最新的回复信息
  // std::string new_comment_from_id = fx->find("f_comment_from_id");
  // std::string new_comment_from_name = fx->find("f_comment_from_name");
  // std::string new_comment_from_tinyimg = fx->find("f_comment_from_tinyimg");

  // new_comment_from_tinyimg = HeadUrl::FromPart(new_comment_from_tinyimg);

  std::string new_comment_type = fx->find("f_comment_type");
  std::string new_comment_origin_url = fx->find("f_comment_origin_url");
  std::string new_comment_origin_image = fx->find("f_comment_origin_image");
  std::string new_comment_origin_title = fx->find("f_comment_origin_title");
  std::string new_comment_origin_type = fx->find("f_comment_origin_type");
  std::string new_comment_time = fx->find("f_comment_time");

  // time format
  std::vector < std::string > time_vec;

  try {
    boost::split(time_vec, new_comment_time, boost::is_any_of(" "), boost::token_compress_on); 

  } catch (...) {
    MCE_WARN( "fill_comment_blog in body.inc, exception: " << new_comment_time);
    return false;
  }


  if (time_vec.size() != 2) {
    MCE_WARN( "body.inc::fill_comment_blog --> time_vec != 2"); 
    return false;
  }
  std::string date_str = time_vec[0];
  std::string time_str = time_vec[1];
  std::vector < std::string > date_vec;
  std::vector < std::string > t_vec;
  
  try {
    boost::split(date_vec, date_str, boost::is_any_of("-"),
          boost::token_compress_on);
    boost::split(t_vec, time_str, boost::is_any_of(":"),
          boost::token_compress_on);
  } catch (...) {
    MCE_WARN( "fill_comment_blog in body.inc, exception: " << time_str);
    return false;
  }


  if (date_vec.size() != 3 || t_vec.size() != 3) {
    MCE_WARN( "body.inc::fill_comment_blog --> date_vec or t_vec != 2");
    return false;
  } 

  struct tm cal_time;

  try {
    cal_time.tm_year =  boost::lexical_cast<int>(date_vec[0])-1900;
    cal_time.tm_mon =  boost::lexical_cast<int>(date_vec[1])-1;
    cal_time.tm_mday =  boost::lexical_cast<int>(date_vec[2]);
    cal_time.tm_hour =  boost::lexical_cast<int>(t_vec[0]);
    cal_time.tm_min =  boost::lexical_cast<int>(t_vec[1]);
    cal_time.tm_sec =  boost::lexical_cast<int>(t_vec[2]);
  } catch (boost::bad_lexical_cast & e) {
    MCE_WARN( "fill_comment_blog in body.inc, exception: " << e.what());
    return false;
  }

  cal_time.tm_isdst = 0; 
  time_t tt = mktime(&cal_time);
  std::stringstream ss;
  ss << tt * 1000;  //java 时间传过来的是毫秒

  dict->SetValue("new_comment_time", ss.str()); 

  
  // write_friend_loop
  const std::vector<std::string> & comment_id_v = fx->find_v("f_comment_id");
  const std::vector<std::string> & comment_body_v = fx->find_v("f_comment_body");
  const std::vector<std::string> & comment_from_name_v = fx->find_v("f_comment_from_name");
  const std::vector<std::string> & comment_time_v = fx->find_v("f_comment_time");
  const std::vector<std::string> & comment_type_v = fx->find_v("f_comment_type");
  const std::vector<std::string> & comment_from_id_v = fx->find_v("f_comment_from_id");  
  
  int vec_size = comment_id_v.size();
  if (comment_body_v.size() != vec_size ||
    comment_from_name_v.size() != vec_size ||
    comment_time_v.size() != vec_size ||
    comment_type_v.size() != vec_size ||
    comment_from_id_v.size() != vec_size) {
    
    MCE_WARN( "body.inc::fill_comment_blog  --> comment blog vec size error!");
    //MCE_WARN( comment_id_v.size());
    //MCE_WARN( comment_body_v.size());
    //MCE_WARN( comment_from_name_v.size());
    //MCE_WARN( comment_time_v.size());
    //MCE_WARN( comment_type_v.size());
    //MCE_WARN( comment_from_id_v.size());
    return false;
  } 
 
  std::set<std::string> uid_set;

  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  for (int i =0; i!=comment_id_v.size(); i++) {

    if (!uid_set.count(comment_from_id_v[i])) {
      uid_set.insert(comment_from_id_v[i]);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("comment_friend_id");
      sec->SetValue("f_comment_id", comment_id_v[i]);

      sec->SetValue("f_comment_body", comment_body_v[i]);
      sec->SetValue("f_comment_from_name", comment_from_name_v[i]);   
      sec->SetValue("f_comment_time", comment_time_v[i]);
      sec->SetValue("f_comment_type", comment_type_v[i]);
      sec->SetValue("f_comment_from_id", comment_from_id_v[i]);
    
      if(!strVipIconId.empty() && comment_from_id_v[i] == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }  
    }
    
  }


  // write comment loop 
  std::vector<std::string>  tinyimg_v = fx->find_v("f_comment_from_tinyimg");
  for (std::vector<std::string>::iterator it = tinyimg_v.begin(); it!= tinyimg_v.end();it++) {
    *it = HeadUrl::FromPart(*it);  
  }
  if (vec_size != tinyimg_v.size()) {
    MCE_WARN( "body.inc::fill_comment_blog  --> comment blog vec size error!");
    //MCE_WARN( tinyimg_v.size());
    return false;
  }


  for (int i =comment_id_v.size()-1; i >= 0; i--) {
    
    if (i >= 1) {
      i =  1;
    }
    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_comment_id");
    sec->SetValue("f_comment_id", comment_id_v[i]);
    sec->SetValue("f_comment_from_tinyimg", tinyimg_v[i]);

    sec->SetValue("f_comment_body", comment_body_v[i]);
    sec->SetValue("f_comment_from_name", comment_from_name_v[i]);   
    sec->SetValue("f_comment_time", comment_time_v[i]);
    sec->SetValue("f_comment_type", comment_type_v[i]);
    sec->SetValue("f_comment_from_id", comment_from_id_v[i]);
      
    
  }


/*
 // source 
  {
    if (new_comment_origin_type == "1") {
      dict->SetValueAndShowSection("vType", vType_, "TYPE_EQUAL_1");
    } else {
      dict->ShowSection("TYPE_EQUAL_1_NOT");
    }
    dict->SetValue("new_comment_type", new_comment_type);
    dict->SetValue("new_comment_origin_url", new_comment_origin_url);
    If(fx, dict, "new_comment_origin_image");
    If(fx, dict, "new_comment_origin_title");
    dict->SetValue("new_comment_origin_title", new_comment_origin_title);
    dict->SetValue("new_comment_origin_image", new_comment_origin_image);
  }
*/

  // write_IfWithValue
  {
    std::string value = fx->find("f_blog_type");
    if (!value.empty() && value == "1")
      dict->ShowSection("f_blog_type1");
  }
  If(fx, dict, "f_blog_video_image","f_blog_video_image");
  If(fx, dict, "f_blog_photo_image","f_blog_photo_image");
  If(fx, dict, "f_blog_music",NULL);
  If(fx, dict, "isNewsFeed",NULL);

////////////////////////////////////////////////////////////////////  
 // 专门处理2901评论日志的赞信息，gid中传入的应该是blog_id
  StringMap & m = fx->body;
  std::string blog_id_str = fx->find("f_blog_id"); 
  xce::ilikeit::GlobalIdPtr gid(new xce::ilikeit::GlobalId);

  long blog_id = 0;
  try {
    blog_id = boost::lexical_cast<long>(blog_id_str);
  } catch (...) {
    MCE_WARN( "fill_comment_blog in body.inc, exception: " << blog_id_str);
    return false;
  }

  gid->id = blog_id;
  gid->type = kGlobalBlog;

  ::xce::ilikeit::LikeInfoSeq likeseq;
  ::xce::ilikeit::GlobalIdSeq globalseq;

  globalseq.push_back(gid);
  bool likeinfo_avaible = false;
  likeinfo_avaible = BuildZan(u->id(), globalseq, &likeseq);

  const ilikeit::LikeInfo* comment_likeinfo = 0;

  //likeinfo = 0;
  if (likeinfo_avaible) {
    comment_likeinfo = likeseq[0].get();
  } 

  if (comment_likeinfo) {
    //MCE_INFO(  comment_likeinfo->totalCount);
  }
///////////
  // std::string totalzancount_str = fx->find("totalzancount");
  // std::string selfzancount_str = fx->find("selfzancount");
  //m.insert(std::make_pair("selfzancount", ""));
  //m.insert(std::make_pair("totalzancount", ""));
  m["selfzancount"] = "";
  m["totalzancount"] = "";
  dict->SetValue("selfzancount", "");
  dict->SetValue("totalzancount", "");

  // 处理 "赞"
  if (comment_likeinfo) {
    int total = comment_likeinfo->totalCount;
    if (comment_likeinfo->selfCount) {
      total -= 1;// 去除自己
      if (total < 0) {
        MCE_WARN( "fill_comment_blog in body.inc, totalzancount:" << total);
      }
      //MCE_INFO( lexical_cast<std::string>(comment_likeinfo->selfCount));
      dict->SetIntValue("selfzancount", comment_likeinfo->selfCount);
      m["selfzancount"] = lexical_cast<std::string>(comment_likeinfo->selfCount);
    }

    if (total > 0) {
      //m.insert(std::make_pair("totalzancount", lexical_cast<std::string>(total)));
      //MCE_INFO( lexical_cast<std::string>(total));
      dict->SetIntValue("totalzancount",  total);
      m["totalzancount"] = lexical_cast<std::string>(total);
    }
    //dict.SetIntValue("selfzancount", likeinfo->selfCount);
    //dict.SetIntValue("totalzancount", total);
  }

  IfElse(fx, dict, "selfzancount",NULL);


  return true;
}

bool fill_comment_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

/*
bool fill_comment_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) { 
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
} */

bool fill_comment_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo* likeinfo, const std::string & template_file, int index, bool isAdmin) {
  std::string from_id = fx->find("f_from_id");
  if (from_id.empty()) {
    MCE_INFO( "body.inc::fill_comment_photo  --> fill_comment_photo --> no photo publish feed.");
    return false;   
  }

  // 最新的回复信息
  // std::string new_comment_from_id = fx->find("f_comment_from_id");
  // std::string new_comment_from_name = fx->find("f_comment_from_name");
  // std::string new_comment_from_tinyimg = fx->find("f_comment_from_tinyimg");

  // new_comment_from_tinyimg = HeadUrl::FromPart(new_comment_from_tinyimg);

  std::string new_comment_type = fx->find("f_comment_type");
  std::string new_comment_origin_url = fx->find("f_comment_origin_url");
  std::string new_comment_origin_image = fx->find("f_comment_origin_image");
  std::string new_comment_origin_title = fx->find("f_comment_origin_title");
  std::string new_comment_origin_type = fx->find("f_comment_origin_type");
  std::string new_comment_time = fx->find("f_comment_time");

  // time format
  std::vector < std::string > time_vec;

  try {
    boost::split(time_vec, new_comment_time, boost::is_any_of(" "), boost::token_compress_on); 

  } catch (...) {
    MCE_WARN( "fill_comment_photo in body.inc, exception: " << new_comment_time);
    return false;
  }


  if (time_vec.size() != 2) {
    MCE_WARN( "body.inc::fill_comment_photo --> time_vec != 2"); 
    return false;
  }
  std::string date_str = time_vec[0];
  std::string time_str = time_vec[1];
  std::vector < std::string > date_vec;
  std::vector < std::string > t_vec;
  
  try {
    boost::split(date_vec, date_str, boost::is_any_of("-"),
          boost::token_compress_on);
    boost::split(t_vec, time_str, boost::is_any_of(":"),
          boost::token_compress_on);
  } catch (...) {
    MCE_WARN( "fill_comment_photo in body.inc, exception: " << time_str);
    return false;
  }


  if (date_vec.size() != 3 || t_vec.size() != 3) {
    MCE_WARN( "body.inc::fill_comment_photo --> date_vec or t_vec != 2");
    return false;
  } 

  struct tm cal_time;

  try {
    cal_time.tm_year =  boost::lexical_cast<int>(date_vec[0])-1900;
    cal_time.tm_mon =  boost::lexical_cast<int>(date_vec[1])-1;
    cal_time.tm_mday =  boost::lexical_cast<int>(date_vec[2]);
    cal_time.tm_hour =  boost::lexical_cast<int>(t_vec[0]);
    cal_time.tm_min =  boost::lexical_cast<int>(t_vec[1]);
    cal_time.tm_sec =  boost::lexical_cast<int>(t_vec[2]);
  } catch (boost::bad_lexical_cast & e) {
    MCE_WARN( "fill_comment_photo in body.inc, exception: " << e.what());
    return false;
  }

  cal_time.tm_isdst = 0; 
  time_t tt = mktime(&cal_time);
  std::stringstream ss;
  ss << tt * 1000;  //java 时间传过来的是毫秒

  dict->SetValue("new_comment_time", ss.str()); 

  
  // write_friend_loop
  const std::vector<std::string> & comment_id_v = fx->find_v("f_comment_id");
  const std::vector<std::string> & comment_body_v = fx->find_v("f_comment_body");
  const std::vector<std::string> & comment_from_name_v = fx->find_v("f_comment_from_name");
  const std::vector<std::string> & comment_time_v = fx->find_v("f_comment_time");
  const std::vector<std::string> & comment_type_v = fx->find_v("f_comment_type");
  const std::vector<std::string> & comment_from_id_v = fx->find_v("f_comment_from_id");  
  
  int vec_size = comment_id_v.size();
  if (comment_body_v.size() != vec_size ||
    comment_from_name_v.size() != vec_size ||
    comment_time_v.size() != vec_size ||
    comment_type_v.size() != vec_size ||
    comment_from_id_v.size() != vec_size) {
    
    MCE_WARN( "body.inc::fill_comment_photo  --> comment photo vec size error!");
    //MCE_WARN( comment_id_v.size());
    //MCE_WARN( comment_body_v.size());
    //MCE_WARN( comment_from_name_v.size());
    //MCE_WARN( comment_time_v.size());
    //MCE_WARN( comment_type_v.size());
    //MCE_WARN( comment_from_id_v.size());
    return false;
  } 
 
  std::set<std::string> uid_set;

  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  for (int i =0; i!=comment_id_v.size(); i++) {

    if (!uid_set.count(comment_from_id_v[i])) {
      uid_set.insert(comment_from_id_v[i]);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("comment_friend_id");
      sec->SetValue("f_comment_id", comment_id_v[i]);

      sec->SetValue("f_comment_body", comment_body_v[i]);
      sec->SetValue("f_comment_from_name", comment_from_name_v[i]);   
      sec->SetValue("f_comment_time", comment_time_v[i]);
      sec->SetValue("f_comment_type", comment_type_v[i]);
      sec->SetValue("f_comment_from_id", comment_from_id_v[i]);
      
      if(!strVipIconId.empty() && comment_from_id_v[i] == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    
  }


  // write comment loop 
  std::vector<std::string>  tinyimg_v = fx->find_v("f_comment_from_tinyimg");
  for (std::vector<std::string>::iterator it = tinyimg_v.begin(); it!= tinyimg_v.end();it++) {
    *it = HeadUrl::FromPart(*it);  
  }
  if (vec_size != tinyimg_v.size()) {
    MCE_WARN( "body.inc::fill_comment_photo  --> comment photo vec size error!");
    //MCE_WARN( tinyimg_v.size());
    return false;
  }


  for (int i =comment_id_v.size()-1; i >= 0; i--) {
    
    if (i >= 1) {
      i =  1;
    }
    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_comment_id");
    sec->SetValue("f_comment_id", comment_id_v[i]);
    sec->SetValue("f_comment_from_tinyimg", tinyimg_v[i]);

    sec->SetValue("f_comment_body", comment_body_v[i]);
    sec->SetValue("f_comment_from_name", comment_from_name_v[i]);   
    sec->SetValue("f_comment_time", comment_time_v[i]);
    sec->SetValue("f_comment_type", comment_type_v[i]);
    sec->SetValue("f_comment_from_id", comment_from_id_v[i]);
      
    
  }
/*
 // source 
  {
    if (new_comment_origin_type == "1") {
      dict->SetValueAndShowSection("vType", vType_, "TYPE_EQUAL_1");
    } else {
      dict->ShowSection("TYPE_EQUAL_1_NOT");
    }
    dict->SetValue("new_comment_type", new_comment_type);
    dict->SetValue("new_comment_origin_url", new_comment_origin_url);
    If(fx, dict, "new_comment_origin_image");
    If(fx, dict, "new_comment_origin_title");
    dict->SetValue("new_comment_origin_title", new_comment_origin_title);
    dict->SetValue("new_comment_origin_image", new_comment_origin_image);
  }
*/

/*
  // write_IfWithValue
  {
    std::string value = fx->find("f_photo_type");
    if (!value.empty() && value == "1")
      dict->ShowSection("f_photo_type1");
  }
  If(fx, dict, "f_photo_video_image","f_photo_video_image");
  If(fx, dict, "f_photo_photo_image","f_photo_photo_image");
  If(fx, dict, "f_photo_music",NULL);
*/
  If(fx, dict, "isNewsFeed",NULL);


////////////////////////////////////////////////////////////////////  
 // 专门处理2902评论照片的赞信息，gid中传入的应该是photo_id
    StringMap & m = fx->body;
    std::string album_photo_id_str = fx->find("f_album_photo_id"); 
    //MCE_INFO( album_photo_id_str);
    xce::ilikeit::GlobalIdPtr gid(new xce::ilikeit::GlobalId);

    long album_photo_id = 0;
    try {
      album_photo_id = boost::lexical_cast<long>(album_photo_id_str);
    } catch (...) {
      MCE_WARN( "fill_comment_photo in body.inc, exception: " << album_photo_id_str);
      return false;
    }

    gid->id = album_photo_id;
    gid->type = kGlobalPhoto;

    ::xce::ilikeit::LikeInfoSeq likeseq;
    ::xce::ilikeit::GlobalIdSeq globalseq;

    globalseq.push_back(gid);
    bool likeinfo_avaible = false;
    likeinfo_avaible = BuildZan(u->id(), globalseq, &likeseq);

    const ilikeit::LikeInfo* comment_likeinfo = 0;
    //likeinfo = 0;
    if (likeinfo_avaible) {
      comment_likeinfo = likeseq[0].get();
    } 
   
    if (comment_likeinfo) {
      //MCE_INFO(  comment_likeinfo->totalCount);
    }

///////////
  // std::string totalzancount_str = fx->find("totalzancount");
  // std::string selfzancount_str = fx->find("selfzancount");
  m["selfzancount"] = "";
  m["totalzancount"] = "";
  dict->SetValue("selfzancount", "");
  dict->SetValue("totalzancount", "");

  // 处理 "赞"
  if (comment_likeinfo) {
    int total = comment_likeinfo->totalCount;
    if (comment_likeinfo->selfCount) {
      total -= 1;// 去除自己
      if (total < 0) {
        MCE_WARN( "fill_comment_photo in body.inc, totalzancount: " << total);
      }
      //MCE_INFO( lexical_cast<std::string>(comment_likeinfo->selfCount));
      m["selfzancount"] = lexical_cast<std::string>(comment_likeinfo->selfCount);
      dict->SetIntValue("selfzancount", comment_likeinfo->selfCount);
    }

    if (total > 0) {
      m["totalzancount"] = lexical_cast<std::string>(total);
      //MCE_INFO( lexical_cast<std::string>(total));
      dict->SetIntValue("totalzancount",  total);
    }
  }

  IfElse(fx, dict, "selfzancount",NULL);
  // write_del_no_assign_typeText
  if (dict_delete_) 
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}  

bool fill_comment_album(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo* likeinfo, const std::string & template_file, int index, bool isAdmin) {

  std::string from_id = fx->find("f_from_id");
  if (from_id.empty()) {
    MCE_INFO( "body.inc::fill_comment_album  --> no photo publish feed.");
    return false;   
  }



  // write_assign_num
  int itotalCount = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      itotalCount += atoi(a.c_str());
  }
  dict->SetIntValue("totalCount", itotalCount);

  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> imgv = fx->find_v("f_album_photo_image");
  std::vector<std::string> descv = fx->find_v("f_album_photo_desc");
  //std::vector<std::string> desc_full_v = fx->find_v("f_album_photo_descfull");



  int imgv_size = imgv.size();
  int descv_size = descv.size(); 
  //int desc_full_v_size = desc_full_v.size();
/*
  if (desc_full_v_size != descv_size) {
     MCE_WARN( "body.inc::fill_comment_album --> desc_full_v_size != desc_full_v .   desc_full_v_size:" 
         << desc_full_v_size << " desc_size:" << descv_size;
     return false;
  }
*/
  if (imgv_size != descv_size) {
     //MCE_WARN( descv_size);
     std::vector<std::string> tmp_vec(imgv_size, "&nbsp");
     descv = tmp_vec;
  }
/*
  if (imgv_size >= 3) {
     if ( descv_size < 3) {
       MCE_WARN( descv_size);
       std::vector<std::string> tmp_vec(3, "&nbsp");
       descv = tmp_vec;
       for (std::vector<std::string>::iterator it = descv.begin(); it != descv.end(); it++) {
         MCE_WARN( *it); 
       }
     }
  } else {
    if (imgv_size != descv_size) {
      MCE_WARN( descv_size);
      std::vector<std::string> tmp_vec(imgv_size, "&nbsp");
      descv = tmp_vec;
      for (std::vector<std::string>::iterator it = descv.begin(); it != descv.end(); it++) {
        MCE_WARN( *it);
      }

    }

  }
*/

  BOOST_FOREACH(const std::string& img, imgv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", img);
      MAKE_VALUE("f_album_photo_id", photo_limit);
      std::string desc = descv[photo_limit];
      //std::string desc_full = desc_full_v[photo_limit]; 
      if (desc != "&nbsp") {
        ctemplate::TemplateDictionary* sub_sec = sec->AddSectionDictionary("photo_desc");
        sub_sec->SetValue("f_album_photo_desc", desc);
        //sub_sec->SetValue("f_album_photo_descfull", desc_full); 
        
      }      
    }
  }

  // write_assign_photoIds
  std::string sphotoIDs;
  int idx = 0;
  std::vector<std::string> photoidv = fx->find_v("f_album_photo_id");
  BOOST_FOREACH(const std::string& imgid, photoidv) {
    if (idx > 19)
      break;
      sphotoIDs += imgid;
      sphotoIDs += ",";
      idx ++;
  }
  dict->SetValue("photoIDs", sphotoIDs);


  // write_totalCount3
  if (itotalCount> 3 && sphotoIDs != "") {
    dict->ShowSection("PHOTO_MORE");
  }

  If(fx, dict, "isNewsFeed",NULL);
  // 最新的回复信息
  // std::string new_comment_from_id = fx->find("f_comment_from_id");
  // std::string new_comment_from_name = fx->find("f_comment_from_name");
  // std::string new_comment_from_tinyimg = fx->find("f_comment_from_tinyimg");

  // new_comment_from_tinyimg = HeadUrl::FromPart(new_comment_from_tinyimg);

  std::string new_comment_type = fx->find("f_comment_type");
  std::string new_comment_origin_url = fx->find("f_comment_origin_url");
  std::string new_comment_origin_image = fx->find("f_comment_origin_image");
  std::string new_comment_origin_title = fx->find("f_comment_origin_title");
  std::string new_comment_origin_type = fx->find("f_comment_origin_type");
  std::string new_comment_time = fx->find("f_comment_time");

  // time format
  std::vector < std::string > time_vec;

  try {
    boost::split(time_vec, new_comment_time, boost::is_any_of(" "), boost::token_compress_on); 

  } catch (...) {
    MCE_WARN( "fill_comment_album in body.inc, exception: " << new_comment_time);
    return false;
  }


  if (time_vec.size() != 2) {
    MCE_WARN( "body.inc::fill_comment_album --> time_vec != 2"); 
    return false;
  }
  std::string date_str = time_vec[0];
  std::string time_str = time_vec[1];
  std::vector < std::string > date_vec;
  std::vector < std::string > t_vec;
  
  try {
    boost::split(date_vec, date_str, boost::is_any_of("-"),
          boost::token_compress_on);
    boost::split(t_vec, time_str, boost::is_any_of(":"),
          boost::token_compress_on);
  } catch (...) {
    MCE_WARN( "fill_comment_album in body.inc, exception: " << time_str);
    return false;
  }


  if (date_vec.size() != 3 || t_vec.size() != 3) {
    MCE_WARN( "body.inc::fill_comment_album --> date_vec or t_vec != 2");
    return false;
  } 

  struct tm cal_time;

  try {
    cal_time.tm_year =  boost::lexical_cast<int>(date_vec[0])-1900;
    cal_time.tm_mon =  boost::lexical_cast<int>(date_vec[1])-1;
    cal_time.tm_mday =  boost::lexical_cast<int>(date_vec[2]);
    cal_time.tm_hour =  boost::lexical_cast<int>(t_vec[0]);
    cal_time.tm_min =  boost::lexical_cast<int>(t_vec[1]);
    cal_time.tm_sec =  boost::lexical_cast<int>(t_vec[2]);
  } catch (boost::bad_lexical_cast & e) {
    MCE_WARN( "fill_comment_album in body.inc, exception: " << e.what());
    return false;
  }

  cal_time.tm_isdst = 0; 
  time_t tt = mktime(&cal_time);
  std::stringstream ss;
  ss << tt * 1000;  //java 时间传过来的是毫秒

  dict->SetValue("new_comment_time", ss.str()); 

  
  // write_friend_loop
  const std::vector<std::string> & comment_id_v = fx->find_v("f_comment_id");
  const std::vector<std::string> & comment_body_v = fx->find_v("f_comment_body");
  const std::vector<std::string> & comment_from_name_v = fx->find_v("f_comment_from_name");
  const std::vector<std::string> & comment_time_v = fx->find_v("f_comment_time");
  const std::vector<std::string> & comment_type_v = fx->find_v("f_comment_type");
  const std::vector<std::string> & comment_from_id_v = fx->find_v("f_comment_from_id");  
  
  int vec_size = comment_id_v.size();
  if (comment_body_v.size() != vec_size ||
    comment_from_name_v.size() != vec_size ||
    comment_time_v.size() != vec_size ||
    comment_type_v.size() != vec_size ||
    comment_from_id_v.size() != vec_size) {
    
    MCE_WARN( "body.inc::fill_comment_album  --> comment photo vec size error!");
    //MCE_WARN( comment_id_v.size());
    //MCE_WARN( comment_body_v.size());
    //MCE_WARN( comment_from_name_v.size());
    //MCE_WARN( comment_time_v.size());
    //MCE_WARN( comment_type_v.size());
    //MCE_WARN( comment_from_id_v.size());
    return false;
  } 
 
  std::set<std::string> uid_set;

  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  for (int i =0; i!=comment_id_v.size(); i++) {

    if (!uid_set.count(comment_from_id_v[i])) {
      uid_set.insert(comment_from_id_v[i]);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("comment_friend_id");
      sec->SetValue("f_comment_id", comment_id_v[i]);

      sec->SetValue("f_comment_body", comment_body_v[i]);
      sec->SetValue("f_comment_from_name", comment_from_name_v[i]);   
      sec->SetValue("f_comment_time", comment_time_v[i]);
      sec->SetValue("f_comment_type", comment_type_v[i]);
      sec->SetValue("f_comment_from_id", comment_from_id_v[i]);

      if(!strVipIconId.empty() && comment_from_id_v[i] == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    
  }


  // write comment loop 
  std::vector<std::string>  tinyimg_v = fx->find_v("f_comment_from_tinyimg");
  for (std::vector<std::string>::iterator it = tinyimg_v.begin(); it!= tinyimg_v.end();it++) {
    *it = HeadUrl::FromPart(*it);  
  }
  if (vec_size != tinyimg_v.size()) {
    MCE_WARN( "body.inc::fill_comment_album  --> comment photo vec size error!");
    //MCE_WARN( tinyimg_v.size());
    return false;
  }


  for (int i =comment_id_v.size()-1; i >= 0; i--) {
    
    if (i >= 1) {
      i =  1;
    }
    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_comment_id");
    sec->SetValue("f_comment_id", comment_id_v[i]);
    sec->SetValue("f_comment_from_tinyimg", tinyimg_v[i]);

    sec->SetValue("f_comment_body", comment_body_v[i]);
    sec->SetValue("f_comment_from_name", comment_from_name_v[i]);   
    sec->SetValue("f_comment_time", comment_time_v[i]);
    sec->SetValue("f_comment_type", comment_type_v[i]);
    sec->SetValue("f_comment_from_id", comment_from_id_v[i]);
      
    
  }
/*
 // source 
  {
    if (new_comment_origin_type == "1") {
      dict->SetValueAndShowSection("vType", vType_, "TYPE_EQUAL_1");
    } else {
      dict->ShowSection("TYPE_EQUAL_1_NOT");
    }
    dict->SetValue("new_comment_type", new_comment_type);
    dict->SetValue("new_comment_origin_url", new_comment_origin_url);
    If(fx, dict, "new_comment_origin_image");
    If(fx, dict, "new_comment_origin_title");
    dict->SetValue("new_comment_origin_title", new_comment_origin_title);
    dict->SetValue("new_comment_origin_image", new_comment_origin_image);
  }
*/

/*
  // write_IfWithValue
  {
    std::string value = fx->find("f_photo_type");
    if (!value.empty() && value == "1")
      dict->ShowSection("f_photo_type1");
  }
  If(fx, dict, "f_photo_video_image","f_photo_video_image");
  If(fx, dict, "f_photo_photo_image","f_photo_photo_image");
  If(fx, dict, "f_photo_music",NULL);
*/
  If(fx, dict, "isNewsFeed",NULL);


////////////////////////////////////////////////////////////////////  
 // 专门处理2906评论相册的赞信息，gid中传入的应该是album_id
  StringMap & m = fx->body;

  std::string album_id_str = fx->find("f_album_id"); 
  //MCE_INFO( album_id_str);
  xce::ilikeit::GlobalIdPtr gid(new xce::ilikeit::GlobalId);

  long album_id = 0;
  try {
    album_id = boost::lexical_cast<long>(album_id_str);
  } catch (...) {
    MCE_WARN( "fill_comment_album in body.inc, exception: " << album_id_str);
    return false;  
  } 

  gid->id = album_id;
  gid->type = kGlobalAlbum;

  ::xce::ilikeit::LikeInfoSeq likeseq;
  ::xce::ilikeit::GlobalIdSeq globalseq;

  globalseq.push_back(gid);
  bool likeinfo_avaible = false;
  likeinfo_avaible = BuildZan(u->id(), globalseq, &likeseq);

  const ilikeit::LikeInfo* comment_likeinfo = 0;
  //likeinfo = 0;
  if (likeinfo_avaible) {
    comment_likeinfo = likeseq[0].get();
  } 
 
  if (comment_likeinfo) {
    //MCE_INFO(  comment_likeinfo->totalCount);   
  }

///////////
  // std::string totalzancount_str = fx->find("totalzancount");
  // std::string selfzancount_str = fx->find("selfzancount");
  m["selfzancount"] = "";
  m["totalzancount"] = "";
  dict->SetValue("selfzancount", "");
  dict->SetValue("totalzancount", "");

  // 处理 "赞"
  if (comment_likeinfo) {
    int total = comment_likeinfo->totalCount;
    if (comment_likeinfo->selfCount) {
      total -= 1;// 去除自己
      if (total < 0) {
        MCE_WARN( "fill_comment_album in body.inc, totalzancount: " << total);
      }
      //dict.ShowSection("selfzancount");
      m["selfzancount"] = lexical_cast<std::string>(comment_likeinfo->selfCount);
      //MCE_INFO( lexical_cast<std::string>(comment_likeinfo->selfCount));
      dict->SetIntValue("selfzancount", comment_likeinfo->selfCount);

    }
    //else 
    //  dict.ShowSection("selfzancount_NOT");
    if (total > 0) {
      m["totalzancount"] = lexical_cast<std::string>(total);
      //MCE_INFO( lexical_cast<std::string>(total));
      dict->SetIntValue("totalzancount",  total);
    }
    //dict.SetIntValue("selfzancount", likeinfo->selfCount);
    //dict.SetIntValue("totalzancount", total);
  }

  IfElse(fx, dict, "selfzancount",NULL);
 
  // write_del_no_assign_typeText
  if (dict_delete_) 
    dict_delete_->ShowSection("typeText_NOT");
  return true;

}

bool fill_comment_message(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
    
  // write_del_no_assign_typeText
  if (dict_delete_) 
    dict_delete_->ShowSection("typeText_NOT");
  return true; 
}

bool fill_miss_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_del_no_assign_typeText
  if (dict_delete_) 
    dict_delete_->ShowSection("typeText_NOT");
  return true; 
}

bool fill_miss_photo_publish_one(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_del_no_assign_typeText
  if (dict_delete_) 
    dict_delete_->ShowSection("typeText_NOT");
  return true; 
}

bool fill_miss_photo_publish_more(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_del_no_assign_typeText
  if (dict_delete_) 
    dict_delete_->ShowSection("typeText_NOT");
  return true; 
}

bool fill_superman(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_del_no_assign_typeText
  if (dict_delete_) 
    dict_delete_->ShowSection("typeText_NOT");
  return true; 
}


bool fill_guide_recommend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  If(fx, dict, "f_userinfo_comment","f_userinfo_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_edm_edm_pymk_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  // write_reply_loop
  int count = 0;
  std::vector<std::string> fromv = fx->find_v("f_entry_from_id");
  BOOST_FOREACH(const std::string &u, fromv) {
    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_entry_from_id_L");
    sec->SetValue("f_entry_from_id", u);
    MAKE_VALUE("f_entry_from_name", count);
    count ++;
  }

  count = -1;
  std::vector<std::string> tov = fx->find_v("f_entry_friend_id");
  BOOST_FOREACH(const std::string &u, tov) {
    ctemplate::TemplateDictionary* sec;
    if (++count < 3) {
      sec = dict->AddSectionDictionary("f_entry_friend_id_L_lt_3");
      sec->SetValue("f_entry_friend_id", u);
      MAKE_VALUE("f_entry_friend_name", count);
      MAKE_VALUE("f_entry_friend_head", count);
    }

    sec = dict->AddSectionDictionary("f_entry_friend_id_L");
    sec->SetValue("f_entry_friend_id", u);
    MAKE_VALUE("f_entry_friend_name", count);
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_app_hot_or_not_photo_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_userinfo_comment","f_userinfo_comment");
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_app_hot_or_not_mark(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_userinfo_comment","f_userinfo_comment");
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_share_group(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  IfElse(fx, dict, "selfzancount",NULL);
   int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  return true;
}


bool fill_invite_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_entry_attachments_attachment_id");
  int friends_num = vs.size();
  char num_str[40];
  sprintf(num_str,"%d",friends_num);
  dict->SetValue("friends_num", num_str);


  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("friends_list_L_5");
      sec->SetValue("f_entry_attachments_attachment_id", id);
      MAKE_VALUE("f_entry_attachments_attachment_src", idcount);
      MAKE_VALUE("f_entry_attachments_attachment_desc", idcount);
    }
    idcount++;
    if (idcount >=5) {
      break;
    }    
  }  





 // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_share_page_album(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  //IfElse(fx, dict, "f_share_album_photo_image");
  If(fx, dict, "f_entry_comment", "f_entry_comment");
  // write_name_loop
  //std::set<std::string> filter_id;
  /*int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }  */
  //If(fx, dict, "f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_share_group_thread(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  IfElse(fx, dict, "selfzancount",NULL);
   int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  return true;
}

bool fill_newmusic_comment(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  IfElse(fx, dict, "selfzancount",NULL);
   int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  return true;
}

bool fill_newmusic_selection(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  IfElse(fx, dict, "selfzancount",NULL);
   int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  return true;
}

bool fill_newmusic_like(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_entry_body", "f_entry_body");
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  IfElse(fx, dict, "selfzancount",NULL);
  If(fx, dict, "f_entry_attachments_attachment_src", "f_entry_attachments_attachment_src");
   int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  return true;
}

bool fill_newmusic_album(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  IfElse(fx, dict, "selfzancount",NULL);
   int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  return true;
}

bool fill_newmusic_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  IfElse(fx, dict, "selfzancount",NULL);
   int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  return true;
}

bool fill_group_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
   // write_name_loop
  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }
    }
    idcount++;
  }  

  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_group_create(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_group_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict){
  
  std::string value = fx->find("f_origin_title");
  if (value.empty()) {
    dict->SetValue("f_origin_title", "thread");
  } else {
    dict->SetValue("f_origin_title",value);
  }

  If(fx, dict, "f_from_certify_icon", "f_from_certify_icon");
  
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  If(fx, dict, "f_entry_attachments_attachment_src","f_entry_attachments_attachment_src");
  return true;
}

bool fill_group_photo_publish_more(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_assign_num
  int itotalCount = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      itotalCount += atoi(a.c_str());
  }
  dict->SetIntValue("totalCount", itotalCount);

  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> imgv = fx->find_v("f_album_photo_image");
  BOOST_FOREACH(const std::string& img, imgv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", img);
      MAKE_VALUE("f_album_photo_id", photo_limit);
	  MAKE_VALUE("f_album_photo_url", photo_limit);
    }
  }

 // write_assign_photoIds
 std::string sphotoIDs;
 int idx = 0;
 std::vector<std::string> photoidv = fx->find_v("f_album_photo_id");
  BOOST_FOREACH(const std::string& imgid, photoidv) {
    if (idx > 19)
      break;
      sphotoIDs += imgid;
      sphotoIDs += ",";
      idx ++;
  }
  dict->SetValue("photoIDs", sphotoIDs);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_share_couple(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_page_lover_status_update(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
 
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  If(fx, dict, "f_from_certify",NULL);
  {
  std::string value = fx->find("visitType");
  if (!value.empty() && value == "newsfeed")
    dict->ShowSection("vTypeminifeed_");
  else
	dict->ShowSection("vTypeminifeed_NOT");
  }
  If(fx, dict, "isNewsFeed",NULL);

  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
	
	 int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  return true;
}

bool fill_social_info(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  return true;
}

bool fill_social_add_friend_mini(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo* likeinfo, const std::string & template_file, int index, bool isAdmin) {

  std::string from_id_str = fx->find("f_from_id");
  int f_from_id = 0;
   try {
    f_from_id =  boost::lexical_cast<int>(from_id_str);
  } catch (boost::bad_lexical_cast & e) {
    MCE_WARN( "fill_social_add_friend_mini in body.inc, exception: " << e.what());
    return false;
  }
 
  const std::vector<std::string> & friends_id_v = fx->find_v("f_entry_friend_id");
  const std::vector<std::string> & friends_name_v = fx->find_v("f_entry_friend_name");
  const std::vector<std::string> & friends_head_v = fx->find_v("f_entry_friend_head");
  int id_vec_size = friends_id_v.size();
  if (friends_name_v.size() != id_vec_size || friends_head_v.size() != id_vec_size) {
    MCE_WARN( "fill_social_add_friend_mini: friends_name_v.size() != id_vec_size, feedid:" << feed->feed);
    return false;
  }
 
  if (friends_id_v.size() > 1) {
    ctemplate::TemplateDictionary* other_sec = dict->AddSectionDictionary("other_friends");
    int other_count = id_vec_size - 1;
    
    other_sec->SetIntValue("other_friends_count", other_count); 
    for (int i = 1; i != friends_id_v.size(); i++) {
      ctemplate::TemplateDictionary* other_friends_name_sec = other_sec->AddSectionDictionary("other_friends_name");
      other_friends_name_sec->SetValue("other_friends_name", friends_name_v[i]);
      //ctemplate::TemplateDictionary* other_friends_id_sec = other_friends_name_sec->AddSectionDictionary("other_friends_id");
      other_friends_name_sec->SetValue("other_friends_id", friends_id_v[i]);

    }
  }

   
  for (int i = 0; i != friends_id_v.size(); i++) {
    ctemplate::TemplateDictionary* friends_sec = dict->AddSectionDictionary("friend_list");
    friends_sec->SetValue("friend_id", friends_id_v[i]);
    friends_sec->SetValue("friend_name", friends_name_v[i]);
    friends_sec->SetValue("friend_head", friends_head_v[i]);
    
  }
  
  if (u->id() != f_from_id) {
    ctemplate::TemplateDictionary* friendship_sec = dict->AddSectionDictionary("lookup_relationship");
    for (int i = 0; i != friends_id_v.size(); i++) {
      ctemplate::TemplateDictionary* friends_sec2 = friendship_sec->AddSectionDictionary("friend_list2");
      friends_sec2->SetValue("friend_id", friends_id_v[i]);
    }
  } 
  return true;
}

bool fill_social_namecard_mini(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo* likeinfo, const std::string & template_file, int index, bool isAdmin) {

  std::string from_id_str = fx->find("f_from_id");
  int f_from_id = 0;
   try {
    f_from_id =  boost::lexical_cast<int>(from_id_str);
  } catch (boost::bad_lexical_cast & e) {
    MCE_WARN( "fill_social_namecard_mini in body.inc, exception: " << e.what());
    return false;
  }
 
  const std::vector<std::string> & friends_id_v = fx->find_v("f_entry_friend_id");
  const std::vector<std::string> & friends_name_v = fx->find_v("f_entry_friend_name");
  int id_vec_size = friends_id_v.size();
  if (friends_name_v.size() != id_vec_size) {
    MCE_WARN( "fill_social_namecard_mini: friends_name_v.size() != id_vec_size, feedid:" << feed->feed);
    return false;
  }
 
  if (friends_id_v.size() > 1) {
    ctemplate::TemplateDictionary* other_sec = dict->AddSectionDictionary("other_friends");
    int other_count = id_vec_size - 1;
    
    other_sec->SetIntValue("other_friends_count", other_count); 
    for (int i = 1; i != friends_id_v.size(); i++) {
      ctemplate::TemplateDictionary* other_friends_name_sec = other_sec->AddSectionDictionary("other_friends_name");
      other_friends_name_sec->SetValue("other_friends_name", friends_name_v[i]);
      //ctemplate::TemplateDictionary* other_friends_id_sec = other_friends_name_sec->AddSectionDictionary("other_friends_id");
      other_friends_name_sec->SetValue("other_friends_id", friends_id_v[i]);
    }
  }

   
  for (int i = 0; i != friends_id_v.size(); i++) {
    ctemplate::TemplateDictionary* friends_sec = dict->AddSectionDictionary("friend_list");
    friends_sec->SetValue("friend_id", friends_id_v[i]);
    friends_sec->SetValue("friend_name", friends_name_v[i]);
  }
  
  if (u->id() != f_from_id) {
    ctemplate::TemplateDictionary* friendship_sec = dict->AddSectionDictionary("lookup_relationship");
    for (int i = 0; i != friends_id_v.size(); i++) {
      ctemplate::TemplateDictionary* friends_sec2 = friendship_sec->AddSectionDictionary("friend_list2");
      friends_sec2->SetValue("friend_id", friends_id_v[i]);
    }
  } 
  return true;
}

bool fill_rraiting(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  //MCE_INFO("rraiting:" << feed->xml);

  std::set<std::string> filter_id;

  int count = -1;
  std::set<std::string> id_set;
  const std::vector<std::string> & id_v = fx->find_v("f_listener_id");
  const std::vector<std::string> & name_v = fx->find_v("f_listener_name");
  const std::vector<std::string> & url_v = fx->find_v("f_listener_url");
  //MCE_INFO("size:" << name_v.size() << " id:" << id_v[0] << " name:" << name_v[0] << " url:" << url_v[0]);

  if (id_v.size() != name_v.size() || id_v.size() != url_v.size()) {
    MCE_WARN("body.inc::fill_rraiting --> vector size error!");
    return false;
  }

  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");
    
  BOOST_FOREACH(const std::string& id, id_v) {

    if (count <= 4 && filter_id.find(id) == filter_id.end()) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("friend_list");
      sec->SetValue("listener_id", id);
      MAKE_VALUE("listener_name", count);
      MAKE_VALUE("listener_url", count);

      if(!strVipIconId.empty() && id == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
      }      

      count++;
    }
    filter_id.insert(id);

  } 

  if (count >= 5) {
    ctemplate::TemplateDictionary* more_sec = dict->AddSectionDictionary("more");
    more_sec->SetIntValue("num", filter_id.size());
  }

	count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }
  If(fx, dict, "isNewsFeed",NULL);
  IfElse(fx, dict, "selfzancount",NULL);

  if (dict_delete_) 
    dict_delete_->ShowSection("typeText_NOT");
  return true; 
}

bool fill_xiaoyoulu(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  //MCE_INFO("xiaoyou feed:" << feed->feed);
  //MCE_INFO("xiaoyou:" << feed->xml);

  std::string strVipIconId = fx->find("userVipIconId");
  std::string strVipIconLevel = fx->find("userVipIconLevel");

  int count = -1;
  const std::vector<std::string> & id_v = fx->find_v("f_classmate_id");
  const std::vector<std::string> & name_v = fx->find_v("f_classmate_name");
  const std::vector<std::string> & url_v = fx->find_v("f_classmate_url");

  if (id_v.size() != name_v.size() || id_v.size() != url_v.size()) {
    MCE_WARN("body.inc::fill_xiaoyoulu --> vector size error!");
    return false;
  }
  
  BOOST_FOREACH(const std::string& r, url_v) {
    if (++count == 5)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("classmate_list");
    sec->SetValue("classmate_url", r);
    sec->SetValue("classmate_name", name_v[count]);

    if(!strVipIconId.empty() && id_v[count] == lexical_cast<std::string>(feed->actor)) {
        sec->SetValue("userVipIconId", strVipIconId);
        sec->SetValue("userVipIconLevel", strVipIconLevel);
        sec->ShowSection("userVipIconId");
    }
  } 

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");

  return true;
}
bool fill_signin(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_signin_comment",NULL);
  const std::string ftype = "1104";
  if (fx->find("f_type") == ftype) {
    dict->ShowSection("pingjia");
  } else {
    dict->ShowSection("pingjia_NOT");
  }
  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);

  } 
  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");

  return true;
}

bool fill_signin_ovi(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // for SOURCE
  // f_origin_type == 1, vType
  // rename to: TYPE_EQUAL_1 TYPE

  // condition: f_origin_type f_status_streamID f_origin_title
  // f_origin_url, f_origin_title
  // rename: FROM, URL, TITLE

  // condition: f_origin_image
  // rename to: WTIHIMG, URL, IMG
  {
    if (fx->find("f_origin_type") == "1") {
      dict->SetValueAndShowSection("vType", vType_, "TYPE_EQUAL_1");
    } else {
      dict->ShowSection("TYPE_EQUAL_1_NOT");
    }
    If(fx, dict, "f_origin_image");
    If(fx, dict, "f_origin_title");
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);

  }

  If(fx, dict, "f_signin_comment",NULL);
// write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;

}



//-----------------------3100----------------------------------------------------------------------------------------
bool fill_page_dell_gossip_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
 If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_owner_certify",NULL);
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_page_dell_page_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_page_image","f_page_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;
  }
  If(fx, dict, "f_page_certify_icon",NULL);
  IfElse(fx, dict, "f_page_freestype_name","f_page_freestype_name");
  If(fx, dict, "f_page_digest","f_page_digest");
  IfElse(fx, dict, "f_page_freestype_action","f_page_freestype_action");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_page_dell_share_blog(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);

  // write_assign_share_blog_url
  std::string ss = fx->find("f_share_blog_url");
  if (ss.find('?') != std::string::npos)
    dict->SetValue("symbol", "&");
  else
    dict->SetValue("symbol", "?");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_dell_share_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_album_photo_image","f_share_album_photo_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_dell_share_link(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  IfElse(fx, dict, "f_share_shared_link_title","f_share_shared_link_title");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_dell_share_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_dell_share_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_dell_status_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
 
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);


  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_from_certify",NULL);
  // write_IfWithValue
  {
  std::string value = fx->find("visitType");
  if (!value.empty() && value == "newsfeed")
    dict->ShowSection("vTypeminifeed_");
  else
    dict->ShowSection("vTypeminifeed_NOT");
  }
  If(fx, dict, "isNewsFeed",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_dell_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_photo_image","f_photo_image");

  // write_pair
  std::string k1 = fx->find("f_photo_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_photo_owner_id_f_from_id");
  else
    dict->ShowSection("f_photo_owner_id_f_from_id_NOT");
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }
  If(fx, dict, "f_photo_digest","f_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_dell_blog_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_pair
  std::string k1 = fx->find("f_blog_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_blog_owner_id_f_from_id");
  else
    dict->ShowSection("f_blog_owner_id_f_from_id_NOT");
  If(fx, dict, "f_blog_digest","f_blog_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_dell_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
 
  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_dell_photo_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_assign_num
  int inum = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      inum += atoi(a.c_str());
  }
  dict->SetIntValue("num", inum);

  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> iv = fx->find_v("f_album_photo_image");
  BOOST_FOREACH(const std::string& img, iv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", img);
	  MAKE_VALUE("f_album_photo_url", photo_limit);
      MAKE_VALUE("f_album_photo_id", photo_limit);
    }
  }
  If(fx, dict, "f_from_certify",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


/*bool fill_page_dell_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_photo_image","f_photo_image");

  // write_pair
  std::string k1 = fx->find("f_photo_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_photo_owner_id_f_from_id");
  else
    dict->ShowSection("f_photo_owner_id_f_from_id_NOT");
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }
  If(fx, dict, "f_photo_digest","f_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}  */


bool fill_page_dell_head_upload(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_userinfo_image","f_userinfo_image");
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

 

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}




bool fill_page_dell_gossip_publish_admin(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_owner_certify",NULL);
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_page_dell_share_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_user_image","f_share_user_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "f_share_user_certify",NULL);
  If(fx, dict, "f_share_user_digest","f_share_user_digest");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_dell_video_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_page_album_video_brief","f_page_album_video_brief");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


//---------------------------3200----------------------------------------------------------------
bool fill_page_nike_sportswear_gossip_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
 If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_owner_certify",NULL);
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_page_nike_sportswear_page_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_page_image","f_page_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;
  }
  If(fx, dict, "f_page_certify_icon",NULL);
  IfElse(fx, dict, "f_page_freestype_name","f_page_freestype_name");
  If(fx, dict, "f_page_digest","f_page_digest");
  IfElse(fx, dict, "f_page_freestype_action","f_page_freestype_action");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_page_nike_sportswear_share_blog(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);

  // write_assign_share_blog_url
  std::string ss = fx->find("f_share_blog_url");
  if (ss.find('?') != std::string::npos)
    dict->SetValue("symbol", "&");
  else
    dict->SetValue("symbol", "?");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_sportswear_share_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_album_photo_image","f_share_album_photo_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_sportswear_share_link(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  IfElse(fx, dict, "f_share_shared_link_title","f_share_shared_link_title");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_sportswear_share_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_sportswear_share_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_sportswear_status_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
 
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);


  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_from_certify",NULL);
  // write_IfWithValue
  {
  std::string value = fx->find("visitType");
  if (!value.empty() && value == "newsfeed")
    dict->ShowSection("vTypeminifeed_");
  else
    dict->ShowSection("vTypeminifeed_NOT");
  }
  If(fx, dict, "isNewsFeed",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_sportswear_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_photo_image","f_photo_image");

  // write_pair
  std::string k1 = fx->find("f_photo_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_photo_owner_id_f_from_id");
  else
    dict->ShowSection("f_photo_owner_id_f_from_id_NOT");
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }
  If(fx, dict, "f_photo_digest","f_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_sportswear_blog_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_pair
  std::string k1 = fx->find("f_blog_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_blog_owner_id_f_from_id");
  else
    dict->ShowSection("f_blog_owner_id_f_from_id_NOT");
  If(fx, dict, "f_blog_digest","f_blog_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_sportswear_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
 
  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_sportswear_photo_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_assign_num
  int inum = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      inum += atoi(a.c_str());
  }
  dict->SetIntValue("num", inum);

  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> iv = fx->find_v("f_album_photo_image");
  BOOST_FOREACH(const std::string& img, iv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", img);
	  MAKE_VALUE("f_album_photo_url", photo_limit);
      MAKE_VALUE("f_album_photo_id", photo_limit);
    }
  }
  If(fx, dict, "f_from_certify",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


/* bool fill_page_nike_sportswear_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_photo_image","f_photo_image");

  // write_pair
  std::string k1 = fx->find("f_photo_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_photo_owner_id_f_from_id");
  else
    dict->ShowSection("f_photo_owner_id_f_from_id_NOT");
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }
  If(fx, dict, "f_photo_digest","f_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}  */


bool fill_page_nike_sportswear_head_upload(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_userinfo_image","f_userinfo_image");
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

 

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}




bool fill_page_nike_sportswear_gossip_publish_admin(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_owner_certify",NULL);
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_page_nike_sportswear_share_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_user_image","f_share_user_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "f_share_user_certify",NULL);
  If(fx, dict, "f_share_user_digest","f_share_user_digest");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_sportswear_video_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_page_album_video_brief","f_page_album_video_brief");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

//------------------3300------------------------------------------------------------------------

bool fill_page_reiz_gossip_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
 If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_owner_certify",NULL);
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_page_reiz_page_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_page_image","f_page_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;
  }
  If(fx, dict, "f_page_certify_icon",NULL);
  IfElse(fx, dict, "f_page_freestype_name","f_page_freestype_name");
  If(fx, dict, "f_page_digest","f_page_digest");
  IfElse(fx, dict, "f_page_freestype_action","f_page_freestype_action");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_page_reiz_share_blog(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);

  // write_assign_share_blog_url
  std::string ss = fx->find("f_share_blog_url");
  if (ss.find('?') != std::string::npos)
    dict->SetValue("symbol", "&");
  else
    dict->SetValue("symbol", "?");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_reiz_share_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_album_photo_image","f_share_album_photo_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_reiz_share_link(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  IfElse(fx, dict, "f_share_shared_link_title","f_share_shared_link_title");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_reiz_share_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_reiz_share_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_reiz_status_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
 
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);


  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_from_certify",NULL);
  // write_IfWithValue
  {
  std::string value = fx->find("visitType");
  if (!value.empty() && value == "newsfeed")
    dict->ShowSection("vTypeminifeed_");
  else
    dict->ShowSection("vTypeminifeed_NOT");
  }
  If(fx, dict, "isNewsFeed",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_reiz_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_photo_image","f_photo_image");

  // write_pair
  std::string k1 = fx->find("f_photo_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_photo_owner_id_f_from_id");
  else
    dict->ShowSection("f_photo_owner_id_f_from_id_NOT");
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }
  If(fx, dict, "f_photo_digest","f_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_reiz_blog_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_pair
  std::string k1 = fx->find("f_blog_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_blog_owner_id_f_from_id");
  else
    dict->ShowSection("f_blog_owner_id_f_from_id_NOT");
  If(fx, dict, "f_blog_digest","f_blog_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_reiz_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
 
  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_reiz_photo_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_assign_num
  int inum = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      inum += atoi(a.c_str());
  }
  dict->SetIntValue("num", inum);

  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> iv = fx->find_v("f_album_photo_image");
  BOOST_FOREACH(const std::string& img, iv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", img);
	  MAKE_VALUE("f_album_photo_url", photo_limit);
      MAKE_VALUE("f_album_photo_id", photo_limit);
    }
  }
  If(fx, dict, "f_from_certify",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


/* bool fill_page_reiz_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_photo_image","f_photo_image");

  // write_pair
  std::string k1 = fx->find("f_photo_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_photo_owner_id_f_from_id");
  else
    dict->ShowSection("f_photo_owner_id_f_from_id_NOT");
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }
  If(fx, dict, "f_photo_digest","f_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}  */


bool fill_page_reiz_head_upload(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_userinfo_image","f_userinfo_image");
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

 

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}




bool fill_page_reiz_gossip_publish_admin(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_owner_certify",NULL);
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_page_reiz_share_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_user_image","f_share_user_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "f_share_user_certify",NULL);
  If(fx, dict, "f_share_user_digest","f_share_user_digest");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_reiz_video_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_page_album_video_brief","f_page_album_video_brief");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


//---------------------------3400--------------------------------------------


bool fill_page_nike_basketball_gossip_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
 If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_owner_certify",NULL);
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_page_nike_basketball_page_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_page_image","f_page_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;
  }
  If(fx, dict, "f_page_certify_icon",NULL);
  IfElse(fx, dict, "f_page_freestype_name","f_page_freestype_name");
  If(fx, dict, "f_page_digest","f_page_digest");
  IfElse(fx, dict, "f_page_freestype_action","f_page_freestype_action");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_page_nike_basketball_share_blog(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);

  // write_assign_share_blog_url
  std::string ss = fx->find("f_share_blog_url");
  if (ss.find('?') != std::string::npos)
    dict->SetValue("symbol", "&");
  else
    dict->SetValue("symbol", "?");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_basketball_share_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_album_photo_image","f_share_album_photo_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_basketball_share_link(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  IfElse(fx, dict, "f_share_shared_link_title","f_share_shared_link_title");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_basketball_share_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_basketball_share_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_basketball_status_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
 
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);


  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_from_certify",NULL);
  // write_IfWithValue
  {
  std::string value = fx->find("visitType");
  if (!value.empty() && value == "newsfeed")
    dict->ShowSection("vTypeminifeed_");
  else
    dict->ShowSection("vTypeminifeed_NOT");
  }
  If(fx, dict, "isNewsFeed",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_basketball_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_photo_image","f_photo_image");

  // write_pair
  std::string k1 = fx->find("f_photo_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_photo_owner_id_f_from_id");
  else
    dict->ShowSection("f_photo_owner_id_f_from_id_NOT");
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }
  If(fx, dict, "f_photo_digest","f_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_basketball_blog_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_pair
  std::string k1 = fx->find("f_blog_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_blog_owner_id_f_from_id");
  else
    dict->ShowSection("f_blog_owner_id_f_from_id_NOT");
  If(fx, dict, "f_blog_digest","f_blog_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_basketball_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
 
  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_basketball_photo_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_assign_num
  int inum = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      inum += atoi(a.c_str());
  }
  dict->SetIntValue("num", inum);

  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> iv = fx->find_v("f_album_photo_image");
  BOOST_FOREACH(const std::string& img, iv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", img);
	  MAKE_VALUE("f_album_photo_url", photo_limit);
      MAKE_VALUE("f_album_photo_id", photo_limit);
    }
  }
  If(fx, dict, "f_from_certify",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


/* bool fill_page_nike_basketball_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_photo_image","f_photo_image");

  // write_pair
  std::string k1 = fx->find("f_photo_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_photo_owner_id_f_from_id");
  else
    dict->ShowSection("f_photo_owner_id_f_from_id_NOT");
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }
  If(fx, dict, "f_photo_digest","f_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}*/


bool fill_page_nike_basketball_head_upload(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_userinfo_image","f_userinfo_image");
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

 

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}




bool fill_page_nike_basketball_gossip_publish_admin(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_owner_certify",NULL);
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_page_nike_basketball_share_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_user_image","f_share_user_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "f_share_user_certify",NULL);
  If(fx, dict, "f_share_user_digest","f_share_user_digest");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_basketball_video_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_page_album_video_brief","f_page_album_video_brief");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

//--------------3500----------------------------------------------------------------

bool fill_page_nike_running_gossip_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
 If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_owner_certify",NULL);
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_page_nike_running_page_join(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_page_image","f_page_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;
  }
  If(fx, dict, "f_page_certify_icon",NULL);
  IfElse(fx, dict, "f_page_freestype_name","f_page_freestype_name");
  If(fx, dict, "f_page_digest","f_page_digest");
  IfElse(fx, dict, "f_page_freestype_action","f_page_freestype_action");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

bool fill_page_nike_running_share_blog(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);

  // write_assign_share_blog_url
  std::string ss = fx->find("f_share_blog_url");
  if (ss.find('?') != std::string::npos)
    dict->SetValue("symbol", "&");
  else
    dict->SetValue("symbol", "?");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_running_share_photo(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_album_photo_image","f_share_album_photo_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_running_share_link(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  IfElse(fx, dict, "f_share_shared_link_title","f_share_shared_link_title");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_running_share_music(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_running_share_video(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_running_status_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {

  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
 
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);


  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_reply_loop
  int count = -1;
  const std::vector<std::string> & reply_v = fx->find_v("f_reply_id");
  BOOST_FOREACH(const std::string& r, reply_v) {
    if (++count == 2)
      break;

    ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_reply_id");
    sec->SetValue("f_reply_id", r);
    MAKE_VALUE("f_reply_from_tinyimg", count);
    MAKE_VALUE("f_reply_body", count);
    MAKE_VALUE("f_reply_from_name", count);   
    MAKE_VALUE("f_reply_time", count);
    MAKE_VALUE("f_reply_type", count);
    MAKE_VALUE("f_reply_from_id", count);
    
  }

  If(fx, dict, "f_from_certify",NULL);
  // write_IfWithValue
  {
  std::string value = fx->find("visitType");
  if (!value.empty() && value == "newsfeed")
    dict->ShowSection("vTypeminifeed_");
  else
    dict->ShowSection("vTypeminifeed_NOT");
  }
  If(fx, dict, "isNewsFeed",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_running_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_photo_image","f_photo_image");

  // write_pair
  std::string k1 = fx->find("f_photo_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_photo_owner_id_f_from_id");
  else
    dict->ShowSection("f_photo_owner_id_f_from_id_NOT");
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }
  If(fx, dict, "f_photo_digest","f_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_running_blog_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  // write_pair
  std::string k1 = fx->find("f_blog_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_blog_owner_id_f_from_id");
  else
    dict->ShowSection("f_blog_owner_id_f_from_id_NOT");
  If(fx, dict, "f_blog_digest","f_blog_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_running_blog_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);
 
  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_running_photo_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");

  If(fx, dict, "f_from_certify",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

  // write_assign_num
  int inum = 0;
  std::vector<std::string> av = fx->find_v("f_album_count");
  BOOST_FOREACH(const std::string& a, av) {
      inum += atoi(a.c_str());
  }
  dict->SetIntValue("num", inum);

  // write_album_loop
  int photo_limit = -1;
  std::vector<std::string> iv = fx->find_v("f_album_photo_image");
  BOOST_FOREACH(const std::string& img, iv) {
    if (++photo_limit < 3) {
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_album_photo_image");
      sec->SetValue("f_album_photo_image", img);
	  MAKE_VALUE("f_album_photo_url", photo_limit);
      MAKE_VALUE("f_album_photo_id", photo_limit);
    }
  }
  If(fx, dict, "f_from_certify",NULL);

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


/* bool fill_page_nike_running_photo_publish_common(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_photo_image","f_photo_image");

  // write_pair
  std::string k1 = fx->find("f_photo_owner_id");
  std::string k2 = fx->find("f_from_id");
  if (k1 == k2)
    dict->ShowSection("f_photo_owner_id_f_from_id");
  else
    dict->ShowSection("f_photo_owner_id_f_from_id_NOT");
  // write_IfWithValue
  {
  if (WeiredType(feed->type).type == 2014)
    dict->ShowSection("f_type2014");
  else
    dict->ShowSection("f_type2014_NOT");
  }
  If(fx, dict, "f_photo_digest","f_photo_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}  */


bool fill_page_nike_running_head_upload(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_userinfo_image","f_userinfo_image");
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_from_certify_icon",NULL);

  std::string value = fx->find("{f_from_url");
  if (value.empty()) {
    dict->SetValue("{f_from_url", "http://page.renren.com/{{f_from_id}}?");
  } else {
    dict->SetValue("{f_from_url",value);
  }

 

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}




bool fill_page_nike_running_gossip_publish_admin(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_from_tinyimg","f_from_tinyimg");
  If(fx, dict, "f_gossip_owner_certify",NULL);
  If(fx, dict, "f_gossip_digest","f_gossip_digest");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}



bool fill_page_nike_running_share_friend(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "f_share_user_image","f_share_user_image");

  // write_name_loop
  std::set<std::string> filter_id;
  int idcount = 0;
  std::vector<std::string> vs = fx->find_v("f_from_id");
  BOOST_FOREACH(std::string & id, vs) {
    if (filter_id.find(id) == filter_id.end()) {
      filter_id.insert(id);
      ctemplate::TemplateDictionary* sec = dict->AddSectionDictionary("f_from_name");
      sec->SetValue("f_from_id", id);
      MAKE_VALUE("f_from_name", idcount);
    }
    idcount++;    
  }
  If(fx, dict, "f_share_user_certify",NULL);
  If(fx, dict, "f_share_user_digest","f_share_user_digest");
  If(fx, dict, "f_share_comment","f_share_comment");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}


bool fill_page_nike_running_video_publish(const feed::FeedData *feed, FeedXml* fx, ctemplate::TemplateDictionary* dict) {
  If(fx, dict, "idef",NULL);
  If(fx, dict, "f_page_album_video_brief","f_page_album_video_brief");

  // write_del_no_assign_typeText
  if (dict_delete_)
    dict_delete_->ShowSection("typeText_NOT");
  return true;
}

