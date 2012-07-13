#include "socialgraph/socialgraphutil/algostuff.h"

namespace xce {

int Intersection(const std::vector<int>& t1, const std::vector<int>& t2,	std::vector<int>& result) {
	std::set<int> s1;
	s1.insert(t1.begin(), t1.end());
	std::set<int> s2;
	s2.insert(t2.begin(), t2.end());

	int size = std::min(s1.size(), s2.size());
	std::vector<int> r(size);
	std::vector<int>::iterator iter;
  iter = std::set_intersection(s1.begin(), s1.end(), 
                               s2.begin(), s2.end(),
                               r.begin());
	int n = int(iter - r.begin());
	result.insert(result.end(), r.begin(), r.begin() + n);
  return n;
}

int GetTextFileLines(const std::string& path, std::vector<std::string>& lines) {
  std::ifstream in(path.c_str());
  if (!in) {
    std::cerr << "cannot not load file:" << path << std::endl;
    return 0;
  }
  std::string line;
  std::getline(in, line);
  while (!line.empty()) {
    lines.push_back(line);
    line.clear();
    std::getline(in, line);
  }
  return lines.size();
}

}
