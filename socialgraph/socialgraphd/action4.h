#ifndef XCE_SOCIALGRAPH_ACTION4_H__
#define XCE_SOCIALGRAPH_ACTION4_H__

#include <vector>
#include <string>
#include <map>

#include "socialgraph/socialgraphd/base_action.h"
#include "OceCxxAdapter/src/UserCacheAdapter.h"
#include "OceCxxAdapter/src/XceLoggerAdapter.h"
#include "socialgraph/socialgraphproto/pukfriendrecommend.pb.h"
#include "socialgraph/socialgraphproto/item.pb.h"

namespace xce {
namespace socialgraph {

class Layout;

typedef std::map<std::string, std::vector<unsigned char> > RawDataMap;
typedef std::map<int, xce::usercache::UserCachePtr> UserCacheMap;
typedef std::vector<unsigned char> RawData;
typedef std::map<std::string, RawData> RawDataMap;
typedef boost::shared_ptr<Layout> LayoutPtr;

bool ConvertPukToItems(RawData*, Items*);


class Layout {
  public:
    virtual std::string DoLayout(UserCacheMap*) = 0;
    virtual std::set<int> UserIdSet() {
      return std::set<int>();
    }
    virtual ~Layout() {}
};


class ItemLayout : public Layout {
  public:
    ItemLayout(int uid, Reconnect& rt, 
               const Item& item_data) : host_(uid), rt_(rt), item_data_(item_data) {
    }

    std::set<int> UserIdSet() {
      std::set<int> id_set;
      id_set.insert(item_data_.id());
      BOOST_FOREACH(int i, item_data_.fields()) {
        id_set.insert(i);
      }
      return id_set;
    }

    virtual std::string DoLayout(UserCacheMap*);
  private:
    int host_;
    Reconnect rt_;
    xce::socialgraph::Item item_data_;
};

class RcdLayout : public Layout {
 public:
  RcdLayout(int uid, const Item& item_data):host_(uid), item_data_(item_data) {

  }
  virtual ~RcdLayout() {}

  std::set<int> UserIdSet() {
    std::set<int> id_set;
    id_set.insert(item_data_.id());
    BOOST_FOREACH(int i, item_data_.fields()) {
      id_set.insert(i);
    }
    return id_set;
  }
  virtual std::string DoLayout(UserCacheMap*);
 private:
  int host_;
  Item item_data_;
};

//-----------------------------------------------------------------------------------------------------------
/* @brief 为blog连续阅读提供展示 */
class BlogLayout : public Layout {
public:
	BlogLayout(const Item& blog_recommend, const std::string& type):
		blog_recommend_(blog_recommend), type_(type) {
	}
	
	virtual std::string DoLayout(UserCacheMap*);
	
private:
	/* @brief 获取blog url
   * @param hostid 哪个用户分享的
   * @param blogid 日志id
   */
	std::string GetBlogUrl(int hostid, long blogid) {
		std::ostringstream oss;
		oss << "http://blog.renren.com/share/" 
			<< hostid << "/" << blogid << "/";
		return oss.str();
	}

	Item blog_recommend_;
	std::string type_;
};

//-----------------------------------------------------------------------------------------------------------

class VideoLayout : public Layout {
 public:
  VideoLayout(const std::string& t_url, 
              const std::string& title,
              long sid, 
              int uid):thrum_url_(t_url), title_(title), share_id_(sid), user_id_(uid) { 

  }
  std::set<int> UserIdSet() {
    return std::set<int>();
  }

  virtual std::string DoLayout(UserCacheMap*);

  std::string GetShareUrl() const {
    std::ostringstream oss;
    oss << "http://share.renren.com/share/" << user_id_ << "/" << share_id_;
    return oss.str();
  }

 private:
  std::string thrum_url_;
  std::string title_;
  long share_id_;
  int user_id_;
};

//-----------------------------------------------------------------------------------------------------------

class GetAction4 : public BaseAction {
  public:
    bool Match(const std::string& url) const {
      return boost::starts_with(url, "/cwf_nget") || boost::starts_with(url, "/cwf_get") || boost::starts_with(url, "/cwf_performance");
    }

    virtual cwf::HttpStatusCode DoGet(cwf::Request* req, cwf::Response* res, cwf::User* u) {
      return DoPost(req, res, u);
    }

    virtual cwf::HttpStatusCode DoPost(cwf::Request*, cwf::Response*, cwf::User*);

