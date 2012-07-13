#include "socialgraph/socialgraphd/headurl.h"

#include <vector>
#include "base/hashmap.h"

#include <boost/thread/mutex.hpp>

#include "base/common.h"
#include "base/logging.h"
#include "base/once.h"

#include "dbpool/queryrunner.h"

// SQL
// mysql -hphoto32.db -uxndev  -prebornlOM --default-character-set=utf8 fb

// db: upload
// select group_name,domain,space_used,cpu_load,status from storage_node where status>0;

namespace xce {

struct Storage {
  enum Type {
    NODE_TYPE_NORMAL = 1,
    NODE_TYPE_HEAD = 2,
    NODE_TYPE_CLASS = 3
  };

  struct Node {
    // int id;
    // std::string name;
    // std::string path;
    // time_t time;
    // int type;

    std::string group;
    std::string domain;
    double spaceUsed;
    double cpuLoad;
    int status;

    enum Status {
      STATUS_DISABLE = 0,   // node 状态无效
      STATUS_READABLE = 1,  //node 状态能读
      STATUS_WRITABLE = 2,  //node 状态能写
      STATUS_WRITABLE_READABLE = STATUS_READABLE | STATUS_WRITABLE
    };

    bool readable() const {
      return (status & STATUS_WRITABLE) // 直接抄过来的代码，貌似有点问题?
        && spaceUsed < 0.9;
    }
  };

  typedef std::vector<Node> NodeListType;
  typedef std::hash_map<std::string, NodeListType> MapType;

  bool GroupExist(const std::string & group) const {
    boost::mutex::scoped_lock guard(lock_);
    return map_.find(group) != map_.end();
  }

  bool empty() const {
    boost::mutex::scoped_lock guard(lock_);
    return map_.empty();
  }

  bool GetReadableNode(const std::string & group, Node * node) const {
    boost::mutex::scoped_lock guard(lock_);

    MapType::const_iterator i = map_.find(group);
    ASSERT(map_.end() != i);
    if (map_.end() == i)
      return false;

    const NodeListType & vc = i->second;
    ASSERT(!vc.empty());
    if (vc.empty())
      return false;

    size_t r = rand() % vc.size();
    for (size_t c = 0; c < vc.size(); ++c, ++r) {
      const Node & n = vc[r % vc.size()];
      if (n.readable()) {
        *node = n;
        return true;
      }
    }

    // 返回第一个
    *node = vc[r % vc.size()];
    return true;
  }

  static bool Extract(const std::string & pattern, std::string & group, std::string & file) {
    // find
    std::string::size_type pos = pattern.find_first_of('/');
    if (std::string::npos == pos) {
      group = pattern;
      return false;
    }

    group = pattern.substr(0, pos);
    file = pattern.substr(pos + 1);
    return true;
  }

  std::string Locate(const std::string & pattern, Type type) {
    if (NODE_TYPE_NORMAL == type)
      return LocateNormal(pattern);
    else if (NODE_TYPE_HEAD == type)
      return LocateHead(pattern);

    return std::string();
  }

  std::string LocateNormal(const std::string & pattern) {
    // if group exist
    // GetReadableNode
    // http:// domain / photos/ pattern
    std::string group, filename;
    Extract(pattern, group, filename);

    if (GroupExist(group)) {
      Node node;
      GetReadableNode(group, &node);

      if (group.find("hd") == 0) {
        // :(, we have to add to subdirectory to recorrect it
        return "http://" + node.domain + "/photos/" + group + "/" + filename;
      } else if (group.find("fmn") == 0) {
        return "http://" + node.domain + "/photos/" + pattern;
      } 
      else
        return "http://" + node.domain + "/" + filename;
    }

    LOG(VERBOSE) << "group not exist " << pattern;

    if ( group.find("hdn") == 0 ) {
      return "http://" + group + ".xnimg.cn" + "/photos/" + group + "/" + filename;
    } else if ( group.find("fmn") == 0 ) {
      return "http://" + group + ".xnimg.cn" + "/photos/" + group + "/" + filename;
    } else if ( group.find("hd") == 0 ) {
      return "http://" + group + ".xiaonei.com" + "/photos/" + group + "/" + filename;
    } else {
      // return URL_HEAD + "/photos/0/0/men_head.gif";
      // return "http://" + group + ".xiaonei.com" + "/" + fileName;
      return "";
    }	
  }

  std::string LocateHead(const std::string & pattern) {
    // if group exist
    // GetReadableNode
    // http:// domain / photos/ pattern
    std::string group, filename;
    Extract(pattern, group, filename);

    if (GroupExist(group)) {
      Node node;
      GetReadableNode(group, &node);
      return "http://" + node.domain + "/photos/" + pattern;
    }

    std::string url("http://head.xiaonei.com");

    if (std::string::npos != pattern.find("head.jpg"))
      url += "/photos/0/0/men_head.gif";
    else if (std::string::npos != pattern.find("main.jpg"))
      url += "/photos/0/0/men_main.gif";
    else if (std::string::npos != pattern.find("tiny.jpg"))
      url += "/photos/0/0/men_tiny.gif";
    else
      url += "/photos/" + pattern;	
    return url;		
  }

