#ifndef _XCE_NOTIFY_HEADURL_H_
#define _XCE_NOTIFY_HEADURL_H_

#include <string>

#include <vector>
#include <map>
#include "QueryRunner.h"

using namespace MyUtil;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (static_cast<int>((sizeof(x)/sizeof(x[0]))))
#endif // ARRAY_SIZE

namespace xce {
namespace notify {

// db: upload
// select group_name,domain,space_used,cpu_load,status from storage_node where status>0;

// SQL
// mysql -hphoto32.db -uxndev  -prebornlOM --default-character-set=utf8 fb
struct HeadStorage { // : public MyUtil::Singleton<HeadStorage> {
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
  typedef std::map<std::string, NodeListType> MapType;

  bool GroupExist(const std::string & group) const {
    IceUtil::Mutex::Lock lock(_mutex);
    return map_.find(group) != map_.end();
  }

  bool empty() const {
    IceUtil::Mutex::Lock lock(_mutex);
    return map_.empty();
  }

  bool GetReadableNode(const std::string & group, Node * node) const {
    IceUtil::Mutex::Lock lock(_mutex);

    MapType::const_iterator i = map_.find(group);
    //ASSERT(map_.end() != i);
    if (map_.end() == i) {
      return false;
    }

    const NodeListType & vc = i->second;
    // ASSERT(!vc.empty());
    if (vc.empty()) {
      return false;
    }

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

    MCE_DEBUG("group not exist " << pattern);

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
    bool b = Extract(pattern, group, filename);
    MCE_DEBUG("Extract res ---------- b : " << b);

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

  static HeadStorage & instance() {
    static HeadStorage x_;
    return x_;
  }

  static void Load() {
    HeadStorage & strg = HeadStorage::instance();
    strg.InitOnce();
  }

  void Tick() {
    // 从数据库加载
    MapType map;
    if (LoadFromDB(&map)) {
      IceUtil::Mutex::Lock lock(_mutex);
      map_.swap(map);
    }
  }

  static bool LoadFromDB(MapType* map) {
    com::xiaonei::xce::Statement sql;
    sql << "select group_name,domain,space_used,cpu_load,status from storage_node where status>0;";

    mysqlpp::StoreQueryResult result;
    try {
      result = com::xiaonei::xce::QueryRunner("upload", com::xiaonei::xce::CDbRServer).store(sql);
    } catch(std::exception & e) {
      MCE_WARN("query db error : " << e.what());
      return false;
    }

    if (!result)
      return false;

    for (size_t i = 0; i<result.num_rows(); ++i) {
      const mysqlpp::Row & row = result[i];
      MCE_DEBUG(row["group_name"] << " " << row["domain"]);
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

    //ASSERT(map->size() > 100);
    //ASSERT(result.num_rows());

    for (MapType::const_iterator i=map->begin(); i!=map->end(); ++i) {
      MCE_DEBUG(" " << i->first << " " << i->second.size());
    }

    return map->size();
  }

  static bool LoadFromCode(MapType * map) {
    Node arr[] = {
//      {"", "", 0, 0},
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
      IceUtil::Mutex::Lock lock(_mutex);
      map_.swap(map);

      // TODO: 定时调用 Tick
    }
    else 
#endif
    if (LoadFromCode(&map)) {
      MCE_INFO("HeadStorage LoadFromDB failed. LoadFromCode succeeded.");
      IceUtil::Mutex::Lock lock(_mutex);
      map_.swap(map);
    }
  }

  HeadStorage(){}
  HeadStorage(const HeadStorage&);
  void operator=(const HeadStorage&);

  MapType map_;
	mutable IceUtil::Mutex _mutex;
};

std::string GetHeadUrl(const std::string & pattern);

}
}

#endif // _XCE_NOTIFY_HEADURL_H_

