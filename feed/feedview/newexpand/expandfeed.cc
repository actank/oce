#include "feed/expandfeed.h"
#include "feed/logic_action.h"
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "ctemplate/template.h"
#include "ctemplate/template_dictionary.h"

#include "base/logging.h"
#include "base/hashmap.h"
// #include "base/once.h"
#include "feed/rfeed_adapter.h"
#include "feed/feedxml.h"
#include "cwf/site.h"
#include "feed/headurl.h"
#include "feed/ilikeit_adapter.h"

/*
模板转换
remove ==============
 [0]
 <#compress>
 ${"\r"}

replace ==============
${ => {{
} => }}
<#_ => {{#
??> => }}

</#if> => {{/}} => 手工添加

dict ===============
f_blog_type == 1  f_blog_type可能不为1么？
构建 header.ftl 中的字典
<#list
<#assign

TODO: time format modifier

还要访问 user 信息 构建 userID
还要访问 selfzancount

function() {
  var reply = {"replyList":[
    {"id":"3073535535","replyer_tinyurl":"http://hdn.xnimg.cn/photos/hdn311/20090517/0430/tiny_7ddY_77325p204234.jpg","replyContent":"整点儿蚁力神吧","ubname":"银鑫","replyTime":"2010-03-23 17:04","type":'0',"reply_layer":0,"ubid":"81120"} ,
    {"id":"3073537981","replyer_tinyurl":"http://hdn.xnimg.cn/photos/hdn121/20091027/2130/tiny_Z39X_15487f019116.jpg","replyContent":"回复银鑫: 这个不是都倒闭了吗？","ubname":"崔浩波","replyTime":"2010-03-23 17:05","type":'0',"reply_layer":0,"ubid":"66271"}
    ],"ownerid":66271,"type":"status"
  };
getReplyOfDoingFromJSON( reply , '1085860482' ,'227366242' , '2','3','','',true) })(); renderStatusFeed('feed2436467054','http://status.renren.com/getdoing.do?id=66271&doingId=1085860482&ref=newsfeed&sfet=502&fin=4'); </script> </div> <a href="javascript:void(0);" stats="NF_X-hide" class="delete" onclick="readHomeFeed('2436467054')">删除</a> </li>
*/

namespace xce {

  const std::string FeedXml::tmp = std::string();

template<typename Target, typename Source>
Target lexical_cast(const Source & arg, const Target & default_target = Target()) {
  try {
    return boost::lexical_cast<Target>(arg);
  }
  catch (boost::bad_lexical_cast* e) {}
  return default_target;
}
//
// <#include "1|4|0">
// 1 表示 feed type
// 4 表示业务，如：0 是网站，99 是page
// 0 对应于 xml 中的 v，
//   如果 v=1.0 此处为 1
//   如果 v=2.0 此处为 2
//   如果没有 v 此处为 0
// 

// CAUTION: 为了支持minifeed，改变了Source 的定义，这里需要去除影响
// 故 & WS_FLAG
std::string GetTemplateFilename(int type, WeiredSource source, int version) {
  for (int i=0; FeedTypeTable[i].type; ++i) {
    if (FeedTypeTable[i].type == type && 
      FeedTypeTable[i].source == (source & WS_FLAG)) {
      char str[256];
      sprintf(str, "%s.%d.tpl", FeedTypeTable[i].filename, version);
      return str;
    }
  }
  return std::string();
}

std::string GetTemplateFilename2(int type, WeiredSource source, int version) {
  for (int i=0; FeedTypeTable[i].type; ++i) {
    if (FeedTypeTable[i].type == type && 
      FeedTypeTable[i].source == (source & WS_FLAG)) {
      char str[256];
      sprintf(str, "%s.%d.tpl2", FeedTypeTable[i].filename, version);
      return str;
    }
  }
  return std::string();
}

void IfElse(FeedXml *fx, ctemplate::TemplateDictionary* dict
        , const char * key, const char * section_value_name = 0) {

  std::string value = fx->find(key);
  if (!value.empty()) {
    if (section_value_name)
      dict->SetValueAndShowSection(section_value_name, value, key);
    else
      dict->ShowSection(key);
  } else {
    std::string not_key(key);
    not_key += "_NOT";
    dict->ShowSection(not_key);
  }
}

void If(FeedXml *fx, ctemplate::TemplateDictionary* dict
        , const char *key, const char * section_value_name = 0) {
  std::string value = fx->find(key);
  if (!value.empty()) {
    if (section_value_name)
      dict->SetValueAndShowSection(section_value_name, value, key);
    else
      dict->ShowSection(key);
  }
}

void IfNot(FeedXml *fx, ctemplate::TemplateDictionary* dict
        , const char *key,  const char * section_value_name = 0) {
  std::string value = fx->find(key);
  if (value.empty()) {
      std::string not_key(key);
      not_key += "_NOT";
      dict->ShowSection(not_key);
  }
}

void IfNotElse(FeedXml *fx, ctemplate::TemplateDictionary* dict
            , const char *key, const char * section_value_name = 0) {
  
  if (!key)
    return;
  std::string value = fx->find(key);
  if (value.empty()) {
    std::string not_key(key);
    not_key += "_NOT";
    dict->ShowSection(not_key);
  } else {
    if (section_value_name)
      dict->SetValueAndShowSection(section_value_name, value, key);
    else
      dict->ShowSection(key);
  }
}

struct DictBuilder {
  // 整个 fill 周期需要共享一些变量
  std::string vType_;
  int reply_limit_;
  unsigned int render_;
  ctemplate::TemplateDictionary * dict_delete_;

