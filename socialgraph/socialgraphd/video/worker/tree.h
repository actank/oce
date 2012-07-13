#ifndef _XCE_TREE_H__
#define _XCE_TREE_H__
#include <string>
#include <sstream>
#include <list>

#include <boost/foreach.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

#include "base/hashmap.h"
#include "base/stringdigest.h"
#include "base/ptime.h"

namespace xce {
namespace socialgraph {

typedef std::hash_map<int, std::string> IndexStrMap;
typedef std::hash_map<std::string, int> StrIndexMap;

class Node;

class VideoShare {
 private:
  std::string md5_;
  long share_id_;
  int user_id_;
  int score_;
  std::string thumb_url_;
  std::string title_;
  Node* node_;
 public:
  VideoShare() : md5_(""), share_id_(0), user_id_(0), score_(0), thumb_url_(""), title_(""),node_(0) { }
  virtual ~VideoShare() { }
  std::string md5() const {
    return md5_;
  }
  void set_md5(const std::string& md5) {
    md5_ = md5;
  }
  long share_id() const {
    return share_id_;
  }
  void set_share_id(long sid) {
    share_id_ = sid;
  }
  int user_id() const {
    return user_id_;
  }
  void set_user_id(int uid) {
    user_id_ = uid;
  }
  int score() const {
   return score_;
  }
  void set_score(int s) {
   score_ = s;
  }
  std::string thumb_url() const {
    return thumb_url_;
  }
  void set_thumb_url(const std::string& t_url) {
    thumb_url_ = t_url;
  }
  std::string title() const {
    return title_;
  }
  void set_title(const std::string& t) {
    title_ = t;
  }
  Node* node() {
    return node_;
  }
  void set_node(Node* node) {
    node_ = node;
  }
  std::string ShareUrl() {
    std::ostringstream oss;
    oss << "http://share.renren.com/share/"
        << user_id_ << "/"
        << share_id_;
   return oss.str();
  }
  std::string VideoShareInfo() {
    std::ostringstream oss;
    oss << "md5():" << md5() << " share_id():" << share_id()
        << " user_id():" << user_id() << " score():" << score()
        << " ShareUrl():" << ShareUrl();
    return oss.str();
  }
};


class Node {
 private:
  int index_; 
  bool leaf_;
  Node* parent_;
  std::list<Node*>* children_;
  std::list<VideoShare*>* video_share_list_;
 public:
  Node() : index_(-1), leaf_(false), parent_(0), video_share_list_(0) {
    children_ = new std::list<Node*>();
    video_share_list_ = new std::list<VideoShare*>();
  }
  virtual ~Node() {
    BOOST_FOREACH(VideoShare* vs, *video_share_list_) {
      if (vs) {
        delete vs;
      }
    }
    delete video_share_list_;
    BOOST_FOREACH(Node* n, *children_) {
      if (n) {
        delete n;
      }
    } 
    delete children_;
  }
  int index() const {
    return index_;
  }
  void set_index(int i) {
    index_ = i;
  }
  bool leaf() const {
    return leaf_;
  }
  void set_leaf(bool l) {
    leaf_ = l;
  }
  Node* parent() const {
    return parent_;
  }
  void set_parent(Node* p) {
    parent_ = p;
  }
  std::list<Node*>* children() const {
    return children_;
  }
  std::list<VideoShare*>* video_share_list() const {
    return video_share_list_;
  }
  bool AddChild(Node* child) {
    if (!child) {
      return false;
    }
    children_->push_back(child);
    return true;
  }
  bool AddVideoShare(VideoShare* vs) {
    if (!vs) {
      return false;
    }
    bool flag = false;
    BOOST_FOREACH(VideoShare* v, *video_share_list_) {
      if (vs->md5() == v->md5()) {
        flag = true;
      }
    }
    if (!flag) {
      video_share_list_->push_back(vs);
      return true;
    }
    return false;
  }
  std::string NodeInfo() const {
    std::ostringstream oss;
    oss << "node->index():" << index();
    oss << "\n\tchildren index_seq:";
    BOOST_FOREACH(Node* child, *children_) {
      oss << child->index() << " ";
    }
    oss << "\n\tVideoShares:";
    BOOST_FOREACH(VideoShare* vs, *video_share_list_) {
      oss << "\n\t\t" << vs->VideoShareInfo();
    }
    return oss.str();
  }
};


// class Tree must be designed as a thread safe class!
class Tree {
 public:
  Tree(const std::string& video_file_path, const std::string& video_tag_file_path) : video_file_path_(video_file_path), video_tag_file_path_(video_tag_file_path) {
    PTIME(pt, "Tree Constructor", true, 0);
    root_ = new Node();
    root_->set_index(-2);
  }
  virtual ~Tree() {
    FreeTree(root_);
  }

  void Init();

  VideoShare* SearchVideoShare(const std::string& md5) {
    boost::shared_lock<boost::shared_mutex> r_lock(rw_mutex_);
    std::hash_map<std::string, VideoShare*>::const_iterator it = video_map_.find(md5);
    if (it != video_map_.end()) {
      return it->second;
    }
    return 0;
  }

  Node* SearchNode(int cate_index) {
    return Search(root_, cate_index);
  }
  
  void Display();
 private:
  Node* Create(const std::vector<int>& path);
  Node* Create(Node* parent, int cate_index);
  Node* SearchChild(Node* parent, int cate_index);
  Node* Search(Node*, int);
  void FreeTree(Node* node) {
    if (node) {
      delete node;
    }
  }
 private:
  std::string video_file_path_;
  std::string video_tag_file_path_;
  IndexStrMap index_str_map_;
  StrIndexMap str_index_map_; 
  Node* root_;
  boost::shared_mutex rw_mutex_;
 private:
  std::hash_map<std::string, VideoShare*> video_map_;
};

}} //end xce::socialgraph


#endif

