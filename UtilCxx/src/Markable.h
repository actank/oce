#ifndef __MARKABLE_H__
#define __MARKABLE_H__

#include <map>

namespace MyUtil {

class Markable {
public:
  void mark(const std::string& key, const std::string& value) {
    marks_.insert(make_pair<std::string, std::string>(key, value));
  }

  std::map<std::string, std::string> demark() {
    std::map<std::string, std::string> result;
    result.swap(marks_);
    return result;
  }
  
  bool marked(const std::string& key) {
    return (!marks_.empty()) && (marks_.find(key) != marks_.end());
  }
private:
  std::map<std::string, std::string> marks_;
};

}
#endif