  DictBuilder() : reply_limit_(2), dict_delete_(0) {}

  bool BuildDictionary(const feed::FeedData *feed, FeedXml* fx
    , cwf::User * u, WeiredSource source
    , ctemplate::TemplateDictionary* dict, int owner = -1, const ilikeit::LikeInfo* likeinfo = NULL,  const std::string & template_file = "", int index = WS_HOME, bool  isAdmin = false);

  
  typedef bool (DictBuilder::*GenerateProc)(const feed::FeedData *, FeedXml *, ctemplate::TemplateDictionary *);
  typedef bool (DictBuilder::*GenerateProc2)(const feed::FeedData *, FeedXml *, ctemplate::TemplateDictionary *, cwf::User *, WeiredSource, int, const ilikeit::LikeInfo*, const std::string &, int index, bool isAdmin);

  static GenerateProc FindEntry(int type, int source) {
    struct Entry {
      int type;
      int source;
      GenerateProc proc;
      GenerateProc2 proc2;
       
    };

    #include "feed/table.inc"

    for(const Entry *e=arr; e->type; ++e) {
      if (e->type == type && e->source == source) {
          return e->proc;
      }
    }
    return 0;
  }

  static GenerateProc2 FindEntry2(int type, int source) {
    struct Entry {
      int type;
      int source;
      GenerateProc proc;
      GenerateProc2 proc2;
       
    };

    #include "feed/table.inc"

    for(const Entry *e=arr; e->type; ++e) {
      if (e->type == type && e->source == source) {
          return e->proc2;
      }
    }
    return 0;
  }

  #include "feed/body.inc"