  private:
    void FilterUserCache(const UserCacheMap& map1, UserCacheMap& map2);
    std::vector<LayoutPtr> PrepareLayout(int uid, ReconnectList*, RawDataMap*);
    void GetData(int uid, ReconnectList*, RawDataMap*);
    std::vector<int> GetUserIdSeq(std::vector<LayoutPtr>*);
    void SendItemMessage(int host, const std::string& mckey, const Reconnect& rt, const RawData& data);

};

class RemoveAction4 : public BaseAction {
  public:
    bool Match(const std::string& url) const {
      return boost::starts_with(url, "/cwf_nremove") || boost::starts_with(url, "/cwf_remove");
    }

    virtual cwf::HttpStatusCode DoGet(cwf::Request* req, cwf::Response* res, cwf::User* u) {
      return DoPost(req, res, u);
    }

    virtual cwf::HttpStatusCode DoPost(cwf::Request*, cwf::Response*, cwf::User*);

  private:
    void SendMessage(int host, int guest, const std::string& biz);
};

//-----------------------------------------------------------------------------------------------------------

/* @brief 为blog连续阅读提供服务 */
class BlogAction : public BaseAction {
public: 
  bool Match(const std::string& url) const {
    return boost::starts_with(url, "/cwf_blog");
  }

  virtual cwf::HttpStatusCode DoGet(cwf::Request* req, cwf::Response* res, cwf::User* u) {
    return DoPost(req, res, u);  
  }

  virtual cwf::HttpStatusCode DoPost(cwf::Request*, cwf::Response*, cwf::User*);
  
private:
  /* @brief 根据blogid获取日志的相关度推荐
   * @param blogid 日志id
   */
  xce::socialgraph::Items GetBlogDataByBlogTag(long blogid);

  /* @brief 根据userid的用户行为推荐日志
   * @param userid 用户id
   */
  xce::socialgraph::Items GetBlogDataByUseraction(int userid);
};

//-----------------------------------------------------------------------------------------------------------

class TestAction : public BaseAction {
  public:
    bool Match(const std::string& url) const {
      return boost::starts_with(url, "/cwf_test");
    }
 
    virtual cwf::HttpStatusCode DoGet(cwf::Request* req, cwf::Response* res, cwf::User* u) {
      return DoPost(req, res, u);
    }

    virtual cwf::HttpStatusCode DoPost(cwf::Request*, cwf::Response*, cwf::User*);

  private:
    void SendMessage(int host, int guest, const std::string& biz);
};

class VideoAction : public BaseAction {
 public:
  bool Match(const std::string& url) const {
    return boost::starts_with(url, "/cwf_video");
  }
  virtual cwf::HttpStatusCode DoGet(cwf::Request* req, cwf::Response* res, cwf::User* u) {
    return DoPost(req, res, u);
  }
  virtual cwf::HttpStatusCode DoPost(cwf::Request*, cwf::Response*, cwf::User*);
};

//--------------------------
class GetAction5 : public Base2Action {
 public:
  bool Match(const std::string& url) const {
    return boost::starts_with(url, "/cwf_nget") || boost::starts_with(url, "/cwf_get") || boost::starts_with(url, "/cwf_performance");
  }
  virtual cwf::HttpStatusCode DoGet(cwf::Request* req, cwf::Response* res, cwf::User* user) {
    return DoPost(req, res, user);
  }
  virtual cwf::HttpStatusCode DoPost(cwf::Request*, cwf::Response*, cwf::User*);
 private:
  std::vector<LayoutPtr> PrepareLayout(int uid);
  std::vector<int> GetUserIdSeq(std::vector<LayoutPtr>*);
  void FilterUserCache(const UserCacheMap& map1, UserCacheMap& map2);

}; 

class RemoveAction5 : public BaseAction {
 public:
  bool Match(const std::string& url) const {
    return boost::starts_with(url, "/cwf_nremove") || boost::starts_with(url, "/cwf_remove");
  }

  virtual cwf::HttpStatusCode DoGet(cwf::Request* req, cwf::Response* res, cwf::User* u) {
    return DoPost(req, res, u);
  }

  virtual cwf::HttpStatusCode DoPost(cwf::Request*, cwf::Response*, cwf::User*);
};

class StatAction : public Base2Action {
 public:
  StatAction();
  bool Match(const std::string& url) const {
    return boost::starts_with(url, "/ajaxHomeReconnectStat");
  }
  virtual cwf::HttpStatusCode DoGet(cwf::Request* req, cwf::Response* res, cwf::User* user) {
    return DoPost(req, res, user);
  }
  virtual cwf::HttpStatusCode DoPost(cwf::Request*, cwf::Response*, cwf::User*);

 private:
  com::xiaonei::service::XceLoggerPtr remote_logger_; 
  std::vector<std::string> log_paths_;

}; 

}}
#endif

