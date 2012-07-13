#ifndef XCE_SOCIALGRAPH_ACTION_H__
#define XCE_SOCIALGRAPH_ACTION_H__

#include <vector>
#include <string>
#include <map>

#include "socialgraph/socialgraphd/base_action.h"
#include "OceCxxAdapter/src/UserCacheAdapter.h"
#include "OceCxxAdapter/src/XceLoggerAdapter.h"
#include "socialgraph/socialgraphproto/item.pb.h"
#include "socialgraph/socialgraphd/layout.h"

namespace xce {
namespace socialgraph {

/* @brief 首页推荐位获取操作 */
class GetAction5 : public Base2Action {
 public:
  bool Match(const std::string& url) const {
    return boost::starts_with(url, "/cwf_nget_home");
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

//-----------------------------------------------------------------------------------------------------------

/* @brief 首页推荐位删除操作 */
class RemoveAction5 : public Base2Action {
 public:
  bool Match(const std::string& url) const {
    return boost::starts_with(url, "/cwf_nremove_home");
  }

  virtual cwf::HttpStatusCode DoGet(cwf::Request* req, cwf::Response* res, cwf::User* u) {
    return DoPost(req, res, u);
  }

  virtual cwf::HttpStatusCode DoPost(cwf::Request*, cwf::Response*, cwf::User*);
};

//-----------------------------------------------------------------------------------------------------------

/* @brief 首页推荐位的数据监控 */
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

/* @brief 为video连续观看提供服务 */
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

//-----------------------------------------------------------------------------------------------------------

/* @brief 其他页面推荐位获取操作 */
class GetOtherAction5: public Base2Action {
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

//-----------------------------------------------------------------------------------------------------------

/* @brief 其他页面推荐位删除操作 */
class RemoveOtherAction5 : public Base2Action {
 public:
  bool Match(const std::string& url) const {
    return boost::starts_with(url, "/cwf_nremove") || boost::starts_with(url, "/cwf_remove");
  }

  virtual cwf::HttpStatusCode DoGet(cwf::Request* req, cwf::Response* res, cwf::User* u) {
    return DoPost(req, res, u);
  }

  virtual cwf::HttpStatusCode DoPost(cwf::Request*, cwf::Response*, cwf::User*);
};
}}
#endif

