#ifndef COMMON_FACTORY_H_
#define COMMON_FACTORY_H_

#include "socialgraph/friendfinderworker/algorithm/method/commonfriendcalculator.h"
#include "base/once.h"

namespace xce {
namespace socialgraph {

enum Method {
	KMerge, //k路归并计算二度好友
	HashMap //hashmap计算二度好友
};

class CommonFactory {
public:
	static CommonFactory& GetInstance();
	CommonFactory() {}
	virtual ~CommonFactory() {}

	CommonFriendCalculatorPtr Create(Method method);
};

}
}


#endif