  static Storage & instance() {
    static Storage x_;
    return x_;
  }

  static void Load() {
    Storage & strg = Storage::instance();
    strg.InitOnce();
  }

  void Tick() {
    // 从数据库加载
    MapType map;
    if (LoadFromDB(&map)) {
      boost::mutex::scoped_lock guard(lock_);
      map_.swap(map);
    }
  }

  static bool LoadFromDB(MapType* map) {
    com::xiaonei::xce::Statement sql;
    sql << "select group_name,domain,space_used,cpu_load,status from storage_node where status>0;";

    mysqlpp::StoreQueryResult result;
    try {
      result = 
        com::xiaonei::xce::QueryRunner("upload", com::xiaonei::xce::CDbRServer)
        .store(sql);
    } catch(std::exception & e) {
      LOG(LS_ERROR) << e.what();
      return false;
    }

    if (!result)
      return false;

    for (size_t i = 0; i<result.num_rows(); ++i) {
      const mysqlpp::Row & row = result[i];
      LOG(VERBOSE) << row["group_name"] << " " << row["domain"];
      Node node;
      // node.id = row["id"];
      // node.name = (std::string)row["node_name"];
      // node.type = row["type"];
      // node.time = time(0);
      // node.path = (std::string)row["path"];

      node.domain = (std::string)row["domain"];
      node.group = (std::string)row["group_name"];
      node.spaceUsed = (double)row["space_used"];
      node.cpuLoad = (double)row["cpu_load"];
      node.status = row["status"];

      map->operator[](node.group).push_back(node);
    }

    ASSERT(map->size() > 100);
    ASSERT(result.num_rows());

    for (MapType::const_iterator i=map->begin(); i!=map->end(); ++i) {
      LOG(VERBOSE) << " " << i->first << " " << i->second.size();
    }

    return map->size();
  }

  static bool LoadFromCode(MapType * map) {
    Node arr[] = {
//      {"", "", 0, 0},
#include "socialgraph/socialgraphd/storage.inc"
    };

    for (size_t i = 0; i<ARRAY_SIZE(arr); ++i) {
      map->operator[](arr[i].group).push_back(arr[i]);
    }
    return map->size();
  }

private:
  void InitOnce() {
    MapType map;
    // 在内网实在无法访问这些数据库，为测试比较有效
    // 1 尽可能不访问db
    // 2 提供静态表的初始化
#ifdef POSIX
    if (LoadFromDB(&map)) {
      boost::mutex::scoped_lock guard(lock_);
      map_.swap(map);

      // TODO: 定时调用 Tick
    }
    else 
#endif
    if (LoadFromCode(&map)) {
      LOG(INFO) << "Storage LoadFromDB failed. LoadFromCode succeeded.";
      boost::mutex::scoped_lock guard(lock_);
      map_.swap(map);
    }
  }

  Storage() {}

  MapType map_;
  mutable boost::mutex lock_;

  DISALLOW_EVIL_CONSTRUCTORS(Storage);
};

// TODO: 逻辑还有些混乱
std::string HeadUrl::FromPart(const std::string & pattern) {
  static XCE_DECLARE_ONCE(once_init);
  XceOnceInit(&once_init, &Storage::Load);

  // 针对合并后的开心
  bool kaixin = false;
  std::string pattern1(pattern);
  if (0 == pattern1.find("kx/")) {
    pattern1 = pattern1.substr(3);
    kaixin = true;		
  }

  if (pattern1.empty())
    return std::string();

  std::string url = Storage::instance().Locate(pattern1, Storage::NODE_TYPE_HEAD);

  if (url.empty())	{
    static const char * URL_HEAD = "http://head.xiaonei.com";
    url  = URL_HEAD;

    if (pattern1.find("head.jpg") != std::string::npos)
      url += "/photos/0/0/men_head.gif";
    else if (pattern1.find("main.jpg") != std::string::npos)
      url += "/photos/0/0/men_main.gif";
    else if (pattern1.find("tiny.jpg") != std::string::npos)
      url += "/photos/0/0/men_tiny.gif";
    else
      url += "/photos/" + pattern1;
    // url = URL_HEAD + "/photos/0/0/men_tiny.gif";
  }

  // kaixin
  if (kaixin) {
    // 针对kaixin的，需要把域名改为 rrimg.com
    size_t pos = url.find("xnimg.cn");
    if (pos != std::string::npos) {
      std::string tmp = url.substr(0, pos);
      tmp += "rrimg.com";
      tmp += url.substr(pos + 8);
      url = tmp;
      // url.insert(pos, "rrimg.com");
    }
  }
  return url;
}

}
