#ifndef WRITE_RECONNECT_H_
#define WRITE_RECONNECT_H_

#include <iostream>
#include <vector>

#include "socialgraph/socialgraphutil/calculator/calculator.h"

namespace xce {
namespace socialgraph {

class WriteReconnect : public Calculator {
public:
	WriteReconnect() {}
	void Detail();

private:
	void Init();
	void CreateWriteReconnectMapByTask(const std::map<int, std::vector<int> >& friend_list_map);
	void SetDistCache(const std::map<int, std::vector<int> >& write_reconnect_map);
};

}
}

#endif
