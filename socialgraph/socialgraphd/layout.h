#ifndef XCE_SOCIALGRAPH_LAYOUT_H__
#define XCE_SOCIALGRAPH_LAYOUT_H__

#include <vector>
#include <string>
#include <map>

#include "socialgraph/socialgraphd/base_action.h"
#include "OceCxxAdapter/src/UserCacheAdapter.h"
#include "OceCxxAdapter/src/XceLoggerAdapter.h"
#include "socialgraph/socialgraphproto/item.pb.h"

namespace xce {
namespace socialgraph {

class Layout;

typedef std::map<std::string, std::vector<unsigned char> > RawDataMap;
typedef std::map<int, xce::usercache::UserCachePtr> UserCacheMap;
typedef std::vector<unsigned char> RawData;
typedef std::map<std::string, RawData> RawDataMap;
typedef boost::shared_ptr<Layout> LayoutPtr;

static const std::string FREINDS_CLUSTER = "FFUCR";		
static const std::string COMMON_FRIENDS = "FFW";
static const std::string NEW_COMMER = "FFNCW";
static const std::string RECONNECT = "FFWR";
static const std::string PAGE = "PAGE";
static const std::string RBLOG = "RCD_BLOG";
static const std::string RVIDEO = "RCD_VIDEO";
static const std::string RSITE = "RCD_SITE";

//-----------------------------------------------------------------------------------------------------------

class Layout {
  public:
    virtual std::string DoLayout(UserCacheMap*) = 0;
    virtual std::set<int> UserIdSet() {
      return std::set<int>();
    }
    virtual ~Layout() {}
};

//-----------------------------------------------------------------------------------------------------------

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

class PageLayout : public Layout {
 public:
  PageLayout(int uid, const Item& item_data):host_(uid), item_data_(item_data) {
  }
  virtual ~PageLayout() {}

  std::set<int> UserIdSet() {
    std::set<int> id_set;
    id_set.insert(item_data_.id());
		int size = item_data_.fields_size();
		for (int i = 1; i < size; ++i) {				//as to real size of common friends stored in the 0 position of fields, 
			id_set.insert(item_data_.fields(i)); 	//start with position 1
		}
    return id_set;
  }
  virtual std::string DoLayout(UserCacheMap*);
 private:
  int host_;
  Item item_data_;
};

//-----------------------------------------------------------------------------------------------------------

/* @brief 为video首页推荐提供展示 */
class RcdVideoLayout : public Layout {
public:
  RcdVideoLayout(int uid, const Item& item_data):host_(uid), item_data_(item_data) {
  }
  virtual ~RcdVideoLayout() {}

  virtual std::string DoLayout(UserCacheMap*);

private:
	/* @brief 获取video url
   * @param hostid 哪个用户分享的
   * @param shareid 视频id
   */
  std::string GetShareUrl(int hostid, long shareid) const {
    std::ostringstream oss;
    oss << "http://share.renren.com/share/" << hostid << "/" << shareid;
    return oss.str();
  }

  int host_;
  Item item_data_;
};

//-----------------------------------------------------------------------------------------------------------

/* @brief 为blog首页推荐提供展示 */
class RcdBlogLayout : public Layout {
public:
  RcdBlogLayout(int uid, const Item& item_data):host_(uid), item_data_(item_data) {
  }
  virtual ~RcdBlogLayout() {}

  virtual std::string DoLayout(UserCacheMap*);

private:
	/* @brief 获取blog url
   * @param hostid 哪个用户分享的
   * @param blogid 日志id
   */
	std::string GetBlogUrl(int hostid, long blogid) const {
		std::ostringstream oss;
		oss << "http://blog.renren.com/share/" 
			<< hostid << "/" << blogid << "/";
		return oss.str();
	}

  int host_;
  Item item_data_;
};

//-----------------------------------------------------------------------------------------------------------

/* @brief 为site首页推荐提供展示 */
class RcdSiteLayout: public Layout {
public:
  RcdSiteLayout(int uid, const Item& item_data):host_(uid), item_data_(item_data) {
  }
  virtual ~RcdSiteLayout() {}

  std::set<int> UserIdSet() {		//return id of minisite
    std::set<int> id_set;
    id_set.insert(item_data_.id());
    return id_set;
  }

  virtual std::string DoLayout(UserCacheMap*);

private:
  int host_;
  Item item_data_;
};

//-----------------------------------------------------------------------------------------------------------

/* @brief 依据不同的数据源创建不同的Layout逻辑 */
class LayoutFactory {
public:
	static LayoutPtr Create(int uid, const Item& item_data);
};

}}
#endif

