#ifndef LOADAREAPEER_H_
#define LOADAREAPEER_H_

#include "socialgraph/socialgraphutil/calculator/calculator.h"

namespace xce {
namespace socialgraph {

/*程序加载入口，在guide流程中给中学用户增加同区县、同入学年好友推荐*/
class LoadAreaPeer : public Calculator {
public:
	void Detail();

private:
	void Initialize();
};

}
}

#endif