  bool Genrate(const feed::FeedData *feed, int index, FeedXml* fx, ctemplate::TemplateDictionary* dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo* likeinfo,  const std::string & template_file, bool isAdmin = false) {
    GenerateProc proc  = FindEntry(WeiredType(feed->type).type, index);
    if (proc) {
      return ((*this).*proc)(feed, fx, dict);
    }
    GenerateProc2 proc2  = FindEntry2(WeiredType(feed->type).type, index);
    if (proc2) {
      return ((*this).*proc2)(feed, fx, dict, u, source, owner, likeinfo, template_file, index, isAdmin);
    } 
    std::string logic_str = "kdsfjdkf";
    ConditionAction la(logic_str, feed, fx, dict, u, source, owner, likeinfo, template_file, index, isAdmin); 
    std::map<std::string, LogicAction*> logic_map = LogicAction::ParseLogicStr(logic_str, feed, fx, dict, u, source, owner, likeinfo, template_file, index, isAdmin);

    bool isNewExpand = true;
    

    return fill_default(feed, fx, dict);
  }

};




bool DictBuilder::BuildDictionary(const feed::FeedData *feed, FeedXml* fx
     , cwf::User * u, WeiredSource source
     , ctemplate::TemplateDictionary* dict, int owner, const ilikeit::LikeInfo* likeinfo,  const std::string & template_file, int index, bool isAdmin) {
  // f_blog_deny_id 支持
  // 在minifeed中，对某些人实施过滤
  if (source & WS_MINIFEED) {
    if (601 == WeiredType(feed->type).type) {
      // BOOST_FOREACH(StringMap& map, fx->body) {
      std::vector<std::string> deny_id = fx->find_v("f_blog_deny_id");
      if (u) {
        BOOST_FOREACH(std::string& id, deny_id) {
        if (u->id() == lexical_cast(id, 0))
          return false;
        }
      }
    }
  }
  
  // 把 f_from_tinyimg 地址补全,暂时先用着
//   VectorMap::iterator vit = fx->ls.find("f_from_tinyimg");
//   if (fx->ls.end() != vit){
//     BOOST_FOREACH(std::string& img, vit->second) {
//       if (!img.empty()) {
//         img = HeadUrl::FromPart(img);
//       }
//     }
//   }
  StringMap::iterator fromimgit = fx->body.find("f_from_tinyimg");
  if (fromimgit != fx->body.end())
    fromimgit->second = HeadUrl::FromPart(fromimgit->second);

  if (8007 == WeiredType(feed->type).type) { // 对8007的一些空缺属性填充
    const std::string & from = fx->find("f_entry_from_tinyimg");
    if (!from.empty())
      fx->body.insert(std::make_pair("f_from_tinyimg",from));
    const std::string & id = fx->find("f_entry_from_id");
    if (!id.empty()) 
      fx->body.insert(std::make_pair("f_from_id",id));
    const std::string & name = fx->find("f_from_name");
    if (!name.empty()) 
      fx->body.insert(std::make_pair("f_from_name",name));    
  }

  // 2 f_entry_attachments_attachment_src[0] 
  // attachments 固然只用在了asp/PhotoPublishMore, 但是对于其他类型来说
  // 需要第一个 attachment
  /*if (!fx->attachment.empty()) {
    const StringMap & map = *(fx->attachment.begin());
    for (StringMap::const_iterator i=map.begin(); i!=map.end(); ++i) {
      //dict->SetValue(i->first, i->second);
      fx->body.front().insert(std::make_pair(i->first,i->second));
    }
  }*/

  // 3 body

  fx->body["f_type"] = lexical_cast<std::string>(WeiredType(feed->type).type);
  for (StringMap::const_iterator i=fx->body.begin(); i!=fx->body.end(); ++i) {
    dict->SetValue(i->first, i->second);
  }

  // 3 photo[0] 
  // photo 固然只用在了PhotoPublishMore, 但是对于其他类型来说
  // 需要第一个 photo
  /*if (!fx->photo.empty()) {
    const StringMap & map = *(fx->photo.begin());
    for (StringMap::const_iterator i=map.begin(); i!=map.end(); ++i) {
      dict->SetValue(i->first, i->second);
    }
  }*/
 
  std::string xml_version = fx->template_version;

  // win32 long 是32位的，不能调用SetIntValue
  dict->SetFormattedValue("id", "%llu", feed->feed);

//   if (u) { // 多余了
//     dict->SetIntValue("userID", u->id());
//     dict->SetValue("userName", u->name());
//   }
  
  // for AVATAR
  // f_from_id, vType, f_from_tinyimg
  // rename to: ID, TYPE, TINYIMG
  {
    ctemplate::TemplateDictionary* sub_dict = dict->AddIncludeDictionary("AVATAR_1");
    //  xml_version == "1" ? "AVATAR_1" : "AVATAR_1");
    sub_dict->SetFilename("avatar.ftl.1.tpl");
    sub_dict->SetValue("f_from_id", fx->find("f_from_id"));
    sub_dict->SetValue("f_from_name", fx->find("f_from_name"));
    sub_dict->SetValue("f_from_tinyimg", fx->find("f_from_tinyimg"));
    sub_dict->SetValue("vType", vType_);
    sub_dict->SetFormattedValue("id", "%llu", feed->feed); 

    sub_dict->SetIntValue("f_type", WeiredType(feed->type).type);
  }

  // for SOURCE
  // f_origin_type == 1, vType
  // rename to: TYPE_EQUAL_1 TYPE

  // condition: f_origin_type f_status_streamID f_origin_title
  // f_origin_url, f_origin_title
  // rename: FROM, URL, TITLE

  // condition: f_origin_image
  // rename to: WTIHIMG, URL, IMG
  {
    ctemplate::TemplateDictionary* sub_dict = dict->AddIncludeDictionary(
      xml_version == "1" ? "SOURCE_1" : "SOURCE");
    sub_dict->SetFilename(xml_version == "1" ? "source.ftl.1.tpl" : "source.ftl.tpl");
    if (fx->find("f_origin_type") == "1") {
      sub_dict->SetValueAndShowSection("vType", vType_, "TYPE_EQUAL_1");
    } else {
      sub_dict->ShowSection("TYPE_EQUAL_1_NOT");
    }
    sub_dict->SetValue("f_origin_url",fx->find("f_origin_url"));
    If(fx, sub_dict, "f_origin_image");
    If(fx, sub_dict, "f_origin_title");
    sub_dict->SetValue("f_origin_title", fx->find("f_origin_title"));
    sub_dict->SetValue("f_origin_image", fx->find("f_origin_image"));
  }

  // for TOOLBAR_1 =  DELETE1 + DELETE2 + focus
  // TOOLBAR_NOFOCUS_1 =  DELETE1 + DELETE2 
  {
    ctemplate::TemplateDictionary* inc = dict->AddIncludeDictionary("TOOLBAR_1");
    inc->SetFilename("toolbar.ftl.1.tpl2");

    ctemplate::TemplateDictionary* inc_nofocus = dict->AddIncludeDictionary("TOOLBAR_NOFOCUS_1");
    inc_nofocus->SetFilename("toolbar_nofocus.ftl.1.tpl2");

    ctemplate::TemplateDictionary* del_inc = 0;

    inc->SetFormattedValue("id", "%llu", feed->feed);
    inc->SetIntValue("f_type", WeiredType(feed->type).type);
    inc->SetValue("f_from_id", fx->find("f_from_id"));
    inc->SetValue("f_from_name", fx->find("f_from_name"));

    inc_nofocus->SetFormattedValue("id", "%llu", feed->feed);
    inc_nofocus->SetIntValue("f_type", WeiredType(feed->type).type);
    inc_nofocus->SetValue("f_from_id", fx->find("f_from_id"));
    inc_nofocus->SetValue("f_from_name", fx->find("f_from_name"));


    std::string merge_count = fx->find("merge_feed_count");
    std::vector<std::string> mergev = fx->find_v("merge_feed_fid");
    std::string fids;

    unsigned int flag = render_;
    if (flag & WS_MINIFEED) {
      inc->ShowSection("isNewsFeed_NOT");
      inc_nofocus->ShowSection("isNewsFeed_NOT");
    } else {
      inc->ShowSection("isNewsFeed");
      inc_nofocus->ShowSection("isNewsFeed");
      if (u && feed->actor != u->id()) {
        inc->ShowSection("SELF_NOT"); //只有toolbar需要show SELF_NOT
      }
    }

    if (u) {
      std::string uid = lexical_cast<std::string>(u->id());
      if ((uid == fx->find("f_from_id")) || isAdmin) {
        inc->ShowSection("SELF");
        inc_nofocus->ShowSection("SELF");
  
        if (flag & WS_MINIGROUP) {
          inc_nofocus->ShowSection("MINIGROUP");
        } else {
          inc_nofocus->ShowSection("MINIGROUP_NOT");
        }
      }

      inc->SetIntValue("userID", u->id());
      inc_nofocus->SetIntValue("userID", u->id());
    }

    // merge
    if (!merge_count.empty()) {
      std::string isNewVersionVideoFeed = fx->find("newversion"); //识别是视频分享是不是新版

      if (isNewVersionVideoFeed.empty()) {  //视频分享老版本才显示相似新鲜事条数
        del_inc = dict->AddIncludeDictionary("DELETE_1"); //这里的DELETE现在仅仅表示“显示相似的几条新鲜事”
        del_inc->SetFilename("delete.ftl.1.tpl2");

        del_inc->ShowSection("merge_feed_count");
        // 手工拼接 feedIDs
        del_inc->SetValue("merge_feed_count", merge_count); 
        // inc->ShowSection("typeText_NOT");
        fids = boost::join(mergev, ",");
        del_inc->SetValue("feedIDs", fids);

      }
      
    }

    char first_id[40];
    sprintf(first_id,"%llu",feed->feed);

    fids = "," + fids;
    std::string id_list = first_id  + fids;
    dict->SetValue("id_list", id_list);  //在顶层模板填充id_list，目前只有share_video需要
    
    dict_delete_ = del_inc;

  }


   // 专门处理评论类新鲜事包括(2901, 2902, 2906, 2905)  
  if (2901 == WeiredType(feed->type).type  || 2902 == WeiredType(feed->type).type || 2906 == WeiredType(feed->type).type || 2905 == WeiredType(feed->type).type) {
     
    // for AVATAR_COMMENT
    // f_from_id, vType, f_from_tinyimg
    // rename to: ID, TYPE, TINYIMG
    {
      ctemplate::TemplateDictionary* sub_dict = dict->AddIncludeDictionary("AVATAR_COMMENT");
      //  xml_version == "1" ? "AVATAR_1" : "AVATAR_1");
      sub_dict->SetFilename("avatar_comment.ftl.1.tpl2");
      sub_dict->SetValue("new_comment_from_id", fx->find("f_comment_from_id"));
      sub_dict->SetValue("new_comment_from_name", fx->find("f_comment_from_name"));

      std::string new_comment_from_tinyimg = fx->find("f_comment_from_tinyimg");
      new_comment_from_tinyimg = HeadUrl::FromPart(new_comment_from_tinyimg);
      sub_dict->SetValue("new_comment_from_tinyimg", new_comment_from_tinyimg);

      sub_dict->SetValue("vType", vType_);
      sub_dict->SetFormattedValue("id", "%llu", feed->feed); 

      sub_dict->SetIntValue("f_type", WeiredType(feed->type).type);
    }

    // for TOOLBAR_COMMENT =  DELETE1 + DELETE2 + focus  评论类新鲜事，目前处理2901, 2902, 2906, 2905
    ctemplate::TemplateDictionary* inc = dict->AddIncludeDictionary("TOOLBAR_COMMENT");
    inc->SetFilename("toolbar_comment.ftl.1.tpl2");

    inc->SetFormattedValue("id", "%llu", feed->feed);
    inc->SetIntValue("f_type", WeiredType(feed->type).type);
    inc->SetValue("new_comment_from_id", fx->find("f_comment_from_id"));
    inc->SetValue("new_comment_from_name", fx->find("f_comment_from_name"));


    unsigned int flag = render_;
    if (flag & WS_MINIFEED) {
      inc->ShowSection("isNewsFeed_NOT");
    } else {
      inc->ShowSection("isNewsFeed");
      if (u && feed->actor != u->id()) {
        inc->ShowSection("SELF_NOT"); //只有toolbar需要show SELF_NOT
      }
    }

    if (u) {
      std::string uid = lexical_cast<std::string>(u->id());
      if (uid == fx->find("f_comment_from_id")) {
        inc->ShowSection("SELF");
      }

      inc->SetIntValue("userID", u->id());
    }


  }


  // TODO: index 是没有必要的
  return Genrate(feed, WS_HOME, fx, dict, u, source, owner, likeinfo, template_file, isAdmin);
} // BuildDictionary

bool Expand(const feed::FeedData * feed, FeedXml * fx
            , cwf::User * u, int owner, WeiredSource source
            , const ilikeit::LikeInfo* likeinfo
            , const std::string & template_file
            , int index
            , std::string * output
            , bool isAdmin) {
  
  ctemplate::TemplateDictionary dict("");

  // 构建 header.ftl.1
// <#assign vType = "newsfeed"/>
// <#assign isNewsFeed = true/>
// <#if visitType[0]=="minifeed">
// <#assign vType = "minifeed"/>
// <#assign isNewsFeed = false/>
// </#if>
// <#assign vType = vType+"&sfet="+f_type[0]+"&fin="+feedIndex[0]/>

  std::string vType;
  unsigned int flag = source;

  dict.SetIntValue("feedIndex", index);
  
  // 依照java做法，塞入fx可能不是最优的做法
  StringMap & m = fx->body;
  if (owner)
    m.insert(std::make_pair("interviewee", lexical_cast<std::string>(owner)));

  if (u) {
    m.insert(std::make_pair("userID", lexical_cast<std::string>(u->id())));
    m.insert(std::make_pair("userName", u->name()));
  }

  if (flag & WS_HOT) {
    vType = "hot";
  } else if (flag & WS_LIVE) {
    vType = "liv";
  } else if (flag & WS_FAVORITE) {
    vType = "fav";
  }

  if (flag & WS_MINIFEED) {
    vType += "minifeed";
    m.insert(std::make_pair("visitType", "minifeed"));
  } else {
    vType += "newsfeed";
    m.insert(std::make_pair("visitType", "newsfeed"));
    m.insert(std::make_pair("isNewsFeed", "1"));
  }
  vType += "&sfet=";
  vType += lexical_cast<std::string>(WeiredType(feed->type).type);
  vType += "&fin=" + lexical_cast<std::string>(index);
  vType += "&ff_id=" + fx->find("f_from_id");

  m.insert(std::make_pair("vType", vType));


  // 处理 "赞"
  if (likeinfo) {
    int total = likeinfo->totalCount;
    if (likeinfo->selfCount) {
      total -= 1;// 去除自己
      if (total < 0) {
        LOG_F(WARNING) << "zan error total :" << total;
      }
      //dict.ShowSection("selfzancount");
      m.insert(std::make_pair("selfzancount", lexical_cast<std::string>(likeinfo->selfCount)));
    }
    //else 
    //  dict.ShowSection("selfzancount_NOT");
    if (total > 0) {
      m.insert(std::make_pair("totalzancount", lexical_cast<std::string>(total)));
    }
    //dict.SetIntValue("selfzancount", likeinfo->selfCount);
    //dict.SetIntValue("totalzancount", total);
  }

  DictBuilder g;
  g.vType_ = vType;
  g.render_ = source;
  fx->template_file = template_file; // 703类型会改动文件模版


  if (!g.BuildDictionary(feed, fx, u, source, &dict, owner, likeinfo, template_file, index, isAdmin))
    return false;

  ctemplate::Template* tpl = ctemplate::Template::GetTemplate(
    fx->template_file
    , ctemplate::DO_NOT_STRIP);

  if (!tpl) {
    LOG(LS_ERROR) << "tpl not found: " << template_file;
    return false;
  }

  return tpl->Expand(output, &dict);
}

bool Expand(const feed::FeedData * feed, cwf::User * u
            , int owner
            , WeiredSource source
            , const ilikeit::LikeInfo* likeinfo
            , int index
            , std::string * output
            , bool isAdmin) {
  // parse feed->xml to StringMap
  // template filename
  // get Template *
  // Build Dictionary
  // Expand
  // 尽早判断返回

  std::string packed_xml("<?xml version=\"1.0\" encoding=\"UTF-8\"?><feeds>");
  packed_xml += feed->xml;
  packed_xml += "</feeds>";

  FeedXml fx;
  int ret = ParseXml(packed_xml, &fx);
  if (0 != ret) {
    LOG(LS_WARNING) << "xml parse error: " << ret
      << " id: " << feed->feed;
    return false;
  }

  if (fx.body.empty()) {
    LOG(INFO) << "empty body, fid:" << feed->feed;
    return false;
  }

  int version  = 0;
  {
    if (!fx.template_version.empty())
      version = atoi(fx.template_version.c_str());
  }

  // <== TODO: feed->type, TypeConfigTable, fx->version
  std::string filename;
  if (source & (WS_HOT | WS_LIVE | WS_FAVORITE)) {
    filename = GetTemplateFilename2(WeiredType(feed->type).type, source, version);
  } else {
    if (true || u->id() % 1000 == 708) {
      filename = GetTemplateFilename2(WeiredType(feed->type).type, source, version);
    } else {
      filename = GetTemplateFilename(WeiredType(feed->type).type, source, version);
    }
  }



  if (filename.empty()) {
    LOG(LS_WARNING) << "tpl not support type: " << WeiredType(feed->type).type
      << " (" << feed->type
      << ") version: " << version
      << " fid: " << feed->feed;
    return false;
  }

  return Expand(feed, &fx, u, owner, source, likeinfo, filename, index, output, isAdmin);
}

bool RawExpand(int type, const char* raw_xml
               , const std::string & template_file
               , std::string * output) {
  std::string xml("<?xml version=\"1.0\" encoding=\"UTF-8\"?><feeds>");
  xml += raw_xml;
  xml += "</feeds>";

  FeedXml fx;
  int ret = ParseXml(xml, &fx);
  ASSERT(0 == ret);
  ASSERT(!fx.body.empty());

  feed::FeedDataPtr feed = new feed::FeedData;
  feed->type = type;

  ctemplate::TemplateDictionary dict("");
  cwf::User u("Fake User", 0);
  DictBuilder g;
  bool f = g.BuildDictionary(feed.get(), &fx, &u, WS_HOME, &dict);
  ASSERT(f);

  feed = 0;

  ctemplate::Template* tpl = ctemplate::Template::GetTemplate(
    template_file
    , ctemplate::STRIP_WHITESPACE);
  ASSERT(tpl);

  if (!tpl)
    return false;
  f = tpl->Expand(output, &dict);
  ASSERT(f);
  return f;
}

bool RawExpand(const std::string & raw_xml, const std::string & raw_tpl
      , ExpandResult * result, bool dump) {
  std::string xml("<?xml version=\"1.0\" encoding=\"UTF-8\"?><feeds>");
  xml += raw_xml;
  xml += "</feeds>";

  FeedXml fx;
  int ret = ParseXml(xml, &fx);
  ASSERT(0 == ret);
  ASSERT(!fx.body.empty());

  if (0 != ret) {
    std::ostringstream oss;
    oss << "xml parse error, code: " << ret << " detail: " 
      << XML_ErrorString((XML_Error)ret);
    result->err = oss.str();
    return false;
  }

  std::string string_type = fx.find("f_type");
  if (string_type.empty()) {
    result->err = "xml without feed type.";
    return false;
  }
  int type = lexical_cast(string_type, 0);
  if (0 == type) {
    result->err = "xml illigle feed type.";
    return false;
  }

  feed::FeedDataPtr feed = new feed::FeedData;
  feed->type = type;

  ctemplate::TemplateDictionary dict("");
  cwf::User u("Fake User", 0);
  DictBuilder g;
  bool f = g.BuildDictionary(feed.get(), &fx, &u, WS_HOME, &dict);
  ASSERT(f);
  feed = 0;

  if (!f) {
    result->err = "dictionary build failed. no detail info.";
    return false;
  }

  ctemplate::Template *tpl = ctemplate::Template::StringToTemplate(raw_tpl, ctemplate::DO_NOT_STRIP);
  if (!tpl) {
    result->err = "illegal tpl.";
    return false;
  }

  f = tpl->Expand(&result->output, &dict);
  if (!f) {
    result->err = "expand failed. no detail info.";
    return false;
  }

  if (dump) {
    tpl->DumpToString("test", &result->dump_tpl);
    dict.DumpToString(&result->dump_dict);
  }

  return true;
}

}
