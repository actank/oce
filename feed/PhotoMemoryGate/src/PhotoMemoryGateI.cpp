#include "PhotoMemoryGateI.h"

#include "PhotoMemoryAdapter.h"
#include "Notify/util/json_escape.h"
#include "Notify/util/tpl_modifiers.h"

using namespace std;

const char * kPmMainTplFile = "../etc/photo_memory/photo_memory_main.tpl";
const char * kPmListTplFile = "../etc/photo_memory/photo_memory_list.tpl";

namespace xce {
namespace feed {
class PMRequestFactory : public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      if (strcmp(path, "/photom/get") == 0 || strcmp(path, "/ebpt/show") == 0) {
        return new PhotoMemoryGetRequest(r);
      }
      if (strcmp(path, "/photom/read") == 0) {
        return new PhotoMemoryReadRequest(r);
      }
    }
    return NULL;
  }
};

}
}

void MyUtil::initialize() {
  static xce::notify::MaxLengthModifier mod_maxlen;
  ctemplate::AddModifier("x-maxlen=", &mod_maxlen);

  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&xce::feed::PhotoMemoryGateI::instance(), service.createIdentity("M", ""));

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service." + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_INFO("Fcgi listens " << fcgi_socket);

  xce::notify::FcgiServer * fcgi_server = new xce::notify::FcgiServer(fcgi_socket, 128, 512 * 1024);
  fcgi_server->RegisterRequestFactory(new xce::feed::PMRequestFactory());
  fcgi_server->Start();
}

