#include "socialgraph/socialgraphd/video/worker/tree.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "base/logging.h"
#include "socialgraph/socialgraphutil/algostuff.h"

namespace xce {
namespace socialgraph {

void Tree::Init() {
  PTIME(pt, "Tree Init", true, 0);
  boost::unique_lock<boost::shared_mutex> w_lock(rw_mutex_);
  std::vector<std::string> vs_lines;
  xce::GetTextFileLines(video_file_path_, vs_lines);
  LOG(INFO) << "vs_lines.size():" << vs_lines.size();
  BOOST_FOREACH(const std::string& line, vs_lines) {
    std::vector<std::string> units;
    boost::split(units, line, boost::is_any_of("\t"));
    if (units.size() == 7 && units[4].size() > 0) {
      // 5c663dfaaece1a49c3f3e27d72c609c9	242717095	5532824806	186	0-2-4 thumb_url title
      std::string md5 = units[0];
      int uid;
      long sid;
      int score;
      std::vector<int> cate_seq;
      std::string thumb_url = units[5];
      std::string title = units[6];
      try {
        uid = boost::lexical_cast<int>(units[1]);
        sid = boost::lexical_cast<long>(units[2]);
        score = boost::lexical_cast<int>(units[3]);
        std::vector<std::string> cates;
        boost::split(cates, units[4], boost::is_any_of("-"));
        BOOST_FOREACH(const std::string& s, cates) {
          int cate_index = boost::lexical_cast<int>(s);
          cate_seq.push_back(cate_index);
        }
      } catch (...) {
        LOG(LERROR) << "str2int cast error, line content:" << line;
        continue;
      }
      Node* node = Create(cate_seq);
      VideoShare* vs = new VideoShare();
      vs->set_md5(md5);
      vs->set_user_id(uid);
      vs->set_share_id(sid);
      vs->set_score(score);
      vs->set_thumb_url(thumb_url);
      vs->set_title(title);
      vs->set_node(node);
      if (node->AddVideoShare(vs)) {
        video_map_[md5] = vs;
      }
      else {
        delete vs;
        vs = 0;
      }
    }
  }
  LOG(INFO) << "video_map_.size():" << video_map_.size();  

  std::vector<std::string> ti_lines;
  xce::GetTextFileLines(video_tag_file_path_, ti_lines);
  BOOST_FOREACH(const std::string& line, ti_lines) {
    std::vector<std::string> units;
    boost::split(units, line, boost::is_any_of("\t"));
    if (units.size() == 2) {
      std::string str;
      int index = -100;
      bool ec = false;
      try {
        index = boost::lexical_cast<int>(units[1]);
        str = units[0];
      } catch (...) {
        ec = true;
        try {
          index = boost::lexical_cast<int>(units[0]);
        } catch (...) {}
        str = units[1];
      }
      str_index_map_.insert(std::make_pair(str, index));
      index_str_map_.insert(std::make_pair(index, str));
    }
  } 
  LOG(INFO) << "ti_lines.size():" << ti_lines.size() 
            << "\t str_index_map_.size():" << str_index_map_.size() 
            << "\t" << "index_str_map_.size():" << index_str_map_.size();

}

Node* Tree::Create(const std::vector<int>& path) {
  Node* node = root_;
  BOOST_FOREACH(int cate_index, path) {
    node = Create(node, cate_index);
  }
  return node;
}

Node* Tree::Create(Node* parent, int cate_index) {
  if (!parent) {
    return 0;
  }
  Node* child = SearchChild(parent, cate_index);
  if (child) {
    return child;
  }
  else {
    Node* new_child = new Node();
    new_child->set_index(cate_index);
    new_child->set_parent(parent);
    new_child->parent()->AddChild(new_child);
    return new_child;
  }
}

Node* Tree::SearchChild(Node* parent, int cate_index) {
  if (!parent) {
    return 0;
  }
  Node* node = 0;
  BOOST_FOREACH(Node* child, *parent->children()) {
    if (child->index() == cate_index) {
      node = child;
    }
  }
  return node;
}

Node* Tree::Search(Node* node, int cate_index) {
  if (!node) {
    return 0;
  }
  if (node->index() == cate_index) {
    return node;
  }
  std::list<Node*>* children = node->children();
  if (children && children->size() > 0) {
    BOOST_FOREACH(Node* child, *children) {
      Node* n = Search(child, cate_index);
      if (n) {
        return n;
      }
    }
  }
  return 0;
}


void Tree::Display() {
  std::hash_map<std::string, VideoShare*>::const_iterator it =  video_map_.begin();
  std::set<int> s;
  for (; it != video_map_.end(); ++it) {
    VideoShare* vs = it->second;
    Node* node = vs->node();
    if (std::find(s.begin(), s.end(), node->index()) == s.end()) {
      LOG(INFO) << "node->index():" << node->index() << " node->video_share_list()->size():" << node->video_share_list()->size();
      LOG(INFO) << "\t" << "node->children()->size():" << node->children()->size();
      s.insert(node->index());
    }
   
  }
}

}} // end xce::socialgraph



