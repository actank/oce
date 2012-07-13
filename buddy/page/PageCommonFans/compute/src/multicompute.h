#ifndef _XCE_BUDDY_MULTI_COMPUTE_H_
#define _XCE_BUDDY_MULTI_COMPUTE_H_
#include "base/threadpool.h"

namespace xce {
namespace buddy {

class CMessage : public xce::Message {
public:
  CMessage(int uid);
  virtual ~CMessage();
	virtual void Run();
private:
	void Calculate(int);
  std::string Comb(std::string prefix, int id);
  void ReverseSort(std::map<int, std::vector<int> >& map1, std::map<int, std::vector<int> >& map2);
  int uid_;
};


}
}
#endif