namespace xce {
namespace feed {

PhotoMemoryGateI::PhotoMemoryGateI() {
}

bool PhotoMemoryGateI::ReloadTemplate(const Ice::Current&) {
  ctemplate::Template* tpl = ctemplate::Template::GetTemplate(kPmMainTplFile, ctemplate::STRIP_BLANK_LINES);
  if (tpl->ReloadIfChanged()) {
    MCE_INFO("tpl " << kPmMainTplFile << " changed and reloaded.");
  }

  tpl = ctemplate::Template::GetTemplate(kPmListTplFile, ctemplate::STRIP_BLANK_LINES);
  if (tpl->ReloadIfChanged()) {
    MCE_INFO("tpl " << kPmListTplFile << " changed and reloaded.");
  }
  return true;
}

string PhotoMemoryGateI::GetPhotos(Ice::Int uid, Ice::Int begin, Ice::Int /* limit */, const Ice::Current& current) {
  MCE_INFO("PhotoMemoryGateI::GetPhotos --> uid:" << uid << " bgein:" << begin << " con:" << current.con->toString());
  ctemplate::TemplateDictionary dict("");
  TemplateDictionary* list_dict = dict.AddIncludeDictionary("INC_PHOTO_LIST");
  list_dict->SetFilename(kPmListTplFile);
  int fetched_count = BuildListDict(uid, begin, 1, list_dict);
  if (fetched_count <= 0) {
    static string empty;
    return empty;
  }
  dict.SetValue("VIEW_PREV_CLASS", begin > 0 ? "pager-back" : "pager-back pager-back-disable");
  dict.SetValue("VIEW_NEXT_CLASS", fetched_count > 1 ? "pager-forward" : "pager-forward pager-forward-disable");

  string html;
  ctemplate::Template* tpl = ctemplate::Template::GetTemplate(kPmMainTplFile, ctemplate::STRIP_BLANK_LINES);
  if (tpl) {
    tpl->Expand(&html, &dict);
  } else {
    MCE_WARN("load template file error : " << kPmMainTplFile);
  }
  return html;
}

int PhotoMemoryGateI::BuildListDict(Ice::Int uid, Ice::Int begin, Ice::Int limit, ctemplate::TemplateDictionary * dict) {
  xce::feed::PhotoContentSeq photos;
  try {
    photos = xce::feed::PhotoMemoryAdapter::instance().GetPhotoContentSeq(uid, begin, limit + 1);
  } catch(Ice::Exception& e) {
    MCE_WARN("PhotoMemoryAdapter GetPhotoContentSeq() err : " << e << " uid=" << uid);
  } catch(std::exception& e) {
    MCE_WARN("PhotoMemoryAdapter GetPhotoContentSeq() err : " << e.what() << " uid=" << uid);
  }

  map<Ice::Int, UserCachePtr> user_info;
  GetUserCache(uid, photos, &user_info);
  MCE_INFO("uid " << uid << " photos count " << photos.size() << "/" << limit + 1);
  // 填充辞典
  size_t photo_count = photos.size();
  if (limit < photos.size()) {
    photo_count = limit;
  }

  for (size_t i = 0; i < photo_count; ++i) {
    ctemplate::TemplateDictionary * photo_dict = dict->AddSectionDictionary("SEC_PHOTO_ITEM");
    const PhotoContentPtr & photo = photos[i];

    MCE_DEBUG("photoid:" << photo->photoid << " owner:" << photo->ownerid 
        << " url:" << photo->url << " title:" << photo->title
        << " album:" << photo->albumname);

    photo_dict->SetIntValue("PHOTO_ID", photo->photoid);
    photo_dict->SetIntValue("OWNER_ID", photo->ownerid);
    photo_dict->SetValue("PHOTO_SRC_URL", photo->url);
    photo_dict->SetValue("PHOTO_DESC", photo->title);
    photo_dict->SetValue("ALBUM_NAME", photo->albumname);
    photo_dict->SetValue("CSS_DISPLAY", i == 0 ? "block" : "none");

    if (photo->title.empty()) {
      photo_dict->ShowSection("PHOTO_DESC_EMPTY");
    } else {
      photo_dict->ShowSection("NOT_PHOTO_DESC_EMPTY");
    }

    map<Ice::Int, UserCachePtr>::const_iterator user_info_it = user_info.find(photo->ownerid);
    UserCachePtr f;
    if (user_info_it != user_info.end()) {
      f = user_info_it->second;
    }
    if (f) {
      photo_dict->SetValue("OWNER_NAME", f->name());
    } else {
      photo_dict->SetIntValue("OWNER_NAME", photo->ownerid);
    }
    if (photo->tagusers.empty()) {
      continue;
    }

    ctemplate::TemplateDictionary * tag_user_dict = photo_dict->AddSectionDictionary("SEC_PHOTO_ITEM");
    for(size_t j = 0; j < photo->tagusers.size(); ++j) {
      tag_user_dict->AddSectionDictionary("SEC_TAG_USER");
      if (j > 0) {
        tag_user_dict->SetValue("TAG_USER_SEP", ";");
      }
      tag_user_dict->SetIntValue("TAG_USER_ID", photo->tagusers[j]);
      UserCachePtr u;
      user_info_it = user_info.find(photo->tagusers[j]);
      if (user_info_it != user_info.end()) {
        u = user_info_it->second;
      }
      if (u) {
        tag_user_dict->SetValue("OWNER_NAME", u->name());
      } else {
        tag_user_dict->SetIntValue("OWNER_NAME", photo->tagusers[j]);
      }
    }
  }

  return photos.size();
}

bool PhotoMemoryGateI::GetUserCache(Ice::Int uid, const PhotoContentSeq & photos, map<Ice::Int, UserCachePtr> * user_caches) {
  set<Ice::Int> uid_set;
  uid_set.insert(uid);
  for(size_t i = 0; i < photos.size(); ++i) {
    // std::copy(photos[i]->tagusers.begin(), photos[i]->tagusers.end(), uid_set.begin());
    uid_set.insert(photos[i]->ownerid);
    for(size_t j = 0; j < photos[i]->tagusers.size(); ++j) {
      uid_set.insert(photos[i]->tagusers[j]);
    }
  }

  IntSeq uids;
  for(set<Ice::Int>::const_iterator it = uid_set.begin(); it != uid_set.end(); ++it) {
    uids.push_back(*it);
  }
  try {
    *user_caches = UserCacheAdapter::instance().GetUserCache(uids);
  } catch (Ice::Exception& e) {
    MCE_WARN("UserCacheAdapter::GetUserCache() error: " << e);
    return false;
  } catch (std::exception& e) {
    MCE_WARN("UserCacheAdapter::GetUserCache() error: " << e.what());
    return false;
  }
  MCE_DEBUG("uid " << uid << " GetUserCached return size=" << user_caches->size() << "/" << uids.size());

  return true;
}


string PhotoMemoryGateI::GetFriendPhotos(Ice::Int userid,Ice::Int friendid, Ice::Int begin, Ice::Int limit, const Ice::Current&) {
  return "";
}

bool PhotoMemoryGetRequest::response() {
  MCE_INFO("PhotoMemoryGetRequest::response");
  //
  // http://notify.renren.com/photom/get?start=0&limit=3
  //
  if (!_fcgi_out) {
    MCE_WARN("_fcgi_out null");
    return false;
  }

  if(!Request::response()) {
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\nverify failure");
    return false;
  }
  ctemplate::TemplateDictionary dict("");
  PhotoMemoryGateI::instance().BuildListDict(getPropertyInt("uid", _login_uid), getPropertyInt("start", 0), getPropertyInt("limit", 3), &dict);

  string rsp_body;
  ctemplate::Template* tpl = ctemplate::Template::GetTemplate(kPmListTplFile, ctemplate::STRIP_BLANK_LINES);
  if (tpl) {
    tpl->Expand(&rsp_body, &dict);
  } else {
    MCE_WARN("load template file error : " << kPmListTplFile);
  }
  FCGX_PutS("Content-type: text/html; charset=utf-8\r\n\r\n", _fcgi_out);
  FCGX_PutS(rsp_body.c_str(), _fcgi_out);
  return true;
}

bool PhotoMemoryReadRequest::response() {
  MCE_INFO("PhotoMemoryReadRequest::response");
  //
  // http://notify.renren.com/photom/read?photo=79033
  //
  if (!_fcgi_out) {
    MCE_WARN("_fcgi_out null");
    return false;
  }
  if(!Request::response()) {
    FCGX_PutS("Content-type: text/html\r\n\r\nverify failure", _fcgi_out);
    return false;
  }
  
  try {
    xce::feed::PhotoMemoryAdapter::instance().SetRead(_login_uid, getPropertyInt("photo", 0));
  } catch(Ice::Exception& e) {
    MCE_WARN("PhotoMemoryAdapter SetRead() err : " << e << " uid=" << _login_uid);
  } catch(std::exception& e) {
    MCE_WARN("PhotoMemoryAdapter SetRead() err : " << e.what() << " uid=" << _login_uid);
  }
  FCGX_PutS("Content-type: text/html\r\n\r\n0", _fcgi_out);
  return false;
}

}
}
