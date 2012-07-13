#ifndef _EXECUTER_H_ 
#define _EXECUTER_H_ 

#include <boost/shared_ptr.hpp>

#include "socialgraph/autogroup/algorithm/method/autogroup.h"

namespace xce {
namespace socialgraph {

//分组的执行者，具体分组方法的代理
class Executer {
public:
	Executer() {}

	~Executer() {}

	void SetAutoGroupInstance(AutoGroupPtr instance) {
		instance_ = instance;
	}

	void Grouping(int userid); 
	void Display();
	void GetGroupResult(Items& result);
	GroupStats GetGroupStats();

private:
	int userid_;
	//真正的执行对象
	AutoGroupPtr instance_;
};

}
}

#endif
